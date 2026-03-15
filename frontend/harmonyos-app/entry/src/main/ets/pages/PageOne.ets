import { IoTService, AlarmData } from '../services/IoTService';

interface RecordItem {
  actor: string
  time: string
  content: string
}

@Entry
@Component
export struct PageOne {
  @State accepted: boolean = false
  @State completed: boolean = false
  @State currentStep: number = 1
  @State sliderValue: number = 0
  @State alarmTitle: string = '综合楼四层教室高噪音异常'
  @State alarmTime: string = '2025-12-06 15:30'
  @State alarmLocation: string = '综合教学楼4层东侧走廊'
  @State alarmDetail: string = '系统检测到 90dB 持续噪声并伴随"敲打/玻璃破碎"片段，持续 30 秒，建议立即派员核实。'
  @State alarmConfidence: number = 0
  @State evidenceUrl: string = ''
  @State records: RecordItem[] = [
    { actor: '指挥中心', time: '15:30', content: '已接警情，队员紧急出发' },
    { actor: '系统自动', time: '15:30', content: '事件确认为高优先级告警' }
  ]
  private iotService = new IoTService()

  aboutToAppear() {
    this.iotService.startPolling((alarm: AlarmData) => {
      this.handleNewAlarm(alarm);
    });
  }

  aboutToDisappear() {
    this.iotService.stopPolling();
  }

  private handleNewAlarm(alarm: AlarmData) {
    if (alarm.status === 'NEW') {
      this.accepted = false;
      this.completed = false;
      this.currentStep = 1;
      this.sliderValue = 0;

      this.alarmTime = alarm.time;
      this.alarmLocation = alarm.location;

      if (alarm.confidence) {
        this.alarmConfidence = alarm.confidence;
        this.alarmDetail = `系统检测到异常行为，AI置信度 ${(alarm.confidence*100).toFixed(1)}%，建议立即派员核实。`;
      } else {
        this.alarmDetail = `系统检测到异常行为，建议立即派员核实。`;
      }

      this.evidenceUrl = alarm.evidenceUrl || '';

      this.records = [
        { actor: '系统自动', time: '刚刚', content: `检测到异常行为${alarm.confidence ? '，置信度'+(alarm.confidence*100).toFixed(1)+'%' : ''}` }
      ];
    }
  }

  private addRecord(actor: string, content: string) {
    this.records = [
      { actor, time: '刚刚', content },
      ...this.records
    ]
  }

  private acceptCase() {
    if (this.accepted) {
      return
    }
    this.accepted = true
    this.currentStep = 2
    this.sliderValue = 0
    this.addRecord('指挥中心', '已接警情，队员紧急出发')
  }

  private arriveScene() {
    if (!this.accepted || this.completed) {
      return
    }
    this.currentStep = 3
    this.addRecord('处置人员', '已到达现场，开始处置')
  }

  private finishCase() {
    if (!this.accepted) {
      return
    }
    this.currentStep = 4
    this.completed = true
    this.addRecord('处置人员', '处置完毕，现场恢复正常')
  }

  private getStatusText(): string {
    if (this.completed) {
      return '已完成'
    }
    if (this.accepted) {
      return '处理中'
    }
    return '待受理'
  }

  private stepActiveColor(index: number): string {
    return this.currentStep >= index ? '#4F7AF7' : '#D1D5DB'
  }

  private stepTextColor(index: number): string {
    return this.currentStep >= index ? '#4F7AF7' : '#9CA3AF'
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
        if (this.completed) {
          Column() {
            Text('!')
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
          Column() {
            // Info card
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

              Text(this.alarmTitle)
                .fontSize(20)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111827')
                .margin({ bottom: 6 })

              Row() {
                Text('🕒')
                  .fontSize(14)
                  .margin({ right: 8 })
                Text(this.alarmTime)
                  .fontSize(15)
                  .fontColor('#4B5563')
              }
              .margin({ top: 4 })

              Row() {
                Text('📍')
                  .fontSize(14)
                  .margin({ right: 8 })
                Text(this.alarmLocation)
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

            // Map card
            Column() {
              Text('位置示意图')
                .fontSize(16)
                .fontColor('#6B7280')
            }
            .width('95%')
            .height(180)
            .backgroundColor('#E5E7EB')
            .borderRadius(16)
            .align(Alignment.Center)
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

            Text(this.alarmDetail)
              .fontSize(16)
              .fontColor('#374151')
              .lineHeight(24)
              .padding(16)
              .backgroundColor('#F5F6FA')
              .borderRadius(16)
              .margin({ left: 16, right: 16, top: 10, bottom: 20 })

            // AI证据链
            if (this.alarmConfidence > 0) {
              Column() {
                Row() {
                  Text('🤖 AI置信度')
                    .fontSize(16)
                    .fontWeight(FontWeight.Medium)
                    .fontColor('#111827')
                  Text(`${(this.alarmConfidence * 100).toFixed(1)}%`)
                    .fontSize(20)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#E24A4A')
                    .margin({ left: 8 })
                }
                .width('100%')
                .justifyContent(FlexAlign.SpaceBetween)
                .margin({ bottom: 12 })

                if (this.evidenceUrl) {
                  Row() {
                    Text('🎵 现场录音')
                      .fontSize(16)
                      .fontColor('#111827')
                      .layoutWeight(1)
                    Text('播放证据')
                      .fontSize(14)
                      .fontColor('#FFFFFF')
                      .padding({ left: 16, right: 16, top: 8, bottom: 8 })
                      .backgroundColor('#E24A4A')
                      .borderRadius(8)
                      .onClick(() => {
                        console.info('播放音频:', this.evidenceUrl)
                      })
                  }
                  .width('100%')
                }
              }
              .padding(16)
              .backgroundColor('#FFF5F5')
              .borderRadius(16)
              .border({ width: 2, color: '#FEE2E2' })
              .margin({ left: 16, right: 16, bottom: 20 })
            }

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

            Text('最新更新 2025-12-06 15:30')
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
      if (!this.accepted && !this.completed) {
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

      if (this.accepted && !this.completed) {
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
