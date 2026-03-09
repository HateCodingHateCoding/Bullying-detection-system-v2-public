import http from '@ohos.net.http';

export interface AlarmData {
  alarmType: string;
  confidence: number;
  timestamp: number;
  location: string;
  audioLevel: number;
  radarData?: string;
  evidenceUrl?: string;
}

export class IoTService {
  private baseUrl: string = 'https://your-backend-api.com/api';
  private pollingInterval: number = 3000;
  private timerId: number = -1;

  setBaseUrl(url: string) {
    this.baseUrl = url;
  }

  async getLatestAlarm(): Promise<AlarmData | null> {
    try {
      const httpRequest = http.createHttp();
      const response = await httpRequest.request(
        `${this.baseUrl}/alarms/latest`,
        {
          method: http.RequestMethod.GET,
          header: { 'Content-Type': 'application/json' },
          connectTimeout: 10000,
          readTimeout: 10000
        }
      );

      if (response.responseCode === 200) {
        const data = JSON.parse(response.result as string);
        return data;
      }
      return null;
    } catch (error) {
      console.error('获取告警失败:', error);
      return null;
    }
  }

  startPolling(callback: (alarm: AlarmData) => void) {
    this.stopPolling();
    this.timerId = setInterval(async () => {
      const alarm = await this.getLatestAlarm();
      if (alarm) {
        callback(alarm);
      }
    }, this.pollingInterval);
  }

  stopPolling() {
    if (this.timerId !== -1) {
      clearInterval(this.timerId);
      this.timerId = -1;
    }
  }
}
