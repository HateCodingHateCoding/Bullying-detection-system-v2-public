import promptAction from '@ohos.promptAction';
import PreferencesUtils from '../utils/PreferencesUtils';
import { router } from '@kit.ArkUI';

@Entry
@Component
export struct PageThree {
  @State appNotify: boolean = true;
  @State smsNotify: boolean = false;
  @State emailNotify: boolean = false;
  @State alarmLevel: number = 50;
  @State currentRole: number = 0;
    async aboutToAppear() {
    const role = await PreferencesUtils.getPreferenceValue('MyPreferences', 'currentRole', 0);
    this.currentRole = Number(role) || 0;
  }

build() {
    Column() {
      // Top bar
      Row() {
        Text('系统设置')
          .fontSize(22)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827')
          .layoutWeight(1)
        Text('保存设置')
          .fontSize(16)
          .fontColor('#FFFFFF')
          .padding({ left: 16, right: 16, top: 8, bottom: 8 })
          .backgroundColor('#4F7AF7')
          .borderRadius(10)
      }
      .padding({ left: 16, right: 16, top: 12, bottom: 12 })

      Scroll() {
        Column() {
          // Account
          Row() {
            Text('👤')
              .fontSize(18)
              .margin({ right: 8 })
            Text('账户管理')
              .fontSize(18)
              .fontWeight(FontWeight.Bold)
              .fontColor('#111827')
          }
          .padding({ left: 16, right: 16, top: 8, bottom: 8 })

          Column() {
            Row() {
              Stack() {
                Circle({ width: 52, height: 52 }).fill('#E5EEFF')
                Text('王')
                  .fontSize(20)
                  .fontColor('#4F7AF7')
              }
              .margin({ right: 12 })

              Column() {
                Text('王保安')
                  .fontSize(18)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#111827')
                // Text('值班管理员')
                //   .fontSize(14)
                //   .fontColor('#6B7280')
                //   .margin({ top: 4 })
              }
              .layoutWeight(1)
            }
            .margin({ bottom: 16 })

            // Row() {
            //   Text('权限设置')
            //     .fontSize(16)
            //     .fontColor('#374151')
            //     .padding({ top: 10, bottom: 10 })
            //     .border({ width: 1, color: '#D1D5DB' })
            //     .borderRadius(12)
            //     .layoutWeight(1)
            //     .textAlign(TextAlign.Center)
            //   Text('登录记录')
            //     .fontSize(16)
            //     .fontColor('#374151')
            //     .padding({ top: 10, bottom: 10 })
            //     .border({ width: 1, color: '#D1D5DB' })
            //     .borderRadius(12)
            //     .layoutWeight(1)
            //     .textAlign(TextAlign.Center)
            //     .margin({ left: 12 })
            // }
          }
          .padding(16)
          .backgroundColor('#F6F7F9')
          .borderRadius(16)
          .margin({ left: 16, right: 16, bottom: 20 })

          // Notification
          Text('通知配置')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111827')
            .margin({ left: 16, right: 16, bottom: 8 })

          Column() {
            // Text('报警阈值')
            //   .fontSize(16)
            //   .fontWeight(FontWeight.Medium)
            //   .fontColor('#111827')
            // Row() {
            //   Text('低敏感度')
            //     .fontSize(14)
            //     .fontColor('#6B7280')
            //     .layoutWeight(1)
            //   Text('高敏感度')
            //     .fontSize(14)
            //     .fontColor('#6B7280')
            // }
            // .margin({ top: 6, bottom: 10 })
            //
            // Slider({
            //   value: this.alarmLevel,
            //   min: 0,
            //   max: 100,
            //   step: 1,
            //   style: SliderStyle.OutSet
            // })
            // .blockColor('#E04B3E')
            // .trackColor('#E5E7EB')
            // .selectedColor('#E04B3E')
            // .showSteps(false)
            // .width('100%')
            // .onChange((value: number) => {
            //   this.alarmLevel = value;
            // })
            // .margin({ bottom: 18 })

            Text('推送方式')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .fontColor('#111827')
              .margin({ bottom: 10 })

            Row() {
              Text('APP通知')
                .fontSize(16)
                .fontColor('#111827')
                .layoutWeight(1)
              Toggle({ type: ToggleType.Switch, isOn: this.appNotify })
                .onChange((isOn: boolean) => {
                  this.appNotify = isOn;
                  promptAction.showToast({ message: isOn ? 'APP通知已开启' : 'APP通知已关闭' })
                })
            }
            .margin({ bottom: 12 })

            Row() {
              Text('短信提醒')
                .fontSize(16)
                .fontColor('#111827')
                .layoutWeight(1)
              Toggle({ type: ToggleType.Switch, isOn: this.smsNotify })
                .onChange((isOn: boolean) => {
                  this.smsNotify = isOn;
                  promptAction.showToast({ message: isOn ? '短信提醒已开启' : '短信提醒已关闭' })
                })
            }
            .margin({ bottom: 12 })

            // Row() {
            //   Text('邮件通知')
            //     .fontSize(16)
            //     .fontColor('#111827')
            //     .layoutWeight(1)
            //   Toggle({ type: ToggleType.Switch, isOn: this.emailNotify })
            //     .onChange((isOn: boolean) => {
            //       this.emailNotify = isOn;
            //       promptAction.showToast({ message: isOn ? '邮件通知已开启' : '邮件通知已关闭' })
            //     })
            // }
          }
          .padding(16)
          .backgroundColor('#F6F7F9')
          .borderRadius(16)
          .margin({ left: 16, right: 16, bottom: 20 })

          if (this.currentRole === 1) {
          // Device management
          Text('设备管理')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111827')
            .margin({ left: 16, right: 16, bottom: 8 })

          Column() {
          //   Text('星闪网络')
          //     .fontSize(16)
          //     .fontWeight(FontWeight.Medium)
          //     .fontColor('#111827')
          //
          //   Row() {
          //     Text('稳定运行')
          //       .fontSize(14)
          //       .fontColor('#16A34A')
          //       .padding({ left: 10, right: 10, top: 4, bottom: 4 })
          //       .backgroundColor('#DCFCE7')
          //       .borderRadius(12)
          //     Text('延迟 < 1ms')
          //       .fontSize(14)
          //       .fontColor('#6B7280')
          //       .margin({ left: 10, top: 4 })
          //   }
          //   .margin({ top: 8, bottom: 12 })

            Text('边缘计算节点')
              .fontSize(16)
              .fontWeight(FontWeight.Medium)
              .fontColor('#111827')
              .margin({ bottom: 6 })

            Row() {
              Circle({ width: 10, height: 10 }).fill('#22C55E')
              Text('在线节点: 3/3')
                .fontSize(14)
                .fontColor('#6B7280')
                .margin({ left: 8 })
            }
            .margin({ bottom: 16 })

            Button('终端设备配置')
              .fontSize(16)
              .fontColor('#374151')
              .padding({ top: 12, bottom: 12 })
              .border({ width: 1, color: '#D1D5DB' })
              .borderRadius(12)
          }
          .padding(16)
          .width("90%")
          .backgroundColor('#F6F7F9')
          .borderRadius(16)
          .margin({ left: 16, right: 16, bottom: 20 })

          // System maintenance
          Text('系统维护')
            .fontSize(18)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111827')
            .margin({ left: 16, right: 16, bottom: 8 })

          Column() {
            Row() {
              Column() {
                Text('数据备份')
                  .fontSize(16)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#111827')
                Text('上次备份: 2025-11-24 22:00')
                  .fontSize(14)
                  .fontColor('#6B7280')
                  .margin({ top: 4 })
              }
              .layoutWeight(1)
              Text('立即备份')
                .fontSize(14)
                .fontColor('#3B82F6')
                .padding({ left: 12, right: 12, top: 6, bottom: 6 })
                .backgroundColor('#EAF2FF')
                .borderRadius(10)
            }
            .margin({ bottom: 14 })

            Divider().color('#E5E7EB').strokeWidth(1).margin({ bottom: 12 })

            Row() {
              Column() {
                Text('系统日志')
                  .fontSize(16)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#111827')
                Text('最近7天无错误记录')
                  .fontSize(14)
                  .fontColor('#6B7280')
                  .margin({ top: 4 })
              }
              .layoutWeight(1)
              Text('查看')
                .fontSize(14)
                .fontColor('#6B7280')
                .padding({ left: 12, right: 12, top: 6, bottom: 6 })
                .backgroundColor('#F3F4F6')
                .borderRadius(10)
            }
            .margin({ bottom: 14 })

            Divider().color('#E5E7EB').strokeWidth(1).margin({ bottom: 12 })

            Column() {
              Text('明瞳安全系统 V2.3.1')
                .fontSize(14)
                .fontColor('#6B7280')
                .textAlign(TextAlign.Center)
              Text('©2025 华为全栈技术支持')
                .fontSize(12)
                .fontColor('#9CA3AF')
                .textAlign(TextAlign.Center)
                .margin({ top: 4 })
            }
            .width('100%')
          }
          .padding(16)
          .backgroundColor('#F6F7F9')
          .borderRadius(16)
          .margin({ left: 16, right: 16, bottom: 24 })
        }
          Button("退出登录").onClick(()=>{
            router.replace({url:"pages/Login"})
          })
            .width("90%")
            .backgroundColor("#ffa51e1e")
      }
      .layoutWeight(1)
      .width('100%')
    }
    .height('100%')
    .width('100%')
  }
}}