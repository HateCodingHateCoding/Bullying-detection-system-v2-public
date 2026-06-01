import promptAction from '@ohos.promptAction';
import PreferencesUtils from '../utils/PreferencesUtils';
import { router } from '@kit.ArkUI';
import http from '@ohos.net.http';

interface DeviceStatus {
  name: string;
  location: string;
  online: boolean;
}

interface StatsData {
  devices: DeviceStatus[];
  todayCount: number;
}

@Entry
@Component
export struct PageThree {
  @State appNotify: boolean = true;
  @State smsNotify: boolean = false;
  @State currentRole: number = 0;
  @State deviceOnline: boolean = false;
  @State deviceName: string = '校园监控终端-01';
  @State deviceLocation: string = '--';
  @State totalAlerts: number = 0;

  async aboutToAppear() {
    const role = await PreferencesUtils.getPreferenceValue('MyPreferences', 'currentRole', 0);
    this.currentRole = Number(role) || 0;
    this.fetchDeviceStatus();
  }

  private fetchDeviceStatus() {
    let httpRequest = http.createHttp();
    httpRequest.request('http://115.120.205.25:8080/stats', {
      method: http.RequestMethod.GET,
      connectTimeout: 8000,
      readTimeout: 8000
    }, (err, data) => {
      if (!err && data.responseCode === 200) {
        try {
          const stats: StatsData = JSON.parse(data.result as string) as StatsData;
          const devices = stats.devices;
          if (devices && devices.length > 0) {
            this.deviceOnline = devices[0].online;
            this.deviceName = devices[0].name;
            this.deviceLocation = devices[0].location || '--';
          }
          this.totalAlerts = stats.todayCount || 0;
        } catch (e) { /* ignore */ }
      }
      httpRequest.destroy();
    });
  }

  // === PLACEHOLDER_BUILD ===

  build() {
    Column() {
      // Header
      Row() {
        Text('系统设置')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827')
          .layoutWeight(1)
      }
      .padding({ left: 20, right: 20, top: 14, bottom: 10 })

      Scroll() {
        Column() {
          // === 设备状态卡片 ===
          Column() {
            Row() {
              Column() {
                Text(this.deviceName)
                  .fontSize(17)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111827')
                Text(this.deviceLocation)
                  .fontSize(13)
                  .fontColor('#6B7280')
                  .margin({ top: 4 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)

              Row() {
                Circle({ width: 8, height: 8 })
                  .fill(this.deviceOnline ? '#10B981' : '#EF4444')
                  .margin({ right: 6 })
                Text(this.deviceOnline ? '运行中' : '离线')
                  .fontSize(13)
                  .fontColor(this.deviceOnline ? '#10B981' : '#EF4444')
                  .fontWeight(FontWeight.Medium)
              }
              .padding({ left: 12, right: 12, top: 6, bottom: 6 })
              .backgroundColor(this.deviceOnline ? '#ECFDF5' : '#FEF2F2')
              .borderRadius(12)
            }
            .width('100%')
            .margin({ bottom: 16 })

            // 设备信息网格
            Row() {
              Column() {
                Text('推理芯片').fontSize(11).fontColor('#9CA3AF')
                Text('Ascend 310B').fontSize(13).fontWeight(FontWeight.Medium).fontColor('#111827').margin({ top: 2 })
              }
              .layoutWeight(1)
              Column() {
                Text('传感器').fontSize(11).fontColor('#9CA3AF')
                Text('雷达+音频').fontSize(13).fontWeight(FontWeight.Medium).fontColor('#111827').margin({ top: 2 })
              }
              .layoutWeight(1)
              Column() {
                Text('今日告警').fontSize(11).fontColor('#9CA3AF')
                Text(this.totalAlerts.toString() + ' 条').fontSize(13).fontWeight(FontWeight.Medium).fontColor('#4F46E5').margin({ top: 2 })
              }
              .layoutWeight(1)
            }
            .width('100%')
          }
          .padding(18)
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ left: 20, right: 20, top: 8 })
          .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })

          // === 通知设置 ===
          Column() {
            Text('通知设置')
              .fontSize(17)
              .fontWeight(FontWeight.Bold)
              .fontColor('#111827')
              .margin({ bottom: 16 })

            Row() {
              Column() {
                Text('APP推送通知').fontSize(15).fontColor('#111827')
                Text('收到告警时弹窗提醒').fontSize(12).fontColor('#9CA3AF').margin({ top: 2 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)
              Toggle({ type: ToggleType.Switch, isOn: this.appNotify })
                .onChange((isOn: boolean) => { this.appNotify = isOn; })
            }
            .width('100%')
            .padding({ top: 10, bottom: 10 })

            Divider().color('#F3F4F6').margin({ top: 4, bottom: 4 })

            Row() {
              Column() {
                Text('短信提醒').fontSize(15).fontColor('#111827')
                Text('紧急告警同步发送短信').fontSize(12).fontColor('#9CA3AF').margin({ top: 2 })
              }
              .alignItems(HorizontalAlign.Start)
              .layoutWeight(1)
              Toggle({ type: ToggleType.Switch, isOn: this.smsNotify })
                .onChange((isOn: boolean) => { this.smsNotify = isOn; })
            }
            .width('100%')
            .padding({ top: 10, bottom: 10 })
          }
          .padding(18)
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ left: 20, right: 20, top: 12 })
          .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })

          // === 系统信息 ===
          Column() {
            Row() {
              Text('系统信息')
                .fontSize(17)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111827')
                .layoutWeight(1)
            }
            .width('100%')
            .margin({ bottom: 14 })

            Row() {
              Text('当前版本').fontSize(14).fontColor('#6B7280').layoutWeight(1)
              Text('v2.4.1 (build 2026.0531)').fontSize(14).fontColor('#111827')
            }
            .width('100%')
            .padding({ top: 10, bottom: 10 })

            Divider().color('#F3F4F6')

            Row() {
              Text('服务状态').fontSize(14).fontColor('#6B7280').layoutWeight(1)
              Row() {
                Circle({ width: 6, height: 6 }).fill('#10B981').margin({ right: 4 })
                Text('正常').fontSize(14).fontColor('#10B981')
              }
            }
            .width('100%')
            .padding({ top: 10, bottom: 10 })

            Divider().color('#F3F4F6')

            Row() {
              Text('模型版本').fontSize(14).fontColor('#6B7280').layoutWeight(1)
              Text('Sentinel v4.2').fontSize(14).fontColor('#111827')
            }
            .width('100%')
            .padding({ top: 10, bottom: 10 })

            Divider().color('#F3F4F6')

            Row() {
              Text('检查更新').fontSize(14).fontColor('#6B7280').layoutWeight(1)
              Text('已是最新').fontSize(14).fontColor('#9CA3AF')
            }
            .width('100%')
            .padding({ top: 10, bottom: 10 })

            Divider().color('#F3F4F6')

            Row() {
              Text('用户协议').fontSize(14).fontColor('#6B7280').layoutWeight(1)
              Text('>').fontSize(14).fontColor('#D1D5DB')
            }
            .width('100%')
            .padding({ top: 10, bottom: 10 })

            Divider().color('#F3F4F6')

            Row() {
              Text('隐私政策').fontSize(14).fontColor('#6B7280').layoutWeight(1)
              Text('>').fontSize(14).fontColor('#D1D5DB')
            }
            .width('100%')
            .padding({ top: 10, bottom: 10 })
          }
          .padding(18)
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ left: 20, right: 20, top: 12 })
          .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })

          // 退出登录
          Text('退出登录')
            .fontSize(16)
            .fontColor('#EF4444')
            .fontWeight(FontWeight.Medium)
            .textAlign(TextAlign.Center)
            .width('100%')
            .padding({ top: 14, bottom: 14 })
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .margin({ left: 20, right: 20, top: 20, bottom: 30 })
            .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
            .onClick(() => {
              router.replaceUrl({ url: 'pages/Login' });
            })
        }
        .width('100%')
      }
      .layoutWeight(1)
      .width('100%')
    }
    .height('100%')
    .width('100%')
    .backgroundColor('#F8FAFC')
  }

}