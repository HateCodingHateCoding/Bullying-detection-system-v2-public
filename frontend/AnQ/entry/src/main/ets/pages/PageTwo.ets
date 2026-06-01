import { McPieChart, Options } from '@mcui/mccharts';
import http from '@ohos.net.http';

interface TimePeriod {
  name: string;
  count: number;
}

interface LocationStat {
  name: string;
  count: number;
}

interface DeviceInfo {
  name: string;
  online: boolean;
}

interface PieDataItem {
  name: string;
  value: number;
  color: string;
}

interface StatsData {
  todayCount: number;
  yesterdayCount: number;
  processingCount: number;
  timePeriods: TimePeriod[];
  locationStats: LocationStat[];
  devices: DeviceInfo[];
}

const PIE_COLORS: string[] = ['#4F46E5', '#3B82F6', '#06B6D4', '#10B981', '#F59E0B'];
const BAR_COLORS: string[] = ['#93C5FD', '#60A5FA', '#3B82F6', '#2563EB', '#1D4ED8'];
const API_BASE = 'http://115.120.205.25:8080';

@Entry
@Component
export struct PageTwo {
  @State todayCount: number = 0;
  @State yesterdayCount: number = 0;
  @State processingCount: number = 0;
  @State timePeriods: TimePeriod[] = [];
  @State locationStats: LocationStat[] = [];
  @State devices: DeviceInfo[] = [];
  @State loading: boolean = true;

  @State pieOption: Options = new Options({
    legend: { show: false },
    series: [{ radius: ['50%', '72%'], center: ['50%', '50%'], label: { show: false }, labelLine: { show: false }, data: [] }]
  });

  aboutToAppear() {
    this.fetchStats();
  }

  // === PLACEHOLDER_METHODS ===

  private fetchStats() {
    let httpRequest = http.createHttp();
    httpRequest.request(API_BASE + '/stats', {
      method: http.RequestMethod.GET,
      connectTimeout: 10000,
      readTimeout: 10000,
      header: { 'Content-Type': 'application/json' }
    }, (err, data) => {
      if (err) {
        this.loading = false;
        httpRequest.destroy();
        return;
      }
      if (data.responseCode === 200) {
        try {
          const stats: StatsData = JSON.parse(data.result as string) as StatsData;
          this.todayCount = stats.todayCount;
          this.yesterdayCount = stats.yesterdayCount;
          this.processingCount = stats.processingCount;
          this.timePeriods = stats.timePeriods || [];
          this.locationStats = stats.locationStats || [];
          this.devices = stats.devices || [];
          this.updatePieChart();
        } catch (e) { /* ignore */ }
      }
      this.loading = false;
      httpRequest.destroy();
    });
  }

  private updatePieChart() {
    const pieData: PieDataItem[] = this.locationStats.map((item: LocationStat, index: number): PieDataItem => {
      return { name: item.name, value: item.count, color: PIE_COLORS[index % PIE_COLORS.length] };
    });
    this.pieOption = new Options({
      legend: { show: false },
      series: [{
        radius: ['50%', '72%'],
        center: ['50%', '50%'],
        label: { show: false },
        labelLine: { show: false },
        data: pieData
      }]
    });
  }

  private barHeight(count: number): number {
    if (this.timePeriods.length === 0) return 0;
    const max = Math.max(...this.timePeriods.map((p: TimePeriod) => p.count));
    if (max === 0) return 0;
    return Math.round((count / max) * 130);
  }

  private getTrend(): string {
    const diff = this.todayCount - this.yesterdayCount;
    if (diff > 0) return '↑' + diff;
    if (diff < 0) return '↓' + Math.abs(diff);
    return '→ 持平';
  }

  private getTrendColor(): string {
    const diff = this.todayCount - this.yesterdayCount;
    if (diff > 0) return '#EF4444';
    if (diff < 0) return '#10B981';
    return '#6B7280';
  }

  private getOnlineCount(): number {
    return this.devices.filter((d: DeviceInfo) => d.online).length;
  }

  private getTotalAlerts(): number {
    return this.timePeriods.reduce((sum: number, p: TimePeriod) => sum + p.count, 0);
  }

  // === PLACEHOLDER_BUILD ===

  build() {
    Column() {
      // Header
      Row() {
        Column() {
          Text('数据统计')
            .fontSize(24)
            .fontWeight(FontWeight.Bold)
            .fontColor('#111827')
          Text('Ascend Sentinel 智能分析')
            .fontSize(13)
            .fontColor('#9CA3AF')
            .margin({ top: 2 })
        }
        .alignItems(HorizontalAlign.Start)
        .layoutWeight(1)

        Row() {
          Text('本月')
            .fontSize(13)
            .fontColor('#4F46E5')
            .fontWeight(FontWeight.Medium)
        }
        .padding({ left: 14, right: 14, top: 7, bottom: 7 })
        .backgroundColor('#EEF2FF')
        .borderRadius(20)
      }
      .padding({ left: 20, right: 20, top: 14, bottom: 10 })

      Scroll() {
        Column() {
          // === 顶部统计卡片 ===
          Row() {
            // 今日警情
            Column() {
              Row() {
                Text('🚨').fontSize(16)
                Text('今日警情').fontSize(13).fontColor('#6B7280').margin({ left: 4 })
              }
              Text(this.todayCount.toString())
                .fontSize(32)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111827')
                .margin({ top: 6 })
              Row() {
                Text(this.getTrend())
                  .fontSize(12)
                  .fontColor(this.getTrendColor())
                  .fontWeight(FontWeight.Medium)
                Text(' 较昨日')
                  .fontSize(12)
                  .fontColor('#9CA3AF')
              }
              .margin({ top: 4 })
            }
            .padding(14)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .layoutWeight(1)
            .alignItems(HorizontalAlign.Start)
            .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })

            // 待处理
            Column() {
              Row() {
                Text('⏳').fontSize(16)
                Text('待处理').fontSize(13).fontColor('#6B7280').margin({ left: 4 })
              }
              Text(this.processingCount.toString())
                .fontSize(32)
                .fontWeight(FontWeight.Bold)
                .fontColor('#F59E0B')
                .margin({ top: 6 })
              Row() {
                Text('需关注')
                  .fontSize(12)
                  .fontColor('#F59E0B')
                  .fontWeight(FontWeight.Medium)
              }
              .margin({ top: 4 })
            }
            .padding(14)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .layoutWeight(1)
            .alignItems(HorizontalAlign.Start)
            .margin({ left: 10 })
            .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })

            // 本月累计
            Column() {
              Row() {
                Text('📊').fontSize(16)
                Text('本月').fontSize(13).fontColor('#6B7280').margin({ left: 4 })
              }
              Text(this.getTotalAlerts().toString())
                .fontSize(32)
                .fontWeight(FontWeight.Bold)
                .fontColor('#4F46E5')
                .margin({ top: 6 })
              Row() {
                Text(this.getOnlineCount() + '/' + this.devices.length + ' 在线')
                  .fontSize(12)
                  .fontColor('#10B981')
                  .fontWeight(FontWeight.Medium)
              }
              .margin({ top: 4 })
            }
            .padding(14)
            .backgroundColor('#FFFFFF')
            .borderRadius(16)
            .layoutWeight(1)
            .alignItems(HorizontalAlign.Start)
            .margin({ left: 10 })
            .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })
          }
          .padding({ left: 20, right: 20, top: 8 })

          // === 时段分布柱状图 ===
          Column() {
            Row() {
              Text('警情时段分布')
                .fontSize(17)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111827')
                .layoutWeight(1)
              Text('本月数据')
                .fontSize(12)
                .fontColor('#9CA3AF')
            }
            .width('100%')
            .margin({ bottom: 16 })

            Row() {
              ForEach(this.timePeriods, (item: TimePeriod, index: number) => {
                Column() {
                  Text(item.count.toString())
                    .fontSize(12)
                    .fontWeight(FontWeight.Bold)
                    .fontColor('#4B5563')
                    .margin({ bottom: 6 })
                  Column()
                    .width(32)
                    .height(this.barHeight(item.count))
                    .backgroundColor(BAR_COLORS[index % BAR_COLORS.length])
                    .borderRadius({ topLeft: 6, topRight: 6, bottomLeft: 4, bottomRight: 4 })
                  Text(item.name)
                    .fontSize(12)
                    .fontColor('#6B7280')
                    .margin({ top: 8 })
                }
                .alignItems(HorizontalAlign.Center)
                .justifyContent(FlexAlign.End)
                .layoutWeight(1)
              }, (item: TimePeriod) => item.name)
            }
            .alignItems(VerticalAlign.Bottom)
            .height(180)
            .width('100%')
          }
          .padding(18)
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ left: 20, right: 20, top: 16 })
          .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })

          // === 高发区域饼图 ===
          Column() {
            Row() {
              Text('高发区域分布')
                .fontSize(17)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111827')
                .layoutWeight(1)
              Text('TOP 5')
                .fontSize(12)
                .fontColor('#4F46E5')
                .fontWeight(FontWeight.Medium)
                .padding({ left: 8, right: 8, top: 3, bottom: 3 })
                .backgroundColor('#EEF2FF')
                .borderRadius(8)
            }
            .width('100%')
            .margin({ bottom: 4 })

            McPieChart({ options: this.pieOption })
              .width('100%')
              .height(200)
              .margin({ top: 4, bottom: 8 })

            // 图例列表
            ForEach(this.locationStats, (item: LocationStat, index: number) => {
              Row() {
                Circle({ width: 10, height: 10 }).fill(PIE_COLORS[index % PIE_COLORS.length])
                Text(item.name)
                  .fontSize(14)
                  .fontColor('#374151')
                  .margin({ left: 8 })
                  .layoutWeight(1)
                Text(item.count.toString() + ' 次')
                  .fontSize(14)
                  .fontWeight(FontWeight.Medium)
                  .fontColor('#111827')
              }
              .width('100%')
              .padding({ top: 8, bottom: 8 })
            }, (item: LocationStat, index: number) => item.name)
          }
          .padding(18)
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ left: 20, right: 20, top: 12 })
          .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })

          // === 设备状态 ===
          Column() {
            Row() {
              Text('终端设备状态')
                .fontSize(17)
                .fontWeight(FontWeight.Bold)
                .fontColor('#111827')
                .layoutWeight(1)
              Text(this.getOnlineCount() + '/' + this.devices.length + ' 在线')
                .fontSize(13)
                .fontColor('#10B981')
                .fontWeight(FontWeight.Medium)
            }
            .width('100%')
            .margin({ bottom: 12 })

            ForEach(this.devices, (device: DeviceInfo) => {
              Row() {
                // 状态指示灯
                Circle({ width: 8, height: 8 })
                  .fill(device.online ? '#10B981' : '#EF4444')
                  .margin({ right: 10 })
                Text(device.name)
                  .fontSize(15)
                  .fontColor('#374151')
                  .layoutWeight(1)
                Text(device.online ? '在线' : '离线')
                  .fontSize(13)
                  .fontColor(device.online ? '#10B981' : '#EF4444')
                  .fontWeight(FontWeight.Medium)
                  .padding({ left: 10, right: 10, top: 4, bottom: 4 })
                  .backgroundColor(device.online ? '#ECFDF5' : '#FEF2F2')
                  .borderRadius(10)
              }
              .width('100%')
              .padding({ top: 10, bottom: 10 })
              .border({ width: { bottom: 1 }, color: '#F3F4F6' })
            }, (device: DeviceInfo) => device.name)
          }
          .padding(18)
          .backgroundColor('#FFFFFF')
          .borderRadius(16)
          .margin({ left: 20, right: 20, top: 12, bottom: 24 })
          .shadow({ radius: 8, color: '#0D000000', offsetX: 0, offsetY: 2 })

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
