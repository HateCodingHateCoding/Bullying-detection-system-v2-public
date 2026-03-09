import paho.mqtt.client as mqtt
import json
import time
import ssl

class HuaweiIoTUploader:
    def __init__(self, device_id, secret, iot_platform_url):
        self.device_id = device_id
        self.secret = secret
        self.broker = iot_platform_url
        self.port = 8883
        self.client = None
        self.connected = False

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            self.connected = True
            print(f"✅ IoT连接成功: {self.device_id}")
        else:
            print(f"❌ IoT连接失败, code: {rc}")

    def connect(self):
        self.client = mqtt.Client(client_id=self.device_id)
        self.client.username_pw_set(self.device_id, self.secret)
        self.client.tls_set(cert_reqs=ssl.CERT_REQUIRED, tls_version=ssl.PROTOCOL_TLSv1_2)
        self.client.on_connect = self.on_connect

        try:
            self.client.connect(self.broker, self.port, 60)
            self.client.loop_start()
            time.sleep(2)
        except Exception as e:
            print(f"❌ IoT连接异常: {e}")

    def upload_alarm(self, alarm_type, confidence, location, audio_level, radar_summary="", evidence_url=""):
        if not self.connected:
            print("⚠️ IoT未连接，跳过上报")
            return

        topic = f"$oc/devices/{self.device_id}/sys/properties/report"
        payload = {
            "services": [{
                "service_id": "AlarmService",
                "properties": {
                    "alarmType": alarm_type,
                    "confidence": round(confidence, 3),
                    "timestamp": int(time.time() * 1000),
                    "location": location,
                    "audioLevel": audio_level,
                    "radarData": radar_summary,
                    "evidenceUrl": evidence_url
                }
            }]
        }

        result = self.client.publish(topic, json.dumps(payload), qos=1)
        if result.rc == 0:
            print(f"📤 告警已上报: {alarm_type}, 置信度: {confidence:.2f}, 证据: {evidence_url}")
        else:
            print(f"❌ 上报失败: {result.rc}")

    def disconnect(self):
        if self.client:
            self.client.loop_stop()
            self.client.disconnect()
