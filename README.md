
# power strip
中文名“排插”，插线板。
源于排插串接，排插覆盖，排插老化等现实问题的困扰，从location-aware energy dissaggregation的科学问题入手，深入研究。

## 工程结构

```bash
├─code 为包括app和bootloader的工程文件
│  ├─components
│  ├─demos
│  ├─include
│  ├─kernel
│  ├─osdepends
│  └─targets
│      ├─STM32L431_BearPi
│      │  ├─Drivers
│      │  │  ├─CMSIS
│      │  │  └─STM32L4xx_HAL_Driver
│      │  ├─GCC
│      │  │  ├─build 编译后的bin文件生成在这里
|      |  |  ├─Makefile 工程使用的Makefile在这里
│      │  ├─Hardware
│      │  ├─Inc
│      │  ├─Lib
│      │  ├─OS_CONFIG
│      │  ├─Script
│      │  └─Src
│      └─STM32L431_BearPi_bootloader_fota
└─release 为先烧后读，运行OK的代码，建议使用STM32 ST-LINK Utility
```

> 生成目录树:使用cmd.exe 的tree命令通过管道输出到文件即可

## 环境需求

LiteOS Studio的版本应该为0.3.0.0 
[https://pan.baidu.com/s/1svR2rkXmzoFKgjJL-we08g](https://pan.baidu.com/s/1svR2rkXmzoFKgjJL-we08g)

## 初次配置

1. 选择Import Embedded Project

2. 配置相关参数:

   1. Makefile:`power_strip\code\targets\STM32L431_BearPi\GCC\Makefile`
   2. Project Path:`power_strip\code`

3. 选择MCU:`STM32L431RC`

4. 选择调试器:

   1. Burner->Parameters->Burning Mode:`STLink/V2`
   2. Debugger->Parameters
      1. ->Debugger:`STLink/V2`
      2. ->Debug Mode:`Reset debug` ??和additional debug有什么不同?

5. 进入主界面进一步配置:

   Project->Configure Project

   1. 配置编译结果输出路径:(点击下载按钮时会从以下配置的目录中找.bin文件)
      1. ->Output Name:`Huawei_LiteOS`
      2. ->Output Path:`.\targets\STM32L431_BearPi\GCC\build`
   2. 配置下载地址:Burner->Address:`0x08010000`

6. 修改代码:

   1. 修改wifi用户名及密码:

      `code\components\net\at_device\wifi_esp8266\esp8266.h`

      ```c
      #define WIFI_SSID      		"wifi名字"
      #define WIFI_PASSWD    		"wifi密码"
      ```

   2. 修改设备标识码:

      (oceanConnect创建设备时指定)

      `\code\demos\agenttiny_lwm2m\agent_tiny_demo.c`

      58行:

      ```c
      char *g_endpoint_name = "设备标识码";
      ```

7. 编译工程:![img](images/编译按钮.png)或快捷键F7

8. 下载到MCU:

   1. ![img](./images/下载按钮.png)或快捷键F8

## 现有功能

### 电测量

### OTA 同时解决 wifi配置和模型升级
## 未来功能
### 增加暂态特性数据
### NN迁移到MCU
### kws-for-mcu玩法
### 针对NILM场景的功能增加


## 学术问题
### energy dissaggregation
### location
### forecasting



## Log

### 20200105
.icode为LiteOS Studio的工程配置文件，不需要了，每次按照导入工程进行配置即可。
### 20200106
MCU的32.768khz和8Mhz是必须的。

### 20200107
解决ase.d文件编译不过问题。

### 20200112
解决hlw8032传感器稳定性问题，OTA测试OK



## 问题解决

### 下载失败:Unknown memory region 

下载时报错:

```bash
[2020-04-16 13:50:51] Start burning [STLink mode]
"C:\LiteOS Studio\resources\app\bin\stlink\st-flash.exe" --serial 303637354646333533303338344535 --flash=512k write "C:\Case\case-STM32\200416_power_strip\code\targets\STM32L431_BearPi\GCC\build\Huawei_LiteOS.bin" 0x80000000
2020-04-16T13:50:52 INFO common.c: Loading device parameters.... 
2020-04-16T13:50:52 INFO common.c: Device connected is: L43x/L44x device, id 0x10016435 
2020-04-16T13:50:52 INFO common.c: SRAM size: 0xc000 bytes (48 KiB), Flash: 0x40000 bytes (256 KiB) in pages of 2048 bytes 
st-flash 1.5.0 
Forcing flash size: --flash=0x00080000 
Unknown memory region 
[2020-04-16 13:50:52] Burning failed.
```

解决:配置下载地址:Burner->Address:`0x08010000`

## 

