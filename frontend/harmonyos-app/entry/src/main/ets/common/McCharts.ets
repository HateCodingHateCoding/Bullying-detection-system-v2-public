import { McLineBarChart, Options } from '@mcui/mccharts';
import { ConsumptionRecord, UserDo } from '../models/common';
import { Constants } from '../common';
import PreferencesUtils from '../utils/PreferencesUtils';
import { Service } from '../service';
import { UserService } from '../service/UserService';

@Component
export struct McCharts {
  @State userDrinkList: UserDo[] = [];
  @State bedtime: number = 22;
  @Consume dailyCaffeineLimit: number;
  @State caffeineSleepLevel: number = 150;
  @State caffeineHalfLife: number = 5;
  @State consumptionRecords: ConsumptionRecord[] = [];
  @Link refresh: boolean;
  @Consume currentCaffeine: number;
  @Consume todayTotalCaffeine: number;
  @Consume status: string;
  @Consume advice: string;
  @State hasLoadedData: boolean = false; // 新增标志位
  @State isRefreshing: boolean = false; // 控制是否正在刷新
  @Prop @Watch("onRefreshByTab") currentIndex: number;
  @State private MineIndex: number = 1;

  async aboutToAppear(): Promise<void> {
    if (!this.hasLoadedData) {
      await this.loadData();
      this.hasLoadedData = true; // 设置标志位，防止再次加载
    }
  }

  async onRefreshByTab() {
    if (this.currentIndex !== this.MineIndex) {
      return;
    }
    if (this.isRefreshing) {
      return;
    }
    this.isRefreshing = true;
    await this.loadData();
    this.isRefreshing = false;
  }

  async loadData() {
    console.log("加载数据图表");
    this.consumptionRecords = [];
    const currentUserId = await PreferencesUtils.getPreferenceValue("MyPreferences", "currentUserId", "");
    const result = await Service.getCurrentUserInfo(Number(currentUserId));

    try {
      await UserService.getUserTodayDrinkList(new Date(), Number(currentUserId)).then((resultList) => {
        this.userDrinkList = resultList;

        this.userDrinkList.forEach(drink => {
          const createTime = drink.create_time;
          const time = createTime ? createTime.getHours() : 0;
          const caffeineAmount = drink.calorie_amount;
          const caffeine = !isNaN(Number(caffeineAmount)) ? Number(caffeineAmount) : 0;

          if (!isNaN(time) && !isNaN(caffeine)) {
            this.consumptionRecords.push({ time, caffeine });
          } else {
            console.error('Invalid data:', time, caffeine);
          }
        });

        console.log(JSON.stringify(this.consumptionRecords));
      });
    } catch (e) {
      console.error("Error fetching drink list:", e);
    } finally {
      this.refresh = false; // 确保刷新状态复位
    }
    if (result && result.length) {
      this.dailyCaffeineLimit = result[0].limit_calorie || 500;
      this.caffeineSleepLevel = result[0].line_sleep || 150;
      this.caffeineHalfLife = result[0].calorie_endtime || 5;
      this.bedtime = result[0].sleep_time?.getHours() || 22;

      this.defOption = new Options({
        xAxis: {
          data: [
            '6', ' ', '8', ' ', '10', ' ', '正午', ' ', '14', ' ', '16', ' ', '18', ' ', '20', ' ', '22', ' ', '午夜',
            ' ',
            '2', ' ', '4', ' ', '6', ' ', '8', ' ', '10', ' ', '12', ' ', '14', ' ', '16', ' ', '18'
          ]
        },
        yAxis: {
          name: '卡路里 (cal)',
        },
        series: [
          {
            name: '消耗（cal）',
            data: this.calculateCaffeineLevels(),
            type: 'bar',
            barStyle: {
              width: 5,
              borderRadius: [4, 4, 4, 4]
            },
            gradient: {
              color: ['#53e075', '#7953e075']
            },
            label: {
              show: false,
            },
            color: '#ffb2d4a8'
          },
          {
            name: '睡眠线',
            data: Array(36).fill(this.caffeineSleepLevel),
            type: 'line',
            lineStyle: { color: '#ff92d75f', type: 'dashed' },
            label: {
              show: false,
              position: 'end',
              color: '#ff0000',
              fontSize: 12,
              formatter: '睡眠线'
            }
          },
          {
            name: '任务目标',
            data: Array(36).fill(this.dailyCaffeineLimit),
            type: 'line',
            lineStyle: { color: '#ff5733', type: 'solid' },
            label: {
              show: false,
              position: 'end',
              color: '#ff92d75f',
              fontSize: 12,
              formatter: '任务目标'
            }
          }
        ]
      });
    }
    this.updateStatusAndAdvice();
  }

  private static calculateCurrentCaffeine(consumptionRecords: ConsumptionRecord[], caffeineHalfLife: number): number {
    const now = new Date().getHours();
    let totalCaffeine = 0;

    consumptionRecords.forEach(record => {
      const elapsedHours = now - record.time;
      if (elapsedHours >= 0) {
        const caffeineAtNow = record.caffeine * Math.exp(-Math.log(2) / caffeineHalfLife * elapsedHours);
        totalCaffeine += caffeineAtNow;
      }
    });

    return totalCaffeine;
  }

  private static calculateTodayTotalCaffeine(consumptionRecords: ConsumptionRecord[]): number {
    const now = new Date().getHours();
    let totalCaffeine = 0;

    consumptionRecords.forEach(record => {
      if (record.time >= 6 && record.time <= now) {
        totalCaffeine += record.caffeine;
      }
    });

    return totalCaffeine;
  }

  private static getCurrentStatus(currentCaffeine: number, dailyCaffeineLimit: number): string {
    if (currentCaffeine === 0) {
      return "无消耗";
    } else if (currentCaffeine > dailyCaffeineLimit) {
      return "目标达成";
    } else {
      return "再来两组！";
    }
  }

  private static getCurrentAdvice(
    currentCaffeine: number,
    bedtime: number,
    caffeineSleepLevel: number,
    caffeineHalfLife: number
  ): string {
    const now = new Date().getHours();
    const hoursUntilBedtime = bedtime - now;

    if (hoursUntilBedtime <= 0) {
      return "已超过就寝时间，不建议再做运动。";
    }

    if (currentCaffeine >= caffeineSleepLevel) {
      return "当前卡路里消耗已超过睡眠阈值，健身也要适度哦，不建议再消耗卡路里。";
    }

    const maxAdditionalCaffeine =
      (caffeineSleepLevel - currentCaffeine) * Math.exp(Math.log(2) / caffeineHalfLife * hoursUntilBedtime);

    if (maxAdditionalCaffeine <= 0) {
      return "距离就寝时间过近，不建议再运动啦。";
    }

    if (hoursUntilBedtime >= 8) {
      return `今天时间还很充裕，可以再来两组哦。当前最多可再消耗 ${Number(maxAdditionalCaffeine.toFixed(2))}cal 卡路里。`;
    } else if (hoursUntilBedtime >= 4) {
      return `距离休息时间还有 ${Number(hoursUntilBedtime)} 小时，建议控制消耗卡路里。当前最多可再消耗 ${Number(maxAdditionalCaffeine.toFixed(2))}cal 卡路里。`;
    } else {
      return `距离休息时间不足 ${Number(hoursUntilBedtime)} 小时，建议别练啦。当前最多可再消耗 ${Number(maxAdditionalCaffeine.toFixed(2))}cal 卡路里。`;
    }
  }

  private updateStatusAndAdvice(): void {
    this.todayTotalCaffeine = 0;
    const currentCaffeine = McCharts.calculateCurrentCaffeine(this.consumptionRecords, this.caffeineHalfLife);
    const todayTotalCaffeine = McCharts.calculateTodayTotalCaffeine(this.consumptionRecords);
    const status = McCharts.getCurrentStatus(currentCaffeine, this.dailyCaffeineLimit);
    const advice =
      McCharts.getCurrentAdvice(currentCaffeine, this.bedtime, this.caffeineSleepLevel, this.caffeineHalfLife);

    this.currentCaffeine = Number(currentCaffeine.toFixed(2));
    this.todayTotalCaffeine = Number(todayTotalCaffeine.toFixed(2));
    this.status = status;
    this.advice = advice;
  }

  private calculateCaffeineLevels(): number[] {
    const hours = 36;
    const caffeineLevels: number[] = Array(hours).fill(0);

    this.consumptionRecords.forEach(record => {
      const startHour = record.time - 6;
      const initialCaffeine = record.caffeine;

      for (let i = startHour; i < hours; i++) {
        const elapsedHours = i - startHour;
        const caffeineAtHour = initialCaffeine * Math.exp(-Math.log(2) / this.caffeineHalfLife * elapsedHours);
        caffeineLevels[i] += caffeineAtHour;
      }
    });

    return caffeineLevels;
  }

  @State defOption: Options = new Options({});

  build() {
    Row() {
      Refresh({ refreshing: $$this.refresh, offset: 120, friction: 100 }) {
        McLineBarChart({
          options: this.defOption
        })
      }
      .onRefreshing(async () => {
        if (this.isRefreshing) return; // 如果已经在刷新中，直接返回
        this.isRefreshing = true; // 设置为正在刷新
        this.consumptionRecords = [];
        await this.loadData();
        this.isRefreshing = false; // 刷新结束
      })
    }
    .height('80%')
    .width("100%");
  }
}
