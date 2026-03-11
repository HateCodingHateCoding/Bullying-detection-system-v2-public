/**
 * K01 端侧数据采集代码
 * Device-side data collection for HiSilicon K01 MCU
 * 
 * 功能:
 * 1. 采集陀螺仪/加速度计数据 (MPU6050/BMI160)
 * 2. 采集全向麦克风声音数据
 * 3. 通过星闪SLE发送数据到香橙派
 * 4. 控制蜂鸣器和LED灯带报警
 * 
 * 作者: 三英战ICT团队
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

// HiSilicon SDK头文件
#include "hi_types.h"
#include "hi_time.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_i2c.h"
#include "hi_spi.h"
#include "hi_adc.h"
#include "hi_pwm.h"
#include "hi_uart.h"
#include "hi_watchdog.h"

// 星闪SLE头文件
#include "sle_device.h"
#include "sle_connection.h"
#include "sle_transmit.h"

// ============== 配置参数 ==============
#define CONFIG_AUDIO_SAMPLE_RATE    16000   // 音频采样率 Hz
#define CONFIG_IMU_SAMPLE_RATE      100     // IMU采样率 Hz
#define CONFIG_AUDIO_BUFFER_SIZE    1024    // 音频缓冲区大小
#define CONFIG_IMU_BUFFER_SIZE      64      // IMU缓冲区大小

#define CONFIG_I2C_BAUDRATE         400000  // I2C速率 400kHz
#define CONFIG_UART_BAUDRATE        115200  // UART波特率

// GPIO引脚定义
#define PIN_BUZZER          HI_GPIO_IDX_7   // 蜂鸣器
#define PIN_LED_DATA        HI_GPIO_IDX_8   // LED灯带数据线
#define PIN_LED_ENABLE      HI_GPIO_IDX_9   // LED使能
#define PIN_IMU_INT         HI_GPIO_IDX_10  // IMU中断

// I2C地址
#define MPU6050_ADDR        0x68
#define BMI160_ADDR         0x69

// ============== 数据结构 ==============

// 数据包类型
typedef enum {
    PACKET_TYPE_IMU = 0x01,
    PACKET_TYPE_AUDIO = 0x02,
    PACKET_TYPE_SOUND_LEVEL = 0x03,
    PACKET_TYPE_STATUS = 0x04,
    PACKET_TYPE_ALARM_CMD = 0x05
} PacketType;

// IMU数据
typedef struct {
    float accel_x;
    float accel_y;
    float accel_z;
    float gyro_x;
    float gyro_y;
    float gyro_z;
    uint32_t timestamp;
} IMUData;

// 音频数据包
typedef struct {
    int16_t samples[CONFIG_AUDIO_BUFFER_SIZE];
    uint16_t num_samples;
    uint32_t timestamp;
} AudioPacket;

// 报警命令
typedef struct {
    uint8_t level;      // 报警级别 0-3
    uint16_t duration;  // 持续时间 ms
    uint8_t pattern;    // 报警模式
} AlarmCommand;

// 系统状态
typedef struct {
    bool sle_connected;
    bool imu_ready;
    bool audio_ready;
    uint32_t last_heartbeat;
    uint32_t packet_sent;
    uint32_t packet_failed;
} SystemStatus;

// ============== 全局变量 ==============
static SystemStatus g_status = {0};
static IMUData g_imu_buffer[CONFIG_IMU_BUFFER_SIZE];
static uint16_t g_imu_buffer_head = 0;
static AudioPacket g_audio_packet;
static bool g_alarm_active = false;

// ============== IMU驱动 ==============

/**
 * 初始化MPU6050
 */
static int mpu6050_init(void)
{
    uint8_t buf[2];
    int ret;
    
    // 唤醒MPU6050
    buf[0] = 0x6B;  // PWR_MGMT_1
    buf[1] = 0x00;
    ret = hi_i2c_write(HI_I2C_IDX_0, MPU6050_ADDR, buf, 2);
    if (ret != HI_ERR_SUCCESS) {
        printf("MPU6050 wake up failed: %d\n", ret);
        return ret;
    }
    
    hi_udelay(100000);  // 等待100ms
    
    // 配置采样率 (1kHz / (1 + SMPLRT_DIV) = 100Hz)
    buf[0] = 0x19;  // SMPLRT_DIV
    buf[1] = 9;     // 100Hz
    hi_i2c_write(HI_I2C_IDX_0, MPU6050_ADDR, buf, 2);
    
    // 配置数字低通滤波器
    buf[0] = 0x1A;  // CONFIG
    buf[1] = 0x03;  // DLPF 44Hz
    hi_i2c_write(HI_I2C_IDX_0, MPU6050_ADDR, buf, 2);
    
    // 配置陀螺仪量程 ±500°/s
    buf[0] = 0x1B;  // GYRO_CONFIG
    buf[1] = 0x08;
    hi_i2c_write(HI_I2C_IDX_0, MPU6050_ADDR, buf, 2);
    
    // 配置加速度计量程 ±8g
    buf[0] = 0x1C;  // ACCEL_CONFIG
    buf[1] = 0x10;
    hi_i2c_write(HI_I2C_IDX_0, MPU6050_ADDR, buf, 2);
    
    // 启用数据就绪中断
    buf[0] = 0x38;  // INT_ENABLE
    buf[1] = 0x01;
    hi_i2c_write(HI_I2C_IDX_0, MPU6050_ADDR, buf, 2);
    
    printf("MPU6050 initialized\n");
    g_status.imu_ready = true;
    return HI_ERR_SUCCESS;
}

/**
 * 读取MPU6050数据
 */
static int mpu6050_read(IMUData *data)
{
    uint8_t buf[14];
    uint8_t reg = 0x3B;  // ACCEL_XOUT_H
    int ret;
    
    // 读取14字节数据 (加速度6字节 + 温度2字节 + 陀螺仪6字节)
    ret = hi_i2c_write_read(HI_I2C_IDX_0, MPU6050_ADDR, &reg, 1, buf, 14);
    if (ret != HI_ERR_SUCCESS) {
        return ret;
    }
    
    // 解析加速度数据 (量程 ±8g, 灵敏度 4096 LSB/g)
    int16_t raw_ax = (buf[0] << 8) | buf[1];
    int16_t raw_ay = (buf[2] << 8) | buf[3];
    int16_t raw_az = (buf[4] << 8) | buf[5];
    
    data->accel_x = raw_ax / 4096.0f * 9.8f;  // 转换为 m/s²
    data->accel_y = raw_ay / 4096.0f * 9.8f;
    data->accel_z = raw_az / 4096.0f * 9.8f;
    
    // 解析陀螺仪数据 (量程 ±500°/s, 灵敏度 65.5 LSB/(°/s))
    int16_t raw_gx = (buf[8] << 8) | buf[9];
    int16_t raw_gy = (buf[10] << 8) | buf[11];
    int16_t raw_gz = (buf[12] << 8) | buf[13];
    
    data->gyro_x = raw_gx / 65.5f;  // °/s
    data->gyro_y = raw_gy / 65.5f;
    data->gyro_z = raw_gz / 65.5f;
    
    data->timestamp = hi_get_milli_seconds();
    
    return HI_ERR_SUCCESS;
}

// ============== 音频采集 ==============

// ADC DMA缓冲区
static int16_t g_adc_buffer[CONFIG_AUDIO_BUFFER_SIZE * 2];
static volatile bool g_adc_buffer_ready = false;

/**
 * 初始化音频ADC
 */
static int audio_init(void)
{
    // 配置ADC
    hi_adc_init();
    
    // 配置GPIO为ADC输入
    hi_io_set_func(HI_IO_NAME_GPIO_5, HI_IO_FUNC_GPIO_5_ADC0);
    
    printf("Audio ADC initialized\n");
    g_status.audio_ready = true;
    return HI_ERR_SUCCESS;
}

/**
 * 采集音频数据
 */
static int audio_capture(AudioPacket *packet)
{
    uint16_t adc_value;
    int ret;
    
    for (int i = 0; i < CONFIG_AUDIO_BUFFER_SIZE; i++) {
        // 读取ADC值
        ret = hi_adc_read(HI_ADC_CHANNEL_0, &adc_value, 
                          HI_ADC_EQU_MODEL_4, HI_ADC_CUR_BAIS_DEFAULT, 10);
        if (ret != HI_ERR_SUCCESS) {
            packet->num_samples = i;
            return ret;
        }
        
        // 转换为有符号16位 (-32768 to 32767)
        packet->samples[i] = (int16_t)(adc_value - 2048) * 16;
        
        // 等待采样间隔 (16kHz = 62.5us)
        hi_udelay(62);
    }
    
    packet->num_samples = CONFIG_AUDIO_BUFFER_SIZE;
    packet->timestamp = hi_get_milli_seconds();
    
    return HI_ERR_SUCCESS;
}

/**
 * 计算声压级
 */
static float calculate_sound_level(AudioPacket *packet)
{
    if (packet->num_samples == 0) return 0.0f;
    
    // 计算RMS
    int64_t sum_sq = 0;
    for (int i = 0; i < packet->num_samples; i++) {
        int32_t sample = packet->samples[i];
        sum_sq += sample * sample;
    }
    
    float rms = sqrtf((float)sum_sq / packet->num_samples);
    
    // 转换为dB (参考值 32768)
    float db = 20.0f * log10f(rms / 32768.0f + 1e-10f) + 96.0f;
    
    return db;
}

// ============== 星闪SLE通信 ==============

static sle_dev_handle_t g_sle_handle = 0;
static uint8_t g_tx_buffer[512];

/**
 * SLE连接回调
 */
static void sle_connect_callback(sle_dev_handle_t handle, sle_conn_state_t state)
{
    if (state == SLE_CONN_STATE_CONNECTED) {
        printf("SLE connected\n");
        g_status.sle_connected = true;
        g_sle_handle = handle;
    } else {
        printf("SLE disconnected\n");
        g_status.sle_connected = false;
    }
}

/**
 * SLE数据接收回调
 */
static void sle_data_callback(sle_dev_handle_t handle, uint8_t *data, uint16_t len)
{
    if (len < 1) return;
    
    // 解析命令
    uint8_t cmd_type = data[0];
    
    if (cmd_type == PACKET_TYPE_ALARM_CMD && len >= sizeof(AlarmCommand) + 1) {
        AlarmCommand *cmd = (AlarmCommand *)(data + 1);
        trigger_alarm(cmd->level, cmd->duration, cmd->pattern);
    }
}

/**
 * 初始化星闪SLE
 */
static int sle_init(void)
{
    sle_dev_config_t config = {
        .device_name = "K01_Sensor",
        .device_name_len = 10,
        .connect_callback = sle_connect_callback,
        .data_callback = sle_data_callback
    };
    
    int ret = sle_device_init(&config);
    if (ret != 0) {
        printf("SLE init failed: %d\n", ret);
        return ret;
    }
    
    // 开始广播
    sle_start_advertise();
    
    printf("SLE initialized, waiting for connection...\n");
    return HI_ERR_SUCCESS;
}

/**
 * 发送数据包
 */
static int send_packet(PacketType type, void *data, uint16_t data_len)
{
    if (!g_status.sle_connected) {
        g_status.packet_failed++;
        return -1;
    }
    
    // 构造数据包
    // 包头: AA 55
    // 长度: 2字节
    // 类型: 1字节
    // 时间戳: 4字节
    // 数据: N字节
    
    uint16_t packet_len = 1 + 4 + data_len;
    uint16_t total_len = 4 + packet_len;
    
    if (total_len > sizeof(g_tx_buffer)) {
        return -1;
    }
    
    g_tx_buffer[0] = 0xAA;
    g_tx_buffer[1] = 0x55;
    g_tx_buffer[2] = packet_len & 0xFF;
    g_tx_buffer[3] = (packet_len >> 8) & 0xFF;
    g_tx_buffer[4] = type;
    
    uint32_t timestamp = hi_get_milli_seconds();
    memcpy(&g_tx_buffer[5], &timestamp, 4);
    
    if (data && data_len > 0) {
        memcpy(&g_tx_buffer[9], data, data_len);
    }
    
    // 发送
    int ret = sle_transmit_data(g_sle_handle, g_tx_buffer, total_len);
    
    if (ret == 0) {
        g_status.packet_sent++;
    } else {
        g_status.packet_failed++;
    }
    
    return ret;
}

/**
 * 发送IMU数据
 */
static int send_imu_data(IMUData *data)
{
    float imu_values[6] = {
        data->accel_x, data->accel_y, data->accel_z,
        data->gyro_x, data->gyro_y, data->gyro_z
    };
    
    return send_packet(PACKET_TYPE_IMU, imu_values, sizeof(imu_values));
}

/**
 * 发送音频数据
 */
static int send_audio_data(AudioPacket *packet)
{
    return send_packet(PACKET_TYPE_AUDIO, packet->samples, 
                       packet->num_samples * sizeof(int16_t));
}

/**
 * 发送声压级
 */
static int send_sound_level(float level)
{
    return send_packet(PACKET_TYPE_SOUND_LEVEL, &level, sizeof(float));
}

// ============== 报警控制 ==============

/**
 * PWM控制蜂鸣器
 */
static void buzzer_control(bool on, uint16_t freq)
{
    if (on && freq > 0) {
        uint16_t duty = 50;  // 50% 占空比
        hi_pwm_start(HI_PWM_PORT_0, duty, freq);
    } else {
        hi_pwm_stop(HI_PWM_PORT_0);
    }
}

/**
 * LED灯带控制 (WS2812)
 */
static void led_strip_set_color(uint8_t r, uint8_t g, uint8_t b, uint8_t count)
{
    // WS2812 协议实现
    // 这里简化为GPIO控制
    if (r > 0 || g > 0 || b > 0) {
        hi_gpio_set_output(PIN_LED_ENABLE, HI_GPIO_VALUE1);
        hi_gpio_set_output(PIN_LED_DATA, HI_GPIO_VALUE1);
    } else {
        hi_gpio_set_output(PIN_LED_ENABLE, HI_GPIO_VALUE0);
        hi_gpio_set_output(PIN_LED_DATA, HI_GPIO_VALUE0);
    }
}

/**
 * 触发报警
 */
static void trigger_alarm(uint8_t level, uint16_t duration, uint8_t pattern)
{
    g_alarm_active = true;
    
    printf("Alarm triggered: level=%d, duration=%d, pattern=%d\n", 
           level, duration, pattern);
    
    uint32_t start_time = hi_get_milli_seconds();
    
    while (hi_get_milli_seconds() - start_time < duration) {
        switch (level) {
            case 3:  // 高级别 - 快速闪烁+高频蜂鸣
                buzzer_control(true, 2000);
                led_strip_set_color(255, 0, 0, 10);
                hi_udelay(100000);  // 100ms
                buzzer_control(false, 0);
                led_strip_set_color(0, 0, 0, 10);
                hi_udelay(100000);
                break;
                
            case 2:  // 中级别 - 慢闪烁+中频蜂鸣
                buzzer_control(true, 1500);
                led_strip_set_color(255, 128, 0, 10);
                hi_udelay(300000);  // 300ms
                buzzer_control(false, 0);
                led_strip_set_color(0, 0, 0, 10);
                hi_udelay(300000);
                break;
                
            case 1:  // 低级别 - 常亮+短蜂鸣
                buzzer_control(true, 1000);
                led_strip_set_color(255, 255, 0, 10);
                hi_udelay(500000);  // 500ms
                buzzer_control(false, 0);
                hi_udelay(500000);
                break;
                
            default:
                break;
        }
        
        // 检查是否需要中断
        // ...
    }
    
    // 停止报警
    buzzer_control(false, 0);
    led_strip_set_color(0, 0, 0, 10);
    g_alarm_active = false;
}

// ============== GPIO初始化 ==============

static int gpio_init(void)
{
    // 蜂鸣器
    hi_io_set_func(PIN_BUZZER, HI_IO_FUNC_GPIO_7_PWM0_OUT);
    hi_gpio_set_dir(PIN_BUZZER, HI_GPIO_DIR_OUT);
    hi_pwm_init(HI_PWM_PORT_0);
    
    // LED控制
    hi_io_set_func(PIN_LED_DATA, HI_IO_FUNC_GPIO_8_GPIO);
    hi_gpio_set_dir(PIN_LED_DATA, HI_GPIO_DIR_OUT);
    hi_io_set_func(PIN_LED_ENABLE, HI_IO_FUNC_GPIO_9_GPIO);
    hi_gpio_set_dir(PIN_LED_ENABLE, HI_GPIO_DIR_OUT);
    
    // IMU中断
    hi_io_set_func(PIN_IMU_INT, HI_IO_FUNC_GPIO_10_GPIO);
    hi_gpio_set_dir(PIN_IMU_INT, HI_GPIO_DIR_IN);
    
    printf("GPIO initialized\n");
    return HI_ERR_SUCCESS;
}

// ============== I2C初始化 ==============

static int i2c_init(void)
{
    hi_i2c_init(HI_I2C_IDX_0, CONFIG_I2C_BAUDRATE);
    
    printf("I2C initialized\n");
    return HI_ERR_SUCCESS;
}

// ============== 主任务 ==============

// IMU采样任务
static void imu_task(void *arg)
{
    IMUData data;
    uint32_t last_sample_time = 0;
    uint32_t sample_interval = 1000 / CONFIG_IMU_SAMPLE_RATE;  // ms
    
    while (1) {
        uint32_t current_time = hi_get_milli_seconds();
        
        if (current_time - last_sample_time >= sample_interval) {
            last_sample_time = current_time;
            
            if (mpu6050_read(&data) == HI_ERR_SUCCESS) {
                // 存入缓冲区
                g_imu_buffer[g_imu_buffer_head] = data;
                g_imu_buffer_head = (g_imu_buffer_head + 1) % CONFIG_IMU_BUFFER_SIZE;
                
                // 发送数据
                send_imu_data(&data);
            }
        }
        
        hi_udelay(1000);  // 1ms
    }
}

// 音频采样任务
static void audio_task(void *arg)
{
    AudioPacket packet;
    
    while (1) {
        if (audio_capture(&packet) == HI_ERR_SUCCESS) {
            // 发送音频数据
            send_audio_data(&packet);
            
            // 计算并发送声压级
            float sound_level = calculate_sound_level(&packet);
            send_sound_level(sound_level);
        }
        
        hi_udelay(10000);  // 10ms
    }
}

// 状态监控任务
static void monitor_task(void *arg)
{
    while (1) {
        // 发送心跳
        g_status.last_heartbeat = hi_get_milli_seconds();
        send_packet(PACKET_TYPE_STATUS, &g_status, sizeof(SystemStatus));
        
        // 打印状态
        printf("Status: SLE=%d, IMU=%d, Audio=%d, Sent=%lu, Failed=%lu\n",
               g_status.sle_connected, g_status.imu_ready, g_status.audio_ready,
               g_status.packet_sent, g_status.packet_failed);
        
        // 喂狗
        hi_watchdog_feed();
        
        hi_sleep(5000);  // 5秒
    }
}

// ============== 主函数 ==============

int main(void)
{
    printf("\n==========================================\n");
    printf("校园霸凌检测 - K01端侧程序\n");
    printf("==========================================\n\n");
    
    // 初始化看门狗
    hi_watchdog_init(30);  // 30秒超时
    
    // 初始化各模块
    gpio_init();
    i2c_init();
    mpu6050_init();
    audio_init();
    sle_init();
    
    // 等待SLE连接
    printf("Waiting for SLE connection...\n");
    while (!g_status.sle_connected) {
        hi_watchdog_feed();
        hi_sleep(100);
    }
    printf("SLE connected, starting data collection\n");
    
    // 创建任务
    // 注意: 实际实现需要使用RTOS任务创建API
    // 这里简化为顺序执行
    
    // 主循环
    uint32_t imu_last = 0;
    uint32_t audio_last = 0;
    uint32_t monitor_last = 0;
    
    while (1) {
        uint32_t now = hi_get_milli_seconds();
        
        // IMU采样 (100Hz = 10ms间隔)
        if (now - imu_last >= 10) {
            imu_last = now;
            IMUData data;
            if (mpu6050_read(&data) == HI_ERR_SUCCESS) {
                send_imu_data(&data);
            }
        }
        
        // 音频采样 (约64ms一个缓冲区)
        if (now - audio_last >= 64) {
            audio_last = now;
            AudioPacket packet;
            if (audio_capture(&packet) == HI_ERR_SUCCESS) {
                send_audio_data(&packet);
                
                // 声压级
                float db = calculate_sound_level(&packet);
                if (db > 70.0f) {  // 仅当声音较大时发送
                    send_sound_level(db);
                }
            }
        }
        
        // 状态监控 (每5秒)
        if (now - monitor_last >= 5000) {
            monitor_last = now;
            g_status.last_heartbeat = now;
            
            printf("[%lu] Sent:%lu Failed:%lu\n", 
                   now, g_status.packet_sent, g_status.packet_failed);
            
            hi_watchdog_feed();
        }
        
        hi_udelay(100);  // 0.1ms
    }
    
    return 0;
}
