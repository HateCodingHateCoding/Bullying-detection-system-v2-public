/**
 * @file shell_port.c
 * @author Letter (NevermindZZT@gmail.com)
 * @brief
 * @version 0.1
 * @date 2019-02-22
 *
 * @copyright (c) 2019 Letter
 *
 */

#include "common.h"
#include "shell.h"
#include "hal_uart.h"
#include "ll_gpio.h"
#include "ll_rcc_dev.h"


#if (CONFIG_SHELL == 1)

static Shell shell;
static char * shellBuffer = NULL;
static HAL_Dev_t *shellDev;
static OSI_Mutex_t shellMutex;

#if CONFIG_SHELL_PM
static struct wakelock shell_lock;
#endif

/**
 * @brief 用户shell写
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际写入的数据长度
 */
short userShellWrite(char *data, unsigned short len)
{
    uint16_t ret = 0;

#if CONFIG_SHELL_UART
    ret = HAL_UART_Transmit(shellDev, (uint8_t *)data, len, CONFIG_SHELL_RECV_TIMEOUT);
#endif

    return ret;
}


/**
 * @brief 用户shell读
 *
 * @param data 数据
 * @param len 数据长度
 *
 * @return short 实际读取到
 */
short userShellRead(char *data, unsigned short len)
{
    uint16_t ret = 0;

#if CONFIG_SHELL_UART
    ret = HAL_UART_Receive(shellDev, (uint8_t *)data, len, OSI_WAIT_FOREVER);
#endif

#if CONFIG_SHELL_PM
    pm_wake_lock_timeout(&shell_lock, CONFIG_SHELL_PM_LOCK_TO);
#endif
    return ret;
}

static void userShellOutChar(char character, void* buffer, size_t idx, size_t maxlen)
{
#if CONFIG_SHELL_UART
    if (character == '\n') {
        char tmp = '\r';
        HAL_UART_Transmit(shellDev, (uint8_t *)&tmp, 1, CONFIG_SHELL_RECV_TIMEOUT);
    }
    HAL_UART_Transmit(shellDev, (uint8_t *)&character, 1, CONFIG_SHELL_RECV_TIMEOUT);

#endif
}


/**
 * @brief 用户shell上锁
 *
 * @param shell shell
 *
 * @return int 0
 */
int userShellLock(Shell *shell)
{
    OSI_MutexLock(&shellMutex, OSI_WAIT_FOREVER);

    return 0;
}

/**
 * @brief 用户shell解锁
 *
 * @param shell shell
 *
 * @return int 0
 */
int userShellUnlock(Shell *shell)
{
    OSI_MutexUnlock(&shellMutex);

    return 0;
}

#if CONFIG_SHELL_PM
static void shell_uart_wakeup_callback(HAL_Dev_t * pDevice, void *arg)
{
    pm_wake_lock_timeout(&shell_lock, CONFIG_SHELL_PM_LOCK_TO);
}

static HAL_Callback_t shell_uart_wakeup = {
    .arg = NULL,
    .cb = shell_uart_wakeup_callback,
};
#endif

void shell_thread(void * param)
{
    shell.write = userShellWrite;
    shell.read = userShellRead;
    shell.outChar = userShellOutChar;
    shell.lock = userShellLock;
    shell.unlock = userShellUnlock;

    shellInit(&shell, shellBuffer, CONFIG_SHELL_BUFF_SIZE);

    while (1) {
        shellTask((void *)&shell);
    }
}
/**
 * @brief 用户shell初始化
 *
 */
int Shell_Init(void)
{
    OSI_Thread_t xHandle;
    OSI_Status_t os_status = OSI_STATUS_OK;

    OSI_ThreadSetInvalid(&xHandle);
    os_status = OSI_ThreadCreate(&xHandle,
                        "shell",
                        shell_thread,
                        NULL,
                        CONFIG_SHELL_TASK_PRIORITY,
                        CONFIG_SHELL_TASK_STACK_SIZE);
    if (os_status != OSI_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    OSI_MutexSetInvalid(&shellMutex);
    os_status = OSI_MutexCreate(&shellMutex);
    if (os_status != OSI_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

    shellBuffer = OSI_Malloc(CONFIG_SHELL_BUFF_SIZE);
    if (os_status != OSI_STATUS_OK) {
        return HAL_STATUS_ERROR;
    }

#if CONFIG_SHELL_UART
    UART_InitParam_t UartParam = {
        .baudRate = CONFIG_SHELL_UART_BAUDRATE,
        .parity = UART_PARITY_NONE,
        .stopBits = UART_STOP_BIT_1,
        .dataBits = UART_DATA_WIDTH_8,
        .autoFlowCtrl = UART_FLOW_CTRL_NONE
    };

    UART_ID_t uart_index = CONFIG_SHELL_UART_NUM;
    shellDev = HAL_UART_Init(uart_index, &UartParam);
    if (shellDev == NULL) {
        return HAL_STATUS_ERROR;
    }
    HAL_UART_Open(shellDev);

#if (CONFIG_SHELL_PM)
    uint8_t enable = 1;
    HAL_UART_ExtControl(shellDev, UART_PARAM_WAKEUP_CALLBACK, &shell_uart_wakeup);
    HAL_UART_ExtControl(shellDev, UART_PARAM_WAKEUP_CTRL, &enable);
    //HAL_UART_PM_Register(shellDev, &shell_uart_wakeup);
#endif

    HAL_UART_SetTransferMode(shellDev, UART_DIR_TX, UART_TRANS_MODE_NOMA);
    HAL_UART_SetTransferMode(shellDev, UART_DIR_RX, UART_TRANS_MODE_INTERRUPT);
#else
    #error "Not define shell communication."
#endif

    return OSI_STATUS_OK;
}

#endif /* CONFIG_SHELL */
