import { McLineChart, McPieChart, Options } from '@mcui/mccharts';

@Entry
@Component
export struct PageTwo {
  @State pieOption: Options = new Options({
    legend: { show: false },
    series: [
      {
        radius: ['45%', '70%'],
        center: ['50%', '50%'],
        label: { show: false },
        labelLine: { show: false },
        data: [
          { name: '图书馆', value: 32, color: '#1D4ED8' },
          { name: '食堂后院', value: 22, color: '#3B82F6' },
          { name: '篮球场', value: 18, color: '#93C5FD' },
          { name: '教学楼走廊', value: 12, color: '#BFDBFE' },
          { name: '其他', value: 10, color: '#A3A3A3' }
        ]
      }
    ]
  });

  @State lineOption: Options = new Options({
    grid: { top: 12, left: 24, right: 12, bottom: 26 },
    legend: { show: false },
    xAxis: {
      data: ['本周', '上周', '2周前'],
      axisLine: { show: false },
      axisTick: { show: false },
      axisLabel: { color: '#6B7280', fontSize: 12 }
    },
    yAxis: {
      show: true,
      min: 0,
      max: 100,
      axisLine: { show: false },
      axisTick: { show: false },
      axisLabel: { color: '#9CA3AF', fontSize: 12 },
      splitLine: { show: false }
    },
    series: [
      {
        name: '效率',
        data: [78, 85, 92],
        smooth: true,
        lineStyle: { color: '#84CC16', width: 3 },
        itemStyle: { color: '#84CC16' },
        symbolSize: 6,
        label: { show: false }
      }
    ]
  });

  build() {
    Column() {
      // Top bar
      Row() {
        Text('数据统计')
          .fontSize(22)
          .fontWeight(FontWeight.Bold)
          .fontColor('#111827')
          .layoutWeight(1)
        Row() {
          Text('本月')
            .fontSize(14)
            .fontColor('#3B82F6')
            .margin({ left: 6 })
        }
        .padding({ left: 14, right: 14, top: 8, bottom: 8 })
        .backgroundColor('#EAF2FF')
        .borderRadius(20)

        Text('⇩')
          .fontSize(16)
          .fontColor('#3B82F6')
          .margin({ left: 10 })
          .padding(10)
          .backgroundColor('#F3F4F6')
          .borderRadius(20)
      }
      .padding({ left: 16, right: 16, top: 12, bottom: 12 })

      Scroll() {
        Column() {
          // Stat cards 2x2
          Row() {
            Column() {
              Text('今日警情')
                .fontSize(16)
                .fontColor('#3B82F6')
              Row() {
                Text('8')
                  .fontSize(28)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111827')
                Text('-2 较昨日')
                  .fontSize(14)
                  .fontColor('#6B7280')
                  .margin({ left: 10, top: 8 })
              }
              .margin({ top: 8 })
            }
            .padding(16)
            .backgroundColor('#EFF6FF')
            .borderRadius(16)
            .layoutWeight(1)

            Column() {
              Text('处理中')
                .fontSize(16)
                .fontColor('#C2410C')
              Row() {
                Text('3')
                  .fontSize(28)
                  .fontWeight(FontWeight.Bold)
                  .fontColor('#111827')
              }
              .margin({ top: 8 })
            }
            .padding(16)
            .backgroundColor('#FFF7ED')
            .borderRadius(16)
            .layoutWeight(1)
            .margin({ left: 12 })
          }
          .padding({ left: 16, right: 16, top: 6 })

          // Bar chart section
          Text('警情时段分布')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111827')
            .margin({ left: 16, right: 16, top: 12, bottom: 8 })

          Column() {
            Row() {
              Column() {
                Text('0').fontSize(12).fontColor('#9CA3AF')
                Text('0').fontSize(12).fontColor('#9CA3AF').margin({ top: 18 })
                Text('0').fontSize(12).fontColor('#9CA3AF').margin({ top: 18 })
                Text('0').fontSize(12).fontColor('#9CA3AF').margin({ top: 18 })
              }
              .margin({ right: 10 })

              Row() {
                Column() {
                  Column()
                    .width(36)
                    .height(20)
                    .backgroundColor('#3B82F6')
                    .borderRadius(8)
                    .margin({ top: 140 })
                  Text('晨间').fontSize(12).fontColor('#D1D5DB').margin({ top: 8 })
                }
                .alignItems(HorizontalAlign.Center)
                .layoutWeight(1)

                Column() {
                  Column()
                    .width(36)
                    .height(60)
                    .backgroundColor('#3B82F6')
                    .borderRadius(8)
                    .margin({ top: 100 })
                  Text('午休').fontSize(12).fontColor('#D1D5DB').margin({ top: 8 })
                }
                .alignItems(HorizontalAlign.Center)
                .layoutWeight(1)

                Column() {
                  Column()
                    .width(36)
                    .height(110)
                    .backgroundColor('#3B82F6')
                    .borderRadius(8)
                    .margin({ top: 50 })
                  Text('课间').fontSize(12).fontColor('#D1D5DB').margin({ top: 8 })
                }
                .alignItems(HorizontalAlign.Center)
                .layoutWeight(1)

                Column() {
                  Column()
                    .width(36)
                    .height(80)
                    .backgroundColor('#3B82F6')
                    .borderRadius(8)
                    .margin({ top: 80 })
                  Text('课后').fontSize(12).fontColor('#D1D5DB').margin({ top: 8 })
                }
                .alignItems(HorizontalAlign.Center)
                .layoutWeight(1)

                Column() {
                  Column()
                    .width(36)
                    .height(40)
                    .backgroundColor('#3B82F6')
                    .borderRadius(8)
                    .margin({ top: 120 })
                  Text('晚间').fontSize(12).fontColor('#D1D5DB').margin({ top: 8 })
                }
                .alignItems(HorizontalAlign.Center)
                .layoutWeight(1)
              }
              .alignItems(VerticalAlign.Bottom)
              .layoutWeight(1)
              .height(180)
            }
            .padding(16)
          }
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ left: 16, right: 16, bottom: 16 })

          // Donut chart section
          Text('高发区域分布')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111827')
            .margin({ left: 16, right: 16, bottom: 8 })

          Column() {
            McPieChart({ options: this.pieOption })
              .width('100%')
              .height(220)
              .margin({ top: 6, bottom: 10 })

            Row() {
              Text('低发区域')
                .fontSize(14)
                .fontColor('#6B7280')
                .layoutWeight(1)
              Text('高发区域')
                .fontSize(14)
                .fontColor('#6B7280')
            }
            .margin({ bottom: 8 })

            Row() {
              Row() {
                Circle({ width: 8, height: 8 }).fill('#1D4ED8')
                Text('图书馆').fontSize(14).fontColor('#374151').margin({ left: 6 })
              }
              .layoutWeight(1)
              Row() {
                Circle({ width: 8, height: 8 }).fill('#3B82F6')
                Text('食堂后院').fontSize(14).fontColor('#374151').margin({ left: 6 })
              }
              .layoutWeight(1)
            }
            .margin({ bottom: 8 })

            Row() {
              Row() {
                Circle({ width: 8, height: 8 }).fill('#93C5FD')
                Text('篮球场').fontSize(14).fontColor('#374151').margin({ left: 6 })
              }
              .layoutWeight(1)
              Row() {
                Circle({ width: 8, height: 8 }).fill('#BFDBFE')
                Text('教学楼走廊').fontSize(14).fontColor('#374151').margin({ left: 6 })
              }
              .layoutWeight(1)
            }
          }
          .padding(16)
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ left: 16, right: 16, bottom: 16 })

          // // Efficiency section
          // Text('处理效率分析')
          //   .fontSize(20)
          //   .fontWeight(FontWeight.Bold)
          //   .fontColor('#111827')
          //   .margin({ left: 16, right: 16, bottom: 8 })

          // Column() {
          //   Column() {
          //     McLineChart({ options: this.lineOption })
          //       .width('100%')
          //       .height('100%')
          //   }
          //   .width('100%')
          //   .height(180)
          //
          //   Row() {
          //     Row() {
          //       Text('预防成功率')
          //         .fontSize(14)
          //         .fontColor('#6B7280')
          //       Text('93%')
          //         .fontSize(14)
          //         .fontColor('#16A34A')
          //         .padding({ left: 10, right: 10, top: 6, bottom: 6 })
          //         .backgroundColor('#DCFCE7')
          //         .borderRadius(12)
          //         .margin({ left: 10 })
          //     }
          //     .padding(12)
          //     .backgroundColor('#FFFFFF')
          //     .borderRadius(12)
          //     .layoutWeight(1)
          //
          //     Row() {
          //       Text('误报率')
          //         .fontSize(14)
          //         .fontColor('#6B7280')
          //       Text('5%')
          //         .fontSize(14)
          //         .fontColor('#DC2626')
          //         .padding({ left: 10, right: 10, top: 6, bottom: 6 })
          //         .backgroundColor('#FEE2E2')
          //         .borderRadius(12)
          //         .margin({ left: 10 })
          //     }
          //     .padding(12)
          //     .backgroundColor('#FFFFFF')
          //     .borderRadius(12)
          //     .layoutWeight(1)
          //     .margin({ left: 12 })
          //   }
          //   .margin({ top: 8 })
          // }
          // .padding(16)
          // .backgroundColor('#FFFFFF')
          // .borderRadius(16)
          // .margin({ left: 16, right: 16, bottom: 16 })

          // Device status
          Text('终端设备状态')
            .fontSize(20)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111827')
            .margin({ left: 16, right: 16, bottom: 8 })

          Column() {

            Row(){
              Text('图书馆-1F-卫生间')
                .fontSize(16)
                .fontWeight(FontWeight.Medium)
                .fontColor('#111827')
                .layoutWeight(1)
              Text('在线')
                .fontSize(14)
                .fontColor('#16A34A')
                .padding({ left: 10, right: 10, top: 6, bottom: 6 })
                .backgroundColor('#DCFCE7')
                .borderRadius(12)
          }            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .margin({ bottom: 12 })
            Row(){
              Text('南操场入口')
                .fontSize(16)
                .fontWeight(FontWeight.Medium)
                .fontColor('#111827')
                .layoutWeight(1)
              Text('离线')
                .fontSize(14)
                .fontColor('#ffec2c2c')
                .padding({ left: 10, right: 10, top: 6, bottom: 6 })
                .backgroundColor('#FEE2E2')
                .borderRadius(12)
            }            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .margin({ bottom: 12 })
            Row(){
              Text('食堂后院')
                .fontSize(16)
                .fontWeight(FontWeight.Medium)
                .fontColor('#111827')
                .layoutWeight(1)
              Text('在线')
                .fontSize(14)
                .fontColor('#16A34A')
                .padding({ left: 10, right: 10, top: 6, bottom: 6 })
                .backgroundColor('#DCFCE7')
                .borderRadius(12)
            }            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .margin({ bottom: 12 })
            Row(){
              Text('3号楼楼梯间')
                .fontSize(16)
                .fontWeight(FontWeight.Medium)
                .fontColor('#111827')
                .layoutWeight(1)
              Text('在线')
                .fontSize(14)
                .fontColor('#16A34A')
                .padding({ left: 10, right: 10, top: 6, bottom: 6 })
                .backgroundColor('#DCFCE7')
                .borderRadius(12)
            }            .padding(16)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .margin({ bottom: 12 })
          }
          .justifyContent(FlexAlign.Start)
          .width("100%")
          .padding({ left: 16, right: 16, bottom: 24 })
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .width("90%")
        }
        .width('100%')
      }
      .layoutWeight(1)
      .width('100%')
    }
    .height('100%')
    .width('100%')
  }
}
