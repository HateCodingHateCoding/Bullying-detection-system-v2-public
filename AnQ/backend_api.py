from flask import Flask, request, jsonify
from flask_cors import CORS
import json
from datetime import datetime

app = Flask(__name__)
CORS(app)

# 存储最新告警(生产环境应使用数据库)
latest_alarm = None

@app.route('/api/alarms/receive', methods=['POST'])
def receive_alarm():
    """接收华为云IoTDA推送的告警数据"""
    global latest_alarm

    try:
        data = request.json
        # 华为云推送格式解析
        if 'services' in data:
            service = data['services'][0]
            properties = service['properties']

            latest_alarm = {
                'alarmType': properties.get('alarmType'),
                'confidence': properties.get('confidence'),
                'timestamp': properties.get('timestamp'),
                'location': properties.get('location'),
                'audioLevel': properties.get('audioLevel'),
                'radarData': properties.get('radarData', ''),
                'evidenceUrl': properties.get('evidenceUrl', '')
            }

            print(f"📥 收到告警: {latest_alarm['alarmType']}, 置信度: {latest_alarm['confidence']:.1%}, 证据: {latest_alarm['evidenceUrl']}")
            return jsonify({'status': 'success'}), 200
    except Exception as e:
        print(f"❌ 处理告警失败: {e}")
        return jsonify({'status': 'error', 'message': str(e)}), 500

@app.route('/api/alarms/latest', methods=['GET'])
def get_latest_alarm():
    """前端轮询获取最新告警"""
    if latest_alarm:
        return jsonify(latest_alarm), 200
    return jsonify(None), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
