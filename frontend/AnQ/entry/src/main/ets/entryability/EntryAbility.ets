import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import PreferencesUtils from '../utils/PreferencesUtils';


import webSocket from '@ohos.net.webSocket';
import router from '@ohos.router';
import { AppStorageV2 } from '@kit.ArkUI';

import vibrator from '@ohos.vibrator'
import promptAction from '@ohos.promptAction'



interface AlertData {
  id: string;
  location: string;
  time: string;
  status: string;
  eventType?: string;
  confidence?: number;
}

const DOMAIN = 0x0000;

export default class EntryAbility extends UIAbility {
  private ws: webSocket.WebSocket | null = null;
  private hasJumped: boolean = false;

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    try {
      this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);
    } catch (err) {
      hilog.error(DOMAIN, 'testTag', 'Failed to set colorMode. Cause: %{public}s', JSON.stringify(err));
    }
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');
  }

  private showAlertDialog(location: string): void {
    promptAction.showDialog({
      title: '⚠️ 紧急警情',
      message: `位置：${location}`,
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

  private initWebSocket(): void {
    hilog.info(DOMAIN, 'testTag', '准备连接 WebSocket');

    this.ws = webSocket.createWebSocket();

    const url = 'ws://115.120.205.25:8080/ws/alert';

    this.ws.connect(url, (err) => {
      if (err) {
        hilog.error(DOMAIN, 'testTag', 'WebSocket 连接失败 %{public}s', JSON.stringify(err));
      } else {
        hilog.info(DOMAIN, 'testTag', 'WebSocket 连接成功');
      }
    });

    this.ws.on('message', (err, data) => {
      if (err) {
        hilog.error(DOMAIN, 'testTag',
          '接收消息失败 %{public}s',
          JSON.stringify(err)
        );
        return;
      }

      hilog.info(DOMAIN, 'testTag',
        '收到消息 %{public}s',
        JSON.stringify(data)
      );

      try {
        const alertObj: AlertData = JSON.parse(data as string) as AlertData;
        const location = alertObj.location || '未知位置';

        // ✅ 写入 PageOne 需要的三个 StorageProp
        AppStorage.setOrCreate('hasAlert', true);
        AppStorage.setOrCreate('alertData', alertObj);
        AppStorage.setOrCreate('alertRecords', []);


        // 如果你想让 App 自动切到第一个 Tab (警情页)
        // 假设 Index 里的 currentIndex 绑定了 AppStorage
        AppStorage.setOrCreate('mainTabIndex', 0);

        // 🔥🔥🔥 新增：直接触发系统反馈
        this.showAlertDialog(location);

      } catch (e) {
        hilog.error(DOMAIN, 'testTag', 'JSON解析失败');
      }
    });
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