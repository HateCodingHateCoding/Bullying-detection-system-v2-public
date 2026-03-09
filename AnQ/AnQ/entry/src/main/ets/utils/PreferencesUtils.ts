  import common from '@ohos.app.ability.common'
import preferences from '@ohos.data.preferences';

import hilog from '@ohos.hilog';
class PreferencesUtils{
  prefMap: Map<string,preferences.Preferences> = new Map()

  async loadPreference(context,name:string){
    try {
      let pref = await preferences.getPreferences(context, name)
      this.prefMap.set(name, pref)
      console.log('testTag', `加载Preferences[${name}]成功`)
    }catch (e){
      console.log('testTag', `加载Preferences[${name}]失败`,JSON.stringify(e))
    }
  }

  async putPreferenceValue(name:string,key:string,value:preferences.ValueType){
    if (!this.prefMap.has(name)) {
      console.log('testTag', `Preferences[${name}]尚未初始化!`)
      return
    }
    try {
      let pref = this.prefMap.get(name)
      await pref.put(key,value)
      await pref.flush()
      console.log('testTag', `保存Preferences[${name}.${key} = ${value}]成功`)
    }catch (e){
      console.log('testTag', `保存Preferences[${name}.${key} = ${value}]失败`,JSON.stringify(e))
    }
  }


  async getPreferenceValue(name:string,key:string,defaultValue:preferences.ValueType){
    if (!this.prefMap.has(name)) {
      console.log('testTag', `Preferences[${name}]尚未初始化!`)
      return
    }
    try {
      let pref = this.prefMap.get(name)
      let value = await pref.get(key,defaultValue)
       console.log('testTag', `读取Preferences[${name}.${key} = ${value}]成功`)
      return value as string
    }catch (e){
      console.log('testTag', `读取Preferences[${name}.${key}]失败`,JSON.stringify(e))
    }
  }
  splitString(input: string) {
    // 定义正则表达式模式来匹配各个部分
    const nicknamePattern = /nickname:(.*?)user:/;
    const userPattern = /user:(.*?)password:/;
    const passwordPattern = /password:(.*)/;

    // 提取 nickname 部分
    const nicknameMatch = input.match(nicknamePattern);
    const nickname = nicknameMatch ? nicknameMatch[1] : '';

    // 提取 user 部分
    const userMatch = input.match(userPattern);
    const user = userMatch ? userMatch[1] : '';

    // 提取 password 部分
    const passwordMatch = input.match(passwordPattern);
    const password = passwordMatch ? passwordMatch[1] : '';

    return {
      nickname,
      user,
      password
    };
  }
}
const preferencesUtils = new PreferencesUtils()
export default preferencesUtils as PreferencesUtils
