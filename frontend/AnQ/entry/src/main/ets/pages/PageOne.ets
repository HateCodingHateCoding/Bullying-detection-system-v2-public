import vibrator from '@ohos.vibrator'
import router from '@ohos.router'

interface RecordItem {
  actor: string
  time: string
  content: string
}
// 定义警情数据的类型
interface AlertData {
  id: string;
  location: string;
  time: string;
  status: string;
  eventType?: string; // 可选字段
  confidence?: number; // 可选字段
}

@Entry
@Component
export struct PageOne {
  @State accepted: boolean = false
  @State completed: boolean = false
  @State currentStep: number = 1
  @State sliderValue: number = 0

  // 从 AppStorage 读取后端推送的真实数据
  @StorageProp('hasAlert') hasAlert: boolean = false;
  @StorageProp('alertData') alertData: AlertData | null = null;
  @StorageProp('alertRecords') alertRecords: Array<RecordItem> = [];

  // 本地处理记录
  @State records: RecordItem[] = []

  // 页面显示时初始化
  aboutToAppear() {
    console.info('PageOne页面显示，当前警情状态:', this.hasAlert, this.alertData);

    if (this.hasAlert && this.alertData) {
      // 有真实警情，初始化处理状态
      this.accepted = false;
      this.completed = false;
      this.currentStep = 1;
      this.sliderValue = 0;

      // 从AppStorage加载记录
      this.records = [...this.alertRecords];
    } else {
      // 没有警情，清空状态
      this.clearState();
    }
  }

  // 清空状态
  private clearState() {
    this.accepted = false;
    this.completed = false;
    this.currentStep = 1;
    this.sliderValue = 0;
    this.records = [];
  }

  // 添加记录
  private addRecord(actor: string, content: string) {
    this.records = [
      { actor, time: this.getCurrentTime(), content },
      ...this.records
    ];
  }

  // 获取当前时间
  private getCurrentTime(): string {
    const now = new Date();
    const hours = now.getHours().toString().padStart(2, '0');
    const minutes = now.getMinutes().toString().padStart(2, '0');
    return `${hours}:${minutes}`;
  }

  // 格式化时间
  private formatTime(timeString: string): string {
    try {
      if (!timeString) return '刚刚';

      const date = new Date(timeString);
      const now = new Date();

      // 如果是5分钟内，显示"刚刚"
      if (now.getTime() - date.getTime() < 300000) {
        return '刚刚';
      }

      // 今天的时间显示"HH:mm"
      if (date.toDateString() === now.toDateString()) {
        const hours = date.getHours().toString().padStart(2, '0');
        const minutes = date.getMinutes().toString().padStart(2, '0');
        return `${hours}:${minutes}`;
      }

      // 其他时间显示"月-日 HH:mm"
      const month = (date.getMonth() + 1).toString().padStart(2, '0');
      const day = date.getDate().toString().padStart(2, '0');
      const hours = date.getHours().toString().padStart(2, '0');
      const minutes = date.getMinutes().toString().padStart(2, '0');
      return `${month}-${day} ${hours}:${minutes}`;
    } catch (e) {
      return '刚刚';
    }
  }

  // 接收警情
  private acceptCase() {
    if (this.accepted) {
      return
    }
    this.accepted = true
    this.currentStep = 2
    this.sliderValue = 0
    this.addRecord('指挥中心', '已接警情，队员紧急出发')
  }

  // 到达现场
  private arriveScene() {
    if (!this.accepted || this.completed) {
      return
    }
    this.currentStep = 3
    this.addRecord('处置人员', '已到达现场，开始处置')
  }

  // 处理完成
  // PageOne.ets 内部
  private finishCase() {
    if (!this.accepted) return;

    // 1. 清空数据
    AppStorage.setOrCreate('hasAlert', false);
    AppStorage.setOrCreate('alertData', null);

    // 2. 重置本地变量
    this.clearState();

    // 这里不需要写任何 router 代码！
    // 逻辑：hasAlert 变 false -> PageOne 的 build() 重新跑 ->
    // 命中 if(!this.hasAlert) -> 显示“🔍 暂无警情”
  }

  // 清空警情状态
  private clearAlertState() {
    // 清除AppStorage中的警情数据
    AppStorage.setOrCreate('hasAlert', false);
    AppStorage.setOrCreate('alertData', null);
    AppStorage.setOrCreate('alertRecords', []);

    // 清空本地状态
    this.clearState();
  }

  // 获取状态文本
  private getStatusText(): string {
    if (!this.hasAlert || !this.alertData) {
      return '暂无警情';
    }
    if (this.completed) {
      return '已完成';
    }
    if (this.accepted) {
      return '处理中';
    }
    return '待受理';
  }

  // 步骤激活颜色
  private stepActiveColor(index: number): string {
    return this.currentStep >= index ? '#4F7AF7' : '#D1D5DB';
  }

  // 步骤文本颜色
  private stepTextColor(index: number): string {
    return this.currentStep >= index ? '#4F7AF7' : '#9CA3AF';
  }

  // 获取警情位置
  private getAlertLocation(): string {
    if (!this.hasAlert || !this.alertData) {
      return '暂无警情';
    }
    return this.alertData.location || '未知位置';
  }

  // 获取警情时间
  private getAlertTime(): string {
    if (!this.hasAlert || !this.alertData) {
      return '--:--';
    }
    return this.formatTime(this.alertData.time) || '刚刚';
  }

  // 获取警情标题
  private getAlertTitle(): string {
    if (!this.hasAlert || !this.alertData) {
      return '等待警情推送';
    }
    const location = this.alertData.location || '未知位置';
    return `${location}异常事件`;
  }

  build() {
    Column() {
      // Top bar
      Row() {
        Text('')
          .fontSize(22)
          .fontWeight(FontWeight.Medium)
          .fontColor('#111827')
        Text('警情详情')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827')
          .layoutWeight(1)
          .textAlign(TextAlign.Center)
        Text('')
          .fontSize(18)
          .fontColor('#111827')
      }
      .padding({ left: 16, right: 16, top: 12, bottom: 12 })
      .width('100%')

      Scroll() {
        // 根据是否有警情显示不同内容
        if (!this.hasAlert || !this.alertData || this.completed) {
          // 显示"暂无警情"
          Column() {
            Text('🔍')
              .fontSize(48)
              .fontColor('#9CA3AF')
              .padding({ bottom: 16 })
            Text('暂无警情')
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .fontColor('#1F2937')
              .padding({ bottom: 8 })
            Text('有新警情时将自动推送到此页面')
              .fontSize(14)
              .fontColor('#6B7280')
          }
          .width('100%')
          .height('70%')
          .alignItems(HorizontalAlign.Center)
          .justifyContent(FlexAlign.Center)
        } else {
          // 显示警情详情
          Column() {
            // Info card - 使用真实数据
            Column() {
              Row() {
                Row() {
                  Text('高优先')
                    .fontSize(12)
                    .fontColor('#FFFFFF')
                    .padding({ left: 10, right: 10, top: 4, bottom: 4 })
                    .backgroundColor('#E24A4A')
                    .borderRadius(8)
                }
                .layoutWeight(1)
                Text(this.getStatusText())
                  .fontSize(14)
                  .fontColor('#3B82F6')
                  .padding({ left: 12, right: 12, top: 6, bottom: 6 })
                  .backgroundColor('#EAF2FF')
                  .borderRadius(10)
              }
              .alignItems(VerticalAlign.Center)
              .margin({ bottom: 8 })

              // 使用真实警情标题
              Text(this.getAlertTitle())
                .fontSize(20)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111827')
                .margin({ bottom: 6 })

              // 使用真实时间
              Row() {
                Text('🕒')
                  .fontSize(14)
                  .margin({ right: 8 })
                Text(this.getAlertTime())
                  .fontSize(15)
                  .fontColor('#4B5563')
              }
              .margin({ top: 4 })

              // 使用真实位置
              Row() {
                Text('📍')
                  .fontSize(14)
                  .margin({ right: 8 })
                Text(this.getAlertLocation())
                  .fontSize(15)
                  .fontColor('#4B5563')
              }
              .margin({ top: 8 })
            }
            .padding(16)
            .alignItems(HorizontalAlign.Start)
            .width('100%')
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .border({ width: 1, color: '#E5E7EB' })
            .margin({ left: 16, right: 16, top: 8, bottom: 12 })

            // // Map card
            // Column() {
            //   Text('位置示意图')
            //     .fontSize(16)
            //     .fontColor('#6B7280')
            // }
            // .width('95%')
            // .height(180)
            // .backgroundColor('#E5E7EB')
            // .borderRadius(16)
            // .align(Alignment.Center)
            // .margin({ left: 16, right: 16, top: 8 })

            // Map card - 替换后的代码
            Column() {
              Image($r('app.media.map_sample')) // 使用 $r 引用 media 目录下的图片名（不带后缀）
                .width('100%')
                .height('100%')
                .objectFit(ImageFit.Cover) // 图片填充模式，Cover表示裁剪填充，Contain表示完整显示
                .borderRadius(16)
            }
            .width('95%')
            .height(180)
            .backgroundColor('#E5E7EB')
            .borderRadius(16)
            .clip(true) // 确保图片超出部分被圆角裁剪
            .margin({ left: 16, right: 16, top: 8 })

            Row() {
              Circle({ width: 12, height: 12 })
                .fill('#E34D4D')
              Text('事件发生位置')
                .fontSize(16)
                .fontColor('#6B7280')
                .margin({ left: 8 })
            }
            .alignItems(VerticalAlign.Center)
            .justifyContent(FlexAlign.Center)
            .width('100%')
            .margin({ top: 12, bottom: 18 })

            // Detail section
            Text('详细描述')
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .fontColor('#111827')
              .margin({ left: 16, right: 16 })

            Text('系统检测到异常呼喊声音并伴随踢打动作片段，疑似暴力事件，置信度为85%，建议立即派员核实。')
              .fontSize(16)
              .fontColor('#374151')
              .lineHeight(24)
              .padding(16)
              .backgroundColor('#F5F6FA')
              .borderRadius(16)
              .margin({ left: 16, right: 16, top: 10, bottom: 20 })

            // Progress
            Text('进度')
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .fontColor('#111827')
              .margin({ left: 16, right: 16 })

            Divider()
              .color('#E5E7EB')
              .strokeWidth(1)
              .margin({ left: 16, right: 16, top: 12, bottom: 12 })

            Row() {
              Column() {
                Circle({ width: 18, height: 18 }).fill(this.stepActiveColor(1))
                Text('告警')
                  .fontSize(14)
                  .fontColor(this.stepTextColor(1))
                  .margin({ top: 8 })
              }
              .alignItems(HorizontalAlign.Center)
              .layoutWeight(1)

              Column() {
                Circle({ width: 18, height: 18 }).fill(this.stepActiveColor(2))
                Text('派发')
                  .fontSize(14)
                  .fontColor(this.stepTextColor(2))
                  .margin({ top: 8 })
              }
              .alignItems(HorizontalAlign.Center)
              .layoutWeight(1)

              Column() {
                Circle({ width: 18, height: 18 }).fill(this.stepActiveColor(3))
                Text('到场')
                  .fontSize(14)
                  .fontColor(this.stepTextColor(3))
                  .margin({ top: 8 })
              }
              .alignItems(HorizontalAlign.Center)
              .layoutWeight(1)

              Column() {
                Circle({ width: 18, height: 18 }).fill(this.stepActiveColor(4))
                Text('完成')
                  .fontSize(14)
                  .fontColor(this.stepTextColor(4))
                  .margin({ top: 8 })
              }
              .alignItems(HorizontalAlign.Center)
              .layoutWeight(1)
            }
            .padding({ left: 16, right: 16 })

            Text(`最新更新 ${this.getAlertTime()}`)
              .fontSize(14)
              .fontColor('#6B7280')
              .width('100%')
              .textAlign(TextAlign.Center)
              .margin({ top: 8, bottom: 24 })

            // Records
            Text('处理记录')
              .fontSize(20)
              .fontWeight(FontWeight.Bold)
              .fontColor('#111827')
              .margin({ left: 16, right: 16, bottom: 6 })

            Column() {
              ForEach(this.records, (item: RecordItem) => {
                Column() {
                  Row() {
                    Text(item.actor)
                      .fontSize(16)
                      .fontWeight(FontWeight.Medium)
                      .fontColor('#111827')
                      .layoutWeight(1)
                    Text(item.time)
                      .fontSize(14)
                      .fontColor('#6B7280')
                  }
                  Text(item.content)
                    .fontSize(16)
                    .fontColor('#4B5563')
                    .margin({ top: 6, bottom: 10 })
                }
              }, (item: RecordItem) => item.actor + item.time + item.content)
            }
            .padding({ left: 16, right: 16, top: 6, bottom: 24 })
          }
        }
      }
      .layoutWeight(1)
      .width('100%')

      // Bottom actions
      if (this.hasAlert && this.alertData && !this.accepted && !this.completed) {
        Column() {
          Stack() {
            Slider({
              value: this.sliderValue,
              min: 0,
              max: 100,
              step: 1,
              style: SliderStyle.InSet
            })
              .trackThickness(44)
              .blockColor('#E24A4A')
              .trackColor('#EEF0F4')
              .selectedColor('#FDE2E2')
              .showTips(false)
              .onChange((value: number) => {
                this.sliderValue = value
                if (value >= 100) {
                  this.acceptCase()
                }
              })

            Row() {
              Text('接受并紧急出发')
                .fontSize(16)
                .fontColor('#1F2937')
            }
            .width('100%')
            .height(44)
            .justifyContent(FlexAlign.Center)
            .alignItems(VerticalAlign.Center)
            .hitTestBehavior(HitTestMode.None)
          }
          .width('90%')

          Text('向右滑动表示接受')
            .fontSize(12)
            .fontColor('#9CA3AF')
            .margin({ top: 8, bottom: 12 })
        }
        .alignItems(HorizontalAlign.Center)
        .width('100%')
      }

      if (this.hasAlert && this.alertData && this.accepted && !this.completed) {
        Row() {
          Text('到达现场')
            .fontSize(18)
            .fontColor('#3B82F6')
            .padding({ top: 12, bottom: 12 })
            .border({ width: 1, color: '#BFDBFE' })
            .borderRadius(12)
            .layoutWeight(1)
            .textAlign(TextAlign.Center)
            .onClick(() => this.arriveScene())
          Text('处理完毕')
            .fontSize(18)
            .fontColor('#16A34A')
            .padding({ top: 12, bottom: 12 })
            .border({ width: 1, color: '#BBF7D0' })
            .borderRadius(12)
            .layoutWeight(1)
            .textAlign(TextAlign.Center)
            .margin({ left: 12 })
            .onClick(() => this.finishCase())
        }
        .padding({ left: 16, right: 16, bottom: 16 })
        .width('100%')
      }
    }
    .height('100%')
    .width('100%')
  }
}