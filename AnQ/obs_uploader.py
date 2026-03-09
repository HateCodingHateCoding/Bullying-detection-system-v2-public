from obs import ObsClient
import time
import os

class OBSUploader:
    def __init__(self, ak, sk, endpoint, bucket_name):
        self.client = ObsClient(
            access_key_id=ak,
            secret_access_key=sk,
            server=endpoint
        )
        self.bucket = bucket_name

    def upload_audio_evidence(self, audio_data, device_id):
        """上传音频证据到OBS"""
        timestamp = int(time.time() * 1000)
        object_key = f"evidence/{device_id}/{timestamp}.wav"

        try:
            # 保存临时文件
            temp_file = f"/tmp/{timestamp}.wav"
            with open(temp_file, 'wb') as f:
                f.write(audio_data)

            # 上传到OBS
            resp = self.client.putFile(self.bucket, object_key, temp_file)

            if resp.status < 300:
                url = f"https://{self.bucket}.{self.client.server}/{object_key}"
                print(f"✅ 证据上传成功: {url}")
                os.remove(temp_file)
                return url
            else:
                print(f"❌ 上传失败: {resp.errorCode}")
                return None
        except Exception as e:
            print(f"❌ 上传异常: {e}")
            return None

    def close(self):
        self.client.close()
