# 润和星闪派物联网开发套件--（SLE）WS63使用星闪功能转发串口数据

![hihope_illustration](https://gitee.com/hihopeorg/hispark-hm-pegasus/raw/master/docs/figures/hihope_illustration.png)

[润和星闪派物联网开发套件](https://item.taobao.com/item.htm?abbucket=16&id=816685710481&ns=1&priceTId=214783b117346662457694855ed644&skuId=5533042544092&spm=a21n57.sem.item.49.46a639031zWytE&utparam=%7B%22aplus_abtest%22%3A%22b28048df8f009463834be6bdac2a3713%22%7D&xxc=taobaoSearch) 基于海思WS63E解决方案的一套软硬件组合的综合性开发套件。

![wifi_iot](https://img.alicdn.com/imgextra/i4/3583112207/O1CN01SvRG981SAr7bdEg3i_!!3583112207.png)

## 一、OH接口说明

### Device Discovery OH接口

| API                                                          | 功能说明                                |
| ------------------------------------------------------------ | --------------------------------------- |
| `EnableSle`                                                  | 开启SLE |
| `SleSetLocalAddr`                                            | 设置本地设备地址 |
| `SleSetAnnounceData`                                         | 设置设备公开数据 |                                 
| `SleSetAnnounceParam`                                        | 设置设备公开参数 |
| `SleStartAnnounce`                                           | 开始广播 |
| `SleSetSeekParam`                                            | 设置扫描参数 |
| `SleStartSeek`                                               | 开始扫描 |
| `SleStopSeek`                                                | 停止扫描 |
| `SleAnnounceSeekRegisterCallbacks`                           | 注册设备公开和设备发现回调函数 |

### Connection Manager OH接口

| API                                                          | 功能说明               |
| ------------------------------------------------------------ | -----------------------|
| `SleConnectRemoteDevice`                                     | 向对端设备发起连接请求 |
| `SleConnectionRegisterCallbacks`                             | 注册连接管理回调函数 |
| `SlePairRemoteDevice`                                        | 与远端设备配对 |

### SSAP server OH接口

| API                                                          | 功能说明             |
| ------------------------------------------------------------ | -------------------- |
| `ssapsRegisterServer`                                        | 注册 SSAP server |
| `SsapsAddDescriptorSync`                                     | 添加特征描述符同步接口 |
| `SsapsAddPropertySync`                                       | 添加特征同步接口 |
| `SsapsAddServiceSync`                                        | 添加服务同步接口 |
| `SsapsStartService`                                          | 启动服务 |
| `SsapsNotifyIndicate`                                        | 给对端发送通知或指示 |
| `SsapsRegisterCallbacks`                                     | 注册服务端回调函数 |

### SSAP client OH接口

| API                                                          | 功能说明             |
| ------------------------------------------------------------ | -------------------- |
| `SsapcRegisterClient`                                        | 注册 SSAP client |
| `SsapWriteReq`                                              | 发起写请求 |


## 二、如何编译

1. 实验需要两块开发板
2. 将23_sle_uart目录复制到openharmony源码的`applications\sample\wifi-iot\app`目录下，
3. 修改openharmony源码的`applications\sample\wifi-iot\app\BUILD.gn`文件，将其中的 `features` 改为：

```
    features = [
        ...
        "23_sle_uart:sle_uart",
        ...
    ]
```
4. 在`device\soc\hisilicon\ws63v100\sdk\build\config\target_config\ws63\config.py`文件中，找到`'ws63-liteos-app'`部分，在其`'ram_component'`中，添加以下代码：
```
"sle_uart"
```

5. 在`device\soc\hisilicon\ws63v100\sdk\libs_url\ws63\cmake\ohos.cmake`文件中，找到`"ws63-liteos-app"`部分，在其`set(COMPONENT_LIST`部分，添加以下代码：
```
"sle_uart"
```

6. 修改源码中的`applications\sample\wifi-iot\app\23_sle_uart\BUILD.gn`文件，将其中的 `sources` 改为：

```
  sources = [ 
     #"sle_uart_client.c",
     "sle_uart_server_adv.c",
     "sle_uart_server.c",
  ]
```
或者
```
  sources = [ 
     "sle_uart_client.c",
     #"sle_uart_server_adv.c",
     #"sle_uart_server.c",
  ]
```

7. 在openharmony sdk根目录目录执行：`rm -rf out && hb set -p nearlink_dk_3863 && hb build -f`，执行编译两次，给两块开发板烧录不同固件


## 三、运行结果
两块ws63开发板可以通过星闪功能转发串口数据,可以通过串口工具发送数据给对方,波特率默认为115200。

### 【套件支持】

##### 1. 套件购买  https://item.taobao.com/item.htm?abbucket=16&id=816685710481&ns=1&priceTId=214783b117346662457694855ed644&skuId=5533042544092&spm=a21n57.sem.item.49.46a639031zWytE&utparam=%7B%22aplus_abtest%22%3A%22b28048df8f009463834be6bdac2a3713%22%7D&xxc=taobaoSearch

##### 2. 技术资料

- Gitee码云网站（使用说明书、规格说明书、OpenHarmony开发案例等） **https://gitee.com/hihopeorg_group/near-link**
- fbb_ws63代码仓（SDK包、技术文档下载）**https://gitee.com/HiSpark/fbb_ws63**

##### 3. 互动交流
- 海思社区星闪专区-论坛 **https://developers.hisilicon.com/forum/0133146886267870001**