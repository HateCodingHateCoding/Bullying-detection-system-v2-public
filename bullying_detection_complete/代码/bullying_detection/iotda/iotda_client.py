#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
华为云IoTDA平台对接代码
Huawei Cloud IoTDA Integration

功能:
1. 设备注册和管理
2. 数据接收和存储到OBS
3. 规则引擎配置
4. 告警推送
5. 模型服务调用

作者: 三英战ICT团队
"""

import os
import json
import time
import hmac
import hashlib
import base64
from datetime import datetime
from typing import Optional, Dict, List, Callable
import logging

# 华为云SDK
try:
    from huaweicloudsdkcore.auth.credentials import BasicCredentials, GlobalCredentials
    from huaweicloudsdkcore.http.http_config import HttpConfig
    from huaweicloudsdkiotda.v5 import *
    from huaweicloudsdkiotda.v5.region.iotda_region import IoTDARegion
    HAS_HW_SDK = True
except ImportError:
    HAS_HW_SDK = False
    print("Warning: huaweicloudsdkiotda not installed")

# MQTT客户端
try:
    import paho.mqtt.client as mqtt
    HAS_MQTT = True
except ImportError:
    HAS_MQTT = False

# HTTP请求
import requests

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger('IoTDAClient')


# ============== 配置 ==============
class IoTDAConfig:
    """IoTDA配置"""
    # 区域
    REGION = 'cn-north-4'
    
    # 访问凭证 (从环境变量获取)
    AK = os.getenv('HUAWEICLOUD_AK', '')
    SK = os.getenv('HUAWEICLOUD_SK', '')
    PROJECT_ID = os.getenv('HUAWEICLOUD_PROJECT_ID', '')
    
    # IoTDA实例
    INSTANCE_ID = os.getenv('IOTDA_INSTANCE_ID', '')
    
    # MQTT接入
    MQTT_HOST = f'iot-mqtts.{REGION}.myhuaweicloud.com'
    MQTT_PORT = 8883
    
    # HTTP接入
    HTTP_ENDPOINT = f'https://iotda.{REGION}.myhuaweicloud.com'
    
    # OBS配置
    OBS_BUCKET = 'bullying-detection-data'
    OBS_ENDPOINT = f'obs.{REGION}.myhuaweicloud.com'
    
    # 产品配置
    PRODUCT_ID = 'bullying_detector'


# ============== 设备管理 ==============
class DeviceManager:
    """
    设备管理类
    用于管理IoTDA上的设备
    """
    
    def __init__(self, config: IoTDAConfig = None):
        self.config = config or IoTDAConfig()
        self.client = None
        
        if HAS_HW_SDK and self.config.AK and self.config.SK:
            self._init_client()
    
    def _init_client(self):
        """初始化华为云客户端"""
        credentials = BasicCredentials(self.config.AK, self.config.SK)
        
        self.client = IoTDAClient.new_builder() \
            .with_credentials(credentials) \
            .with_region(IoTDARegion.value_of(self.config.REGION)) \
            .build()
        
        logger.info("IoTDA client initialized")
    
    def create_product(self, product_name: str, description: str = '') -> Dict:
        """
        创建产品
        
        Args:
            product_name: 产品名称
            description: 产品描述
            
        Returns:
            产品信息
        """
        if not self.client:
            return {}
        
        try:
            # 定义服务
            services = [
                # 霸凌检测服务
                ServiceCapability(
                    service_id='bullying_detection',
                    service_type='detection',
                    properties=[
                        ServiceProperty(
                            property_name='probability',
                            data_type='decimal',
                            required=False,
                            method='R'
                        ),
                        ServiceProperty(
                            property_name='alarm_level',
                            data_type='int',
                            required=False,
                            method='R'
                        ),
                        ServiceProperty(
                            property_name='attention_weight',
                            data_type='decimal',
                            required=False,
                            method='R'
                        ),
                        ServiceProperty(
                            property_name='timestamp',
                            data_type='int',
                            required=False,
                            method='R'
                        ),
                        ServiceProperty(
                            property_name='location',
                            data_type='string',
                            required=False,
                            method='R'
                        ),
                        ServiceProperty(
                            property_name='device_id',
                            data_type='string',
                            required=False,
                            method='R'
                        )
                    ]
                ),
                # 报警服务
                ServiceCapability(
                    service_id='alarm',
                    service_type='alarm',
                    events=[
                        ServiceEvent(
                            event_type='bullying_detected',
                            paras=[
                                ServiceEventPara(para_name='probability', data_type='decimal'),
                                ServiceEventPara(para_name='level', data_type='string'),
                                ServiceEventPara(para_name='location', data_type='string'),
                                ServiceEventPara(para_name='device_id', data_type='string')
                            ]
                        )
                    ]
                ),
                # 命令服务
                ServiceCapability(
                    service_id='command',
                    service_type='command',
                    commands=[
                        ServiceCommand(
                            command_name='trigger_alarm',
                            paras=[
                                ServiceCommandPara(para_name='level', data_type='int'),
                                ServiceCommandPara(para_name='duration', data_type='int')
                            ]
                        ),
                        ServiceCommand(
                            command_name='update_config',
                            paras=[
                                ServiceCommandPara(para_name='threshold', data_type='decimal'),
                                ServiceCommandPara(para_name='cooldown', data_type='int')
                            ]
                        )
                    ]
                )
            ]
            
            request = CreateProductRequest()
            request.body = AddProduct(
                product_id=self.config.PRODUCT_ID,
                name=product_name,
                device_type='Sensor',
                protocol_type='MQTT',
                data_format='json',
                service_capabilities=services,
                description=description
            )
            
            response = self.client.create_product(request)
            logger.info(f"Product created: {response.product_id}")
            return response.to_dict()
            
        except Exception as e:
            logger.error(f"Create product failed: {e}")
            return {}
    
    def register_device(self, device_id: str, device_name: str, 
                       location: str = '') -> Dict:
        """
        注册设备
        
        Args:
            device_id: 设备ID
            device_name: 设备名称
            location: 设备位置
            
        Returns:
            设备信息（包含密钥）
        """
        if not self.client:
            return {}
        
        try:
            request = AddDeviceRequest()
            request.body = AddDevice(
                device_id=device_id,
                node_id=device_id,
                device_name=device_name,
                product_id=self.config.PRODUCT_ID,
                description=f'Location: {location}',
                extension_info={
                    'location': location
                }
            )
            
            response = self.client.add_device(request)
            
            logger.info(f"Device registered: {response.device_id}")
            
            return {
                'device_id': response.device_id,
                'secret': response.auth_info.secret if response.auth_info else '',
                'node_id': response.node_id
            }
            
        except Exception as e:
            logger.error(f"Register device failed: {e}")
            return {}
    
    def delete_device(self, device_id: str) -> bool:
        """删除设备"""
        if not self.client:
            return False
        
        try:
            request = DeleteDeviceRequest()
            request.device_id = device_id
            self.client.delete_device(request)
            logger.info(f"Device deleted: {device_id}")
            return True
        except Exception as e:
            logger.error(f"Delete device failed: {e}")
            return False
    
    def list_devices(self, product_id: str = None) -> List[Dict]:
        """列出设备"""
        if not self.client:
            return []
        
        try:
            request = ListDevicesRequest()
            if product_id:
                request.product_id = product_id
            
            response = self.client.list_devices(request)
            
            devices = []
            for device in response.devices:
                devices.append({
                    'device_id': device.device_id,
                    'device_name': device.device_name,
                    'status': device.status,
                    'product_id': device.product_id
                })
            
            return devices
            
        except Exception as e:
            logger.error(f"List devices failed: {e}")
            return []


# ============== 规则引擎 ==============
class RuleEngine:
    """
    规则引擎管理
    配置数据转发和告警规则
    """
    
    def __init__(self, config: IoTDAConfig = None):
        self.config = config or IoTDAConfig()
        self.client = None
        
        if HAS_HW_SDK and self.config.AK and self.config.SK:
            self._init_client()
    
    def _init_client(self):
        """初始化客户端"""
        credentials = BasicCredentials(self.config.AK, self.config.SK)
        
        self.client = IoTDAClient.new_builder() \
            .with_credentials(credentials) \
            .with_region(IoTDARegion.value_of(self.config.REGION)) \
            .build()
    
    def create_data_forwarding_rule(self, rule_name: str, 
                                    obs_bucket: str = None) -> Dict:
        """
        创建数据转发规则（转发到OBS）
        
        用于将设备上报的数据存储到OBS，供ModelArts训练使用
        """
        if not self.client:
            return {}
        
        obs_bucket = obs_bucket or self.config.OBS_BUCKET
        
        try:
            # 创建转发动作
            request = CreateRoutingRuleRequest()
            request.body = AddRuleReq(
                rule_name=rule_name,
                description='转发霸凌检测数据到OBS',
                select="*",
                where="",
                active=True
            )
            
            response = self.client.create_routing_rule(request)
            rule_id = response.rule_id
            
            # 添加转发目标（OBS）
            action_request = CreateRuleActionRequest()
            action_request.body = AddActionReq(
                rule_id=rule_id,
                channel='OBS',
                channel_detail={
                    'obs_bucket_name': obs_bucket,
                    'obs_location': 'cn-north-4',
                    'file_path': 'bullying_data/${deviceId}/${timestamp}.json'
                }
            )
            
            self.client.create_rule_action(action_request)
            
            logger.info(f"Data forwarding rule created: {rule_id}")
            return {'rule_id': rule_id}
            
        except Exception as e:
            logger.error(f"Create forwarding rule failed: {e}")
            return {}
    
    def create_alarm_rule(self, rule_name: str, threshold: float = 0.7,
                         webhook_url: str = None) -> Dict:
        """
        创建告警规则
        
        当检测概率超过阈值时触发告警
        """
        if not self.client:
            return {}
        
        try:
            # 创建规则
            request = CreateRoutingRuleRequest()
            request.body = AddRuleReq(
                rule_name=rule_name,
                description=f'霸凌检测告警 (阈值>{threshold})',
                select="*",
                where=f"notify_data.body.services[0].properties.probability > {threshold}",
                active=True
            )
            
            response = self.client.create_routing_rule(request)
            rule_id = response.rule_id
            
            # 添加告警动作
            if webhook_url:
                action_request = CreateRuleActionRequest()
                action_request.body = AddActionReq(
                    rule_id=rule_id,
                    channel='HTTP_FORWARDING',
                    channel_detail={
                        'http_forwarding': {
                            'url': webhook_url
                        }
                    }
                )
                self.client.create_rule_action(action_request)
            
            logger.info(f"Alarm rule created: {rule_id}")
            return {'rule_id': rule_id}
            
        except Exception as e:
            logger.error(f"Create alarm rule failed: {e}")
            return {}


# ============== MQTT设备客户端 ==============
class DeviceMQTTClient:
    """
    设备侧MQTT客户端
    用于设备连接IoTDA并上报数据
    """
    
    def __init__(self, device_id: str, device_secret: str, 
                 config: IoTDAConfig = None):
        self.config = config or IoTDAConfig()
        self.device_id = device_id
        self.device_secret = device_secret
        
        self.client = None
        self.connected = False
        self.message_callbacks: List[Callable] = []
        
    def _generate_password(self) -> str:
        """生成MQTT连接密码"""
        timestamp = str(int(time.time() * 1000))
        
        sign_content = timestamp
        signature = hmac.new(
            self.device_secret.encode('utf-8'),
            sign_content.encode('utf-8'),
            hashlib.sha256
        ).hexdigest()
        
        return f"{timestamp}_{signature}"
    
    def _on_connect(self, client, userdata, flags, rc):
        """连接回调"""
        if rc == 0:
            self.connected = True
            logger.info(f"Device {self.device_id} connected to IoTDA")
            
            # 订阅命令下发主题
            cmd_topic = f"$oc/devices/{self.device_id}/sys/commands/#"
            self.client.subscribe(cmd_topic)
            
        else:
            logger.error(f"Connection failed: {rc}")
    
    def _on_disconnect(self, client, userdata, rc):
        """断开回调"""
        self.connected = False
        logger.warning(f"Device disconnected: {rc}")
    
    def _on_message(self, client, userdata, msg):
        """消息回调"""
        try:
            payload = json.loads(msg.payload.decode('utf-8'))
            logger.debug(f"Received message: {msg.topic}")
            
            for callback in self.message_callbacks:
                callback(msg.topic, payload)
                
        except Exception as e:
            logger.error(f"Message handling error: {e}")
    
    def add_message_callback(self, callback: Callable):
        """添加消息回调"""
        self.message_callbacks.append(callback)
    
    def connect(self) -> bool:
        """连接到IoTDA"""
        if not HAS_MQTT:
            logger.error("paho-mqtt not installed")
            return False
        
        try:
            self.client = mqtt.Client(
                client_id=self.device_id,
                protocol=mqtt.MQTTv311
            )
            
            # 设置TLS
            self.client.tls_set()
            
            # 设置回调
            self.client.on_connect = self._on_connect
            self.client.on_disconnect = self._on_disconnect
            self.client.on_message = self._on_message
            
            # 设置认证
            password = self._generate_password()
            self.client.username_pw_set(self.device_id, password)
            
            # 连接
            self.client.connect(
                self.config.MQTT_HOST,
                self.config.MQTT_PORT,
                keepalive=60
            )
            
            self.client.loop_start()
            
            # 等待连接
            for _ in range(50):  # 5秒超时
                if self.connected:
                    return True
                time.sleep(0.1)
            
            return False
            
        except Exception as e:
            logger.error(f"Connect failed: {e}")
            return False
    
    def disconnect(self):
        """断开连接"""
        if self.client:
            self.client.loop_stop()
            self.client.disconnect()
            self.connected = False
    
    def report_properties(self, service_id: str, properties: Dict) -> bool:
        """
        上报属性
        
        Args:
            service_id: 服务ID
            properties: 属性字典
        """
        if not self.connected:
            return False
        
        topic = f"$oc/devices/{self.device_id}/sys/properties/report"
        
        payload = {
            'services': [{
                'service_id': service_id,
                'properties': properties
            }]
        }
        
        result = self.client.publish(topic, json.dumps(payload))
        return result.rc == mqtt.MQTT_ERR_SUCCESS
    
    def report_event(self, service_id: str, event_type: str, 
                    event_params: Dict) -> bool:
        """
        上报事件
        
        Args:
            service_id: 服务ID
            event_type: 事件类型
            event_params: 事件参数
        """
        if not self.connected:
            return False
        
        topic = f"$oc/devices/{self.device_id}/sys/events/up"
        
        payload = {
            'services': [{
                'service_id': service_id,
                'event_type': event_type,
                'event_time': datetime.utcnow().strftime('%Y%m%dT%H%M%SZ'),
                'paras': event_params
            }]
        }
        
        result = self.client.publish(topic, json.dumps(payload))
        return result.rc == mqtt.MQTT_ERR_SUCCESS
    
    def respond_command(self, request_id: str, result_code: int = 0,
                       response_name: str = '', paras: Dict = None) -> bool:
        """响应命令"""
        if not self.connected:
            return False
        
        topic = f"$oc/devices/{self.device_id}/sys/commands/response/request_id={request_id}"
        
        payload = {
            'result_code': result_code,
            'response_name': response_name,
            'paras': paras or {}
        }
        
        result = self.client.publish(topic, json.dumps(payload))
        return result.rc == mqtt.MQTT_ERR_SUCCESS


# ============== 数据服务 ==============
class DataService:
    """
    数据服务
    处理设备数据存储和查询
    """
    
    def __init__(self, config: IoTDAConfig = None):
        self.config = config or IoTDAConfig()
    
    def upload_training_data(self, data: Dict, filename: str) -> bool:
        """
        上传训练数据到OBS
        
        Args:
            data: 训练数据
            filename: 文件名
        """
        try:
            from obs import ObsClient
            
            obs_client = ObsClient(
                access_key_id=self.config.AK,
                secret_access_key=self.config.SK,
                server=f'https://{self.config.OBS_ENDPOINT}'
            )
            
            # 上传
            object_key = f'training_data/{filename}'
            response = obs_client.putContent(
                self.config.OBS_BUCKET,
                object_key,
                json.dumps(data)
            )
            
            if response.status < 300:
                logger.info(f"Data uploaded: {object_key}")
                return True
            else:
                logger.error(f"Upload failed: {response.reason}")
                return False
                
        except Exception as e:
            logger.error(f"Upload error: {e}")
            return False
    
    def get_historical_data(self, device_id: str, 
                           start_time: datetime, 
                           end_time: datetime) -> List[Dict]:
        """
        获取历史数据
        
        Args:
            device_id: 设备ID
            start_time: 开始时间
            end_time: 结束时间
        """
        # 这里需要从OBS或DIS获取历史数据
        # 简化实现
        return []


# ============== 告警服务 ==============
class AlarmService:
    """
    告警服务
    处理告警推送
    """
    
    def __init__(self, config: IoTDAConfig = None):
        self.config = config or IoTDAConfig()
        self.webhook_urls: List[str] = []
        self.sms_enabled = False
        self.email_enabled = False
    
    def add_webhook(self, url: str):
        """添加Webhook URL"""
        self.webhook_urls.append(url)
    
    def send_alarm(self, alarm_data: Dict) -> bool:
        """
        发送告警
        
        Args:
            alarm_data: 告警数据
        """
        success = True
        
        # Webhook推送
        for url in self.webhook_urls:
            try:
                response = requests.post(
                    url,
                    json=alarm_data,
                    timeout=5
                )
                if response.status_code != 200:
                    logger.warning(f"Webhook failed: {url}")
                    success = False
            except Exception as e:
                logger.error(f"Webhook error: {e}")
                success = False
        
        # 短信推送（需要配置SMN）
        if self.sms_enabled:
            # self._send_sms(alarm_data)
            pass
        
        # 邮件推送
        if self.email_enabled:
            # self._send_email(alarm_data)
            pass
        
        return success


# ============== 示例使用 ==============
def example_setup():
    """示例：初始化IoTDA"""
    config = IoTDAConfig()
    
    # 创建设备管理器
    device_manager = DeviceManager(config)
    
    # 创建产品
    device_manager.create_product(
        product_name='校园霸凌检测终端',
        description='多模态融合校园霸凌检测设备'
    )
    
    # 注册设备
    device_info = device_manager.register_device(
        device_id='node_001',
        device_name='教学楼A-3楼-男厕所',
        location='教学楼A, 3楼, 男厕所'
    )
    
    print(f"Device registered: {device_info}")
    
    # 配置规则引擎
    rule_engine = RuleEngine(config)
    
    # 创建数据转发规则（存到OBS用于训练）
    rule_engine.create_data_forwarding_rule(
        rule_name='bullying_data_forward',
        obs_bucket='bullying-detection-data'
    )
    
    # 创建告警规则
    rule_engine.create_alarm_rule(
        rule_name='high_probability_alarm',
        threshold=0.8,
        webhook_url='https://your-alarm-webhook.com/notify'
    )


def example_device():
    """示例：设备连接和数据上报"""
    config = IoTDAConfig()
    
    # 创建设备客户端
    client = DeviceMQTTClient(
        device_id='node_001',
        device_secret='your_device_secret',
        config=config
    )
    
    # 命令处理回调
    def handle_command(topic: str, payload: Dict):
        if 'trigger_alarm' in topic:
            level = payload.get('paras', {}).get('level', 1)
            duration = payload.get('paras', {}).get('duration', 5000)
            print(f"Received alarm command: level={level}, duration={duration}")
            
            # 这里触发本地报警
            # ...
            
            # 响应命令
            request_id = topic.split('request_id=')[-1]
            client.respond_command(request_id, 0, 'trigger_alarm_response')
    
    client.add_message_callback(handle_command)
    
    # 连接
    if client.connect():
        print("Connected to IoTDA")
        
        # 模拟上报数据
        for i in range(10):
            # 上报检测结果
            client.report_properties('bullying_detection', {
                'probability': 0.3 + i * 0.05,
                'alarm_level': 1 if i < 5 else 2,
                'attention_weight': 0.5,
                'timestamp': int(time.time() * 1000),
                'location': '教学楼A-3楼-男厕所',
                'device_id': 'node_001'
            })
            
            # 如果概率高，上报事件
            if i >= 7:
                client.report_event('alarm', 'bullying_detected', {
                    'probability': 0.3 + i * 0.05,
                    'level': 'HIGH',
                    'location': '教学楼A-3楼-男厕所',
                    'device_id': 'node_001'
                })
            
            time.sleep(1)
        
        client.disconnect()
    else:
        print("Connection failed")


if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description='IoTDA客户端')
    parser.add_argument('--setup', action='store_true', help='初始化设置')
    parser.add_argument('--device', action='store_true', help='模拟设备')
    args = parser.parse_args()
    
    if args.setup:
        example_setup()
    elif args.device:
        example_device()
    else:
        print("Usage: python iotda_client.py --setup|--device")
