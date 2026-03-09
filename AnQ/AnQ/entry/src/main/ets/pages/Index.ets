import { TabBarClass } from '../models/tabbar';
import { PageOne } from './PageOne';
import { PageThree } from './PageThree';
import { PageTwo } from './PageTwo';

@Entry
@Component
struct Index {
  // Tab设置相关的变量
  @State currentIndex: number = 1;
  @State isRefreshing: boolean = true;
  @State refresh: boolean = false;
  @State tabList: TabBarClass[] = [
    {
      name: '警情',
      title: '警情',
      // icon: $r('app.media.coffeeHome'),
      // activeIcon: $r("app.media.coffeeHome")
    },
    {
      name: '统计',
      title: '统计',
      // icon: $r('app.media.coffeeTwoed'),
      // activeIcon: $r("app.media.coffeeTwoed")
    },
    {
      name: '我的',
      title: '我的',
      // icon: $r('app.media.Lib'),
      // activeIcon: $r("app.media.Libed")
    },
  ];

  @Builder
  getTabBuilder(item: TabBarClass, index: number) {
    Column() {
      // 使用圆形背景包裹图标
      Stack() {
        Image(this.currentIndex === index ? item.activeIcon : item.icon)
        .size({ width: 40, height: 40 })
        .margin({ top: 4 })
        .fillColor(this.currentIndex === index ? "#ffb80505" : "#eee")

        Text(item.title)
          .fontWeight(this.currentIndex === index ?FontWeight.Bold:null)
          .margin({ top: 4 })


      }
      .width(32)
      .height(32)
    }
    .width('100%')
    .height(58)
    .justifyContent(FlexAlign.Center)
  }

  build() {
    Column() {
      Column(){

        Tabs({ barPosition: BarPosition.End, index: $$this.currentIndex }) {
          ForEach(this.tabList, (item: TabBarClass, index: number) => {
            TabContent() {
              if (item.name === '警情') {
                PageOne()
              } else if (item.name === '统计') {
                PageTwo()
              } else if (item.name === '我的') {
                PageThree()
              }
            }.tabBar(this.getTabBuilder(item, index))
          })
        }
        .animationDuration(300)
        .padding({
          top: AppStorage.get("topHeight") as number,
          bottom: AppStorage.get("bottomHeight") as number
        })

      }

    }

    .linearGradient({
      colors: [
        [$r("app.color.IndexOne"), 0],
        [$r("app.color.IndexTwo"), 1]
      ]
    })
  }
}
