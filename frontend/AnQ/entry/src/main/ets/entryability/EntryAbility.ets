import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import PreferencesUtils from '../utils/PreferencesUtils';

import webSocket from '@ohos.net.webSocket';
import router from '@ohos.router';
import { AppStorageV2 } from '@kit.ArkUI';

import vibrator from '@ohos.vibrator'
import promptAction from '@ohos.promptAction'
import backgroundTaskManager from '@ohos.resourceschedule.backgroundTaskManager';
import wantAgent, { WantAgent } from '@ohos.app.ability.wantAgent';



interface AlertData {
  id: string;
  location: string;
  time: string;
  status: string;
  alarmType?: string;
  eventType?: string;
  confidence?: number;
  audioLevel?: number;
  radarData?: string;
  evidenceUrl?: string;
}

const DOMAIN = 0x0000;

export default class EntryAbility extends UIAbility {
  private ws: webSocket.WebSocket | null = null;
  private hasJumped: boolean = false;
  private reconnectTimer: number = -1;
  private readonly RECONNECT_INTERVAL = 5000;

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    try {
      this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Failed to set colorMode. Cause: %{public}s', JSON.stringify(err));
    }
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');
  }

  private showAlertDialog(alertObj: AlertData): void {
    const confidenceText = alertObj.confidence === undefined ? '' : `\n置信度：${Math.round(alertObj.confidence * 100)}%`;
    const evidenceText = alertObj.evidenceUrl ? '\n证据已上传 OBS' : '';
    promptAction.showDialog({
      title: '⚠️ 紧急警情',
      message: `位置：${alertObj.location}${confidenceText}${evidenceText}`,
      buttons: [
        {
          text: '查看',
          color: '#007DFF'
        },
        {
          text: '忽略',
          color: '#999999'
        }
      ]
    })
  }

  private async startBackgroundTask(): Promise<void> {
    const wantAgentInfo: wantAgent.WantAgentInfo = {
      wants: [
        {
          bundleName: 'com.anq.anq',
          abilityName: 'EntryAbility'
        }
      ],
      actionType: wantAgent.OperationType.START_ABILITY,
      requestCode: 0,
      actionFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG]
    };
    const agent = await wantAgent.getWantAgent(wantAgentInfo);
    await backgroundTaskManager.startBackgroundRunning(this.context, backgroundTaskManager.BackgroundMode.DATA_TRANSFER, agent);
    hilog.info(DOMAIN, 'testTag', '后台长时任务已启动');
  }

  private scheduleReconnect(): void {
    if (this.reconnectTimer !== -1) {
      return;
    }
    let timerId: number = setTimeout(() => {
      this.reconnectTimer = -1;
      hilog.info(DOMAIN, 'testTag', 'WebSocket 尝试重连...');
      this.connectWebSocket();
    }, this.RECONNECT_INTERVAL);
    this.reconnectTimer = timerId;
  }

  private connectWebSocket(): void {
    if (this.ws) {
      try { this.ws.close(); } catch (_e) {}
    }
    this.ws = webSocket.createWebSocket();
    const url = 'ws://115.120.205.25:8080/ws/alert';

    this.ws.on('open', () => {
      hilog.info(DOMAIN, 'testTag', 'WebSocket 连接成功');
    });

    this.ws.on('close', () => {
      hilog.warn(DOMAIN, 'testTag', 'WebSocket 连接关闭，准备重连');
      this.scheduleReconnect();
    });

    this.ws.on('error', (err) => {
      hilog.error(DOMAIN, 'testTag', 'WebSocket 错误 %{public}s', JSON.stringify(err));
      this.scheduleReconnect();
    });

    this.ws.on('message', (err, data) => {
      if (err) {
        hilog.error(DOMAIN, 'testTag', '接收消息失败 %{public}s', JSON.stringify(err));
        return;
      }

      hilog.info(DOMAIN, 'testTag', '收到消息 %{public}s', JSON.stringify(data));

      try {
        const alertObj: AlertData = JSON.parse(data as string) as AlertData;

        AppStorage.setOrCreate('hasAlert', true);
        AppStorage.setOrCreate('alertData', alertObj);
        AppStorage.setOrCreate('alertRecords', []);
        AppStorage.setOrCreate('mainTabIndex', 0);

        this.showAlertDialog(alertObj);
      } catch (e) {
        hilog.error(DOMAIN, 'testTag', 'JSON解析失败');
      }
    });

    this.ws.connect(url, (err) => {
      if (err) {
        hilog.error(DOMAIN, 'testTag', 'WebSocket 连接失败 %{public}s', JSON.stringify(err));
        this.scheduleReconnect();
      }
    });
  }

  private initWebSocket(): void {
    hilog.info(DOMAIN, 'testTag', '准备连接 WebSocket');
    this.startBackgroundTask();
    this.connectWebSocket();
  }

  onDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  async onWindowStageCreate(windowStage: window.WindowStage): Promise<void> {
    // Main window is created, set main page for this ability
    this.initWebSocket();

    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');
    await PreferencesUtils.loadPreference(this.context,'MyPreferences')

    windowStage.loadContent('pages/Login', (err) => {
      if (err.code) {
        hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
        return;
      }
      hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');
  }
}