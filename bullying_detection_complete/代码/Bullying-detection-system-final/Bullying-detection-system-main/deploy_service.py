#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
ModelArts 在线服务部署
Online Service Deployment for ModelArts

功能:
1. 模型部署到ModelArts在线服务
2. FunctionGraph函数封装
3. API调用接口

作者: 三英战ICT团队
"""

import os
import json
import time
import logging
from typing import Dict, Optional
import numpy as np

logging.basicConfig(level=logging.INFO)
logger = logging.getLogger('ModelArtsService')


# ============== 在线服务推理脚本 ==============
"""
以下代码需要作为 customize_service.py 上传到ModelArts

ModelArts在线服务要求:
1. 继承 model_service.XserviceBaseClass
2. 实现 _preprocess, _inference, _postprocess 方法
"""

CUSTOMIZE_SERVICE_CODE = '''
# customize_service.py - ModelArts在线服务推理脚本

import json
import numpy as np
import mindspore as ms
from mindspore import Tensor
from model_service.model_service import SingleNodeService

class BullyingDetectionService(SingleNodeService):
    """校园霸凌检测在线服务"""
    
    def __init__(self, model_name, model_path):
        super().__init__(model_name, model_path)
        self.model = None
        self.config = {
            'n_mels': 64,
            'audio_time_steps': 128,
            'vibration_seq_len': 200
        }
        
    def _preprocess(self, data):
        """
        数据预处理
        
        输入格式:
        {
            "audio": [[...]],  # 梅尔频谱图 (1, 64, 128)
            "imu": [[...]]     # IMU数据 (200, 6)
        }
        """
        try:
            # 解析输入
            if isinstance(data, str):
                data = json.loads(data)
            
            audio = np.array(data.get('audio', []), dtype=np.float32)
            imu = np.array(data.get('imu', []), dtype=np.float32)
            
            # 验证形状
            if audio.shape != (1, self.config['n_mels'], self.config['audio_time_steps']):
                # 尝试调整
                if len(audio.shape) == 2:
                    audio = audio.reshape(1, self.config['n_mels'], self.config['audio_time_steps'])
            
            if len(imu.shape) == 2:
                if imu.shape != (self.config['vibration_seq_len'], 6):
                    # 填充或截断
                    if imu.shape[0] > self.config['vibration_seq_len']:
                        imu = imu[:self.config['vibration_seq_len'], :]
                    else:
                        pad_len = self.config['vibration_seq_len'] - imu.shape[0]
                        imu = np.pad(imu, ((0, pad_len), (0, 0)))
            
            # 添加batch维度
            audio = audio.reshape(1, 1, self.config['n_mels'], self.config['audio_time_steps'])
            imu = imu.reshape(1, self.config['vibration_seq_len'], 6)
            
            return {
                'audio': Tensor(audio, ms.float32),
                'imu': Tensor(imu, ms.float32)
            }
            
        except Exception as e:
            logger.error(f"Preprocess error: {e}")
            raise
    
    def _inference(self, preprocessed_data):
        """模型推理"""
        audio = preprocessed_data['audio']
        imu = preprocessed_data['imu']
        
        # 执行推理
        probability, attention = self.model(audio, imu)
        
        return {
            'probability': probability.asnumpy(),
            'attention': attention.asnumpy()
        }
    
    def _postprocess(self, inference_result):
        """
        后处理
        
        输出格式:
        {
            "probability": 0.85,
            "alarm_level": "HIGH",
            "attention_weight": 0.6,
            "recommendation": "建议立即关注"
        }
        """
        probability = float(inference_result['probability'][0, 0])
        attention = float(inference_result['attention'][0, 0])
        
        # 确定报警级别
        if probability < 0.5:
            alarm_level = "NONE"
            recommendation = "正常状态"
        elif probability < 0.7:
            alarm_level = "LOW"
            recommendation = "存在轻微异常，建议关注"
        elif probability < 0.85:
            alarm_level = "MEDIUM"
            recommendation = "检测到异常行为，建议派人查看"
        else:
            alarm_level = "HIGH"
            recommendation = "高风险！建议立即处理"
        
        return json.dumps({
            'probability': round(probability, 4),
            'alarm_level': alarm_level,
            'attention_weight': round(attention, 4),
            'recommendation': recommendation
        }, ensure_ascii=False)
'''


# ============== FunctionGraph 函数代码 ==============
FUNCTION_GRAPH_CODE = '''
# index.py - FunctionGraph函数入口

import json
import requests
import os

# ModelArts在线服务地址
MODELARTS_ENDPOINT = os.environ.get('MODELARTS_ENDPOINT', '')
MODELARTS_TOKEN = os.environ.get('MODELARTS_TOKEN', '')

def handler(event, context):
    """
    FunctionGraph函数入口
    
    event格式:
    {
        "body": {
            "audio": [...],
            "imu": [...],
            "device_id": "node_001",
            "location": "教学楼A-3楼"
        }
    }
    """
    try:
        # 解析输入
        body = event.get('body', {})
        if isinstance(body, str):
            body = json.loads(body)
        
        audio = body.get('audio', [])
        imu = body.get('imu', [])
        device_id = body.get('device_id', 'unknown')
        location = body.get('location', 'unknown')
        
        # 调用ModelArts在线服务
        headers = {
            'Content-Type': 'application/json',
            'X-Auth-Token': MODELARTS_TOKEN
        }
        
        payload = {
            'audio': audio,
            'imu': imu
        }
        
        response = requests.post(
            MODELARTS_ENDPOINT,
            headers=headers,
            json=payload,
            timeout=10
        )
        
        if response.status_code != 200:
            return {
                'statusCode': 500,
                'body': json.dumps({'error': 'Model inference failed'})
            }
        
        result = response.json()
        
        # 添加设备信息
        result['device_id'] = device_id
        result['location'] = location
        
        # 如果需要触发告警
        if result.get('alarm_level') in ['MEDIUM', 'HIGH']:
            # 这里可以调用其他服务发送告警
            send_alarm_notification(result)
        
        return {
            'statusCode': 200,
            'body': json.dumps(result, ensure_ascii=False)
        }
        
    except Exception as e:
        return {
            'statusCode': 500,
            'body': json.dumps({'error': str(e)})
        }


def send_alarm_notification(result):
    """发送告警通知"""
    # 这里可以调用SMN服务发送短信/邮件
    # 或调用webhook
    pass
'''


# ============== ModelArts API客户端 ==============
class ModelArtsClient:
    """
    ModelArts API客户端
    用于管理模型和在线服务
    """
    
    def __init__(self, ak: str = None, sk: str = None, 
                 project_id: str = None, region: str = 'cn-north-4'):
        self.ak = ak or os.environ.get('HUAWEICLOUD_AK', '')
        self.sk = sk or os.environ.get('HUAWEICLOUD_SK', '')
        self.project_id = project_id or os.environ.get('HUAWEICLOUD_PROJECT_ID', '')
        self.region = region
        
        self.endpoint = f'https://modelarts.{region}.myhuaweicloud.com'
        self.token = None
        
    def _get_token(self) -> str:
        """获取IAM Token"""
        if self.token:
            return self.token
        
        import requests
        
        iam_url = f'https://iam.{self.region}.myhuaweicloud.com/v3/auth/tokens'
        
        payload = {
            "auth": {
                "identity": {
                    "methods": ["hw_ak_sk"],
                    "hw_ak_sk": {
                        "access": {
                            "key": self.ak
                        },
                        "secret": {
                            "key": self.sk
                        }
                    }
                },
                "scope": {
                    "project": {
                        "name": self.region
                    }
                }
            }
        }
        
        response = requests.post(iam_url, json=payload)
        if response.status_code == 201:
            self.token = response.headers.get('X-Subject-Token')
            return self.token
        else:
            raise Exception(f"Get token failed: {response.text}")
    
    def _request(self, method: str, path: str, data: Dict = None) -> Dict:
        """发送API请求"""
        import requests
        
        url = f'{self.endpoint}/v1/{self.project_id}{path}'
        headers = {
            'Content-Type': 'application/json',
            'X-Auth-Token': self._get_token()
        }
        
        if method == 'GET':
            response = requests.get(url, headers=headers)
        elif method == 'POST':
            response = requests.post(url, headers=headers, json=data)
        elif method == 'DELETE':
            response = requests.delete(url, headers=headers)
        else:
            raise ValueError(f"Unsupported method: {method}")
        
        if response.status_code >= 400:
            raise Exception(f"API error: {response.text}")
        
        return response.json() if response.text else {}
    
    def create_model(self, model_name: str, model_version: str,
                    obs_path: str, runtime: str = 'mindspore_1.8.0-cann_5.1.0-py_3.7-euler_2.8.3-aarch64') -> Dict:
        """
        创建模型
        
        Args:
            model_name: 模型名称
            model_version: 模型版本
            obs_path: OBS上的模型路径
            runtime: 运行时
        """
        data = {
            'model_name': model_name,
            'model_version': model_version,
            'source_location': obs_path,
            'model_type': 'MindSpore',
            'runtime': runtime,
            'description': '校园霸凌检测多模态融合模型'
        }
        
        return self._request('POST', '/models', data)
    
    def deploy_service(self, service_name: str, model_id: str,
                      instance_count: int = 1,
                      specification: str = 'modelarts.vm.cpu.2u') -> Dict:
        """
        部署在线服务
        
        Args:
            service_name: 服务名称
            model_id: 模型ID
            instance_count: 实例数量
            specification: 规格
        """
        data = {
            'service_name': service_name,
            'description': '校园霸凌检测在线推理服务',
            'infer_type': 'real-time',
            'config': [{
                'model_id': model_id,
                'weight': 100,
                'specification': specification,
                'instance_count': instance_count
            }]
        }
        
        return self._request('POST', '/services', data)
    
    def get_service_status(self, service_id: str) -> Dict:
        """获取服务状态"""
        return self._request('GET', f'/services/{service_id}')
    
    def delete_service(self, service_id: str) -> Dict:
        """删除服务"""
        return self._request('DELETE', f'/services/{service_id}')
    
    def call_service(self, service_url: str, audio_data: np.ndarray,
                    imu_data: np.ndarray) -> Dict:
        """
        调用在线服务
        
        Args:
            service_url: 服务URL
            audio_data: 音频特征数据
            imu_data: IMU特征数据
        """
        import requests
        
        payload = {
            'audio': audio_data.tolist(),
            'imu': imu_data.tolist()
        }
        
        headers = {
            'Content-Type': 'application/json',
            'X-Auth-Token': self._get_token()
        }
        
        response = requests.post(service_url, headers=headers, json=payload)
        return response.json()


# ============== 部署脚本 ==============
def deploy_to_modelarts(config: Dict):
    """
    完整的部署流程
    
    Args:
        config: 配置字典
    """
    logger.info("开始部署到ModelArts...")
    
    client = ModelArtsClient(
        ak=config.get('ak'),
        sk=config.get('sk'),
        project_id=config.get('project_id'),
        region=config.get('region', 'cn-north-4')
    )
    
    # 1. 上传模型到OBS
    logger.info("Step 1: 上传模型到OBS")
    obs_model_path = config.get('obs_model_path')
    # 这里假设模型已经上传到OBS
    
    # 2. 创建模型
    logger.info("Step 2: 在ModelArts创建模型")
    model_result = client.create_model(
        model_name='bullying_detection_model',
        model_version='v1.0',
        obs_path=obs_model_path
    )
    model_id = model_result.get('model_id')
    logger.info(f"模型创建成功: {model_id}")
    
    # 3. 部署在线服务
    logger.info("Step 3: 部署在线服务")
    service_result = client.deploy_service(
        service_name='bullying_detection_service',
        model_id=model_id,
        instance_count=1
    )
    service_id = service_result.get('service_id')
    logger.info(f"服务部署中: {service_id}")
    
    # 4. 等待服务就绪
    logger.info("Step 4: 等待服务就绪")
    for i in range(60):  # 最多等待10分钟
        status = client.get_service_status(service_id)
        if status.get('status') == 'running':
            logger.info("服务已就绪!")
            return {
                'model_id': model_id,
                'service_id': service_id,
                'service_url': status.get('access_address')
            }
        elif status.get('status') == 'failed':
            raise Exception(f"服务部署失败: {status.get('error_msg')}")
        
        time.sleep(10)
        logger.info(f"等待中... ({i+1}/60)")
    
    raise Exception("服务部署超时")


# ============== 测试代码 ==============
def test_service(service_url: str, token: str):
    """测试在线服务"""
    import requests
    
    # 生成测试数据
    test_audio = np.random.randn(1, 64, 128).astype(np.float32)
    test_imu = np.random.randn(200, 6).astype(np.float32)
    
    # 模拟霸凌场景
    test_audio[:, 40:60, 50:70] *= 3  # 高能量区域
    test_imu[100:120, :3] *= 5        # 冲击事件
    
    payload = {
        'audio': test_audio.tolist(),
        'imu': test_imu.tolist()
    }
    
    headers = {
        'Content-Type': 'application/json',
        'X-Auth-Token': token
    }
    
    response = requests.post(service_url, headers=headers, json=payload)
    
    print("测试结果:")
    print(json.dumps(response.json(), indent=2, ensure_ascii=False))


if __name__ == "__main__":
    import argparse
    
    parser = argparse.ArgumentParser(description='ModelArts服务部署')
    parser.add_argument('--deploy', action='store_true', help='部署服务')
    parser.add_argument('--test', action='store_true', help='测试服务')
    parser.add_argument('--service-url', type=str, help='服务URL')
    args = parser.parse_args()
    
    if args.deploy:
        config = {
            'ak': os.environ.get('HUAWEICLOUD_AK'),
            'sk': os.environ.get('HUAWEICLOUD_SK'),
            'project_id': os.environ.get('HUAWEICLOUD_PROJECT_ID'),
            'region': 'cn-north-4',
            'obs_model_path': 'obs://your-bucket/models/bullying_model/'
        }
        result = deploy_to_modelarts(config)
        print(f"部署完成: {result}")
        
    elif args.test and args.service_url:
        token = os.environ.get('HUAWEICLOUD_TOKEN', '')
        test_service(args.service_url, token)
    
    else:
        # 输出代码模板
        print("=" * 60)
        print("ModelArts 在线服务推理脚本 (customize_service.py)")
        print("=" * 60)
        print(CUSTOMIZE_SERVICE_CODE)
        print("\n" + "=" * 60)
        print("FunctionGraph 函数代码 (index.py)")
        print("=" * 60)
        print(FUNCTION_GRAPH_CODE)
