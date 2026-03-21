import { router } from '@kit.ArkUI'
import PreferencesUtils from '../utils/PreferencesUtils'

@Entry
@Component
struct Login {
  @State selectedRole: number = 0
  @State rememberPassword: boolean = false

  build() {
    Column() {
      Column() {
        Stack() {
          Circle()
            .width(72)
            .height(72)
            .fill('#FFFFFF')
          Text('A')
            .fontSize(34)
            .fontWeight(FontWeight.Bold)
            .fontColor('#3A5AD9')
        }
        .margin({ top: 16, bottom: 18 })

        Text('Ascend Sentinel')
          .fontSize(28)
          .fontWeight(FontWeight.Bold)
          .fontColor('#FFFFFF')
          .margin({ bottom: 10 })

        Text('基于华为全栈技术的校园欺凌智能干预系统')
          .fontSize(14)
          .fontColor('#E6EEFF')
      }
      .width('100%')
      .height(300)
      .justifyContent(FlexAlign.Center)
      .alignItems(HorizontalAlign.Center)
      .backgroundColor(this.selectedRole === 0 ? "#2F55D9" : "#3B3F4A")

      Column() {
        Row() {
          this.RoleCard(0, '保卫处值班员', '#3D7CFF', '')
          this.RoleCard(1, '系统管理员', '#7A7A7A', '')
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)
        .margin({ bottom: 8 })

        Text('请选择您的身份登录系统')
          .fontSize(14)
          .fontColor('#A2A7B2')
          .margin({ bottom: 22 })

        Column() {
          Text('用户名 / 手机号')
            .fontSize(14)
            .fontColor('#5B616E')
            .margin({ bottom: 6 })

          TextInput({ placeholder: '请输入账号' })
            .height(52)
            .fontSize(16)
            .backgroundColor('#F5F6F8')
            .borderRadius(12)
            .padding({ left: 16, right: 16 })
        }
        .margin({ bottom: 18 })

        Column() {
          Text('密码')
            .fontSize(14)
            .fontColor('#5B616E')
            .margin({ bottom: 6 })

          TextInput({ placeholder: '请输入密码' })
            .height(52)
            .fontSize(16)
            .backgroundColor('#F5F6F8')
            .borderRadius(12)
            .padding({ left: 16, right: 16 })
        }
        .margin({ bottom: 18 })

        Row() {
          Row() {
            Checkbox({ name: 'remember', group: 'login' })
              .select(this.rememberPassword)
              .onChange((value: boolean) => {
                this.rememberPassword = value
              })
            Text('记住密码')
              .fontSize(14)
              .fontColor('#6A707C')
              .margin({ left: 6 })
          }
          Text('忘记密码?')
            .fontSize(14)
            .fontColor('#3D7CFF')
        }
        .width('100%')
        .justifyContent(FlexAlign.SpaceBetween)
        .margin({ bottom: 22 })

        Button('登录系统')
          .width('100%')
          .height(56)
          .fontSize(18)
          .fontColor('#FFFFFF')
          .borderRadius(28)
          .onClick(async () => {
            await PreferencesUtils.putPreferenceValue('MyPreferences', 'currentRole', this.selectedRole)
            router.replace({url:"pages/Index"})
          })
          .backgroundColor(this.selectedRole === 0 ? '#3D7CFF' : '#5B636E')
      }
      .width('100%')
      .padding({ left: 24, right: 24, top: 22 })
      .backgroundColor('#FFFFFF')
      .layoutWeight(1)
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }

  @Builder
  private RoleCard(index: number, title: string, accent: string, glyph: string) {
    Column() {
      Circle()
        .width(44)
        .height(44)
        .fill(this.selectedRole === index ? accent : '#E4E7EE')
        .margin({ bottom: 10 })
      Text(glyph)
        .fontSize(18)
        .fontWeight(FontWeight.Bold)
        .fontColor(this.selectedRole === index ? '#FFFFFF' : '#9AA2B1')
        .position({ x: 0, y: -38 })
      Text(title)
        .fontSize(16)
        .fontWeight(FontWeight.Medium)
        .fontColor(this.selectedRole === index ? accent : '#7A808C')
    }
    .width('48%')
    .height(110)
    .borderWidth(2)
    .borderColor(this.selectedRole === index ? accent : '#D8DCE3')
    .borderRadius(14)
    .backgroundColor('#FFFFFF')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .onClick(() => {
      this.selectedRole = index
    })
  }
}
