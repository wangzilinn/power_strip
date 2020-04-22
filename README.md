
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

1. LiteOS studio, 选择Import Embedded Project

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

## 为插线板提供神经网络

----

生成工程

----

1. 从CubeMX生成神经网络工程, 输出使用Makefile格式, 称[NN工程]

----

从[NN工程]迁移文件

----

2. 复制`[NN工程]\Inc`文件夹下的

   1. `app_x-cube-ai.h`, 
   2. `[network_name].h`,
   3.  `[network_name]_data.h`
   4. `constants_ai.h`

   到`\code\targets\STM32L431_BearPi\Inc`

3. 复制[NN工程]\Src

   1. `app_x-cube-ai.c`
   2. `[network_name].c`
   3. `[network_name]_data.c`

   到`\code\targets\STM32L431_BearPi\Src`

4. 复制`[NN工程]\Middlewares\ST\AI`到`\code\targets\STM32L431_BearPi\AI`

5. **剪切**`\code\targets\STM32L431_BearPi\AI\Lib\NetworkRuntime500_CM4_GCC.a`到`\code\targets\STM32L431_BearPi\Lib`文件夹内

----

修改代码:

----

6. HAL启动CRC功能:`code/targets/STM32L431_BearPi/Inc/stm32l4xx_hal_conf.h`

   ```diff
   @@ -54,7 +54,7 @@
    /*#define HAL_CRYP_MODULE_ENABLED   */
    /*#define HAL_CAN_MODULE_ENABLED   */
    /*#define HAL_COMP_MODULE_ENABLED   */
   -/*#define HAL_CRC_MODULE_ENABLED   */
   +#define HAL_CRC_MODULE_ENABLED   
    /*#define HAL_CRYP_MODULE_ENABLED   */
    /*#define HAL_DAC_MODULE_ENABLED   */
    /*#define HAL_DCMI_MODULE_ENABLED   */
   ```

7. `\code\targets\STM32L431_BearPi\Inc`路径下新建`crc.h`

   ```c
   /*
    * @Description: crc硬件初始化
    * @Author: Wang Zilin
    * @Date: 2020-04-22 14:17:29
    * @LastEditors: Wang Zilin
    * @LastEditTime: 2020-04-22 14:21:14
    */
   
   /* Define to prevent recursive inclusion -------------------------------------*/
   #ifndef __CRC_H
   #define __CRC_H
   #ifdef __cplusplus
    extern "C" {
   #endif
   
   /* Includes ------------------------------------------------------------------*/
   #include "stm32l4xx_hal.h"
   #include "main.h"
   
   // 全局CRC句柄
   extern CRC_HandleTypeDef hcrc;
   
   extern void _Error_Handler(char *, int);
   // CRC初始化, 被main()调用
   extern void MX_CRC_Init(void);
   
   #ifdef __cplusplus
   }
   #endif
   #endif
   ```

8. `\code\targets\STM32L431_BearPi\Src`路径下新建`crc.c`,实现CRC初始化

   ```c
   /*
    * @Description: NN使用的CRC的硬件初始化
    * @Author: Wang Zilin
    * @Date: 2020-04-22 14:13:58
    * @LastEditors: Wang Zilin
    * @LastEditTime: 2020-04-22 14:19:08
    */
   
   
   /* Includes ------------------------------------------------------------------*/
   #include "crc.h"
   
   CRC_HandleTypeDef hcrc;
   
   
   
   void MX_CRC_Init(void)
   {
   
     /* USER CODE BEGIN CRC_Init 0 */
   
     /* USER CODE END CRC_Init 0 */
   
     /* USER CODE BEGIN CRC_Init 1 */
   
     /* USER CODE END CRC_Init 1 */
     hcrc.Instance = CRC;
     hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
     hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
     hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
     hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
     hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
     if (HAL_CRC_Init(&hcrc) != HAL_OK)
     {
       Error_Handler();
     }
     /* USER CODE BEGIN CRC_Init 2 */
   
     /* USER CODE END CRC_Init 2 */
   
   }
   
   
   /**
   * @brief CRC MSP Initialization
   * This function configures the hardware resources used in this example
   * @param hcrc: CRC handle pointer
   * @retval None
   */
   void HAL_CRC_MspInit(CRC_HandleTypeDef* hcrc)
   {
     if(hcrc->Instance==CRC)
     {
     /* USER CODE BEGIN CRC_MspInit 0 */
   
     /* USER CODE END CRC_MspInit 0 */
       /* Peripheral clock enable */
       __HAL_RCC_CRC_CLK_ENABLE();
     /* USER CODE BEGIN CRC_MspInit 1 */
   
     /* USER CODE END CRC_MspInit 1 */
     }
   
   }
   
   /**
   * @brief CRC MSP De-Initialization
   * This function freeze the hardware resources used in this example
   * @param hcrc: CRC handle pointer
   * @retval None
   */
   void HAL_CRC_MspDeInit(CRC_HandleTypeDef* hcrc)
   {
     if(hcrc->Instance==CRC)
     {
     /* USER CODE BEGIN CRC_MspDeInit 0 */
   
     /* USER CODE END CRC_MspDeInit 0 */
       /* Peripheral clock disable */
       __HAL_RCC_CRC_CLK_DISABLE();
     /* USER CODE BEGIN CRC_MspDeInit 1 */
   
     /* USER CODE END CRC_MspDeInit 1 */
     }
   
   }
   ```

9. 使crc.h被包含:

   ​	`code/targets/STM32L431_BearPi/Inc/sys_init.h	`

   ```diff
   @@ -61,6 +61,7 @@
    #include "spi.h"
    #include "lcd.h"
    #include "BH1750.h" 
   +#include "crc.h"
    
    #ifdef WITH_DTLS
    #include "mbedtls/net.h"
   ```

10. 修改main.c:

    1. 增加头文件

    ```c
    #include "app_x-cube-ai.h" //神经网络依赖
    ```

    2. 增加硬件初始化函数:

    ```diff
    @@ -61,11 +62,13 @@ VOID HardWare_Init(VOID)
     	/* Configure the system clock */
     	SystemClock_Config();
     	MX_GPIO_Init();
    +    MX_CRC_Init();
         MX_USART3_UART_Init();
     	MX_I2C1_Init();
         MX_I2C1_Init();
     	MX_USART1_UART_Init();
     	MX_SPI2_Init();
    +    MX_X_CUBE_AI_Init();//初始化神经网络
     	dwt_delay_init(SystemCoreClock);
     	LCD_Init();					
     	LCD_Clear(WHITE);		   	
    @@ -109,13 +112,13 @@ void process_hlw8032_data(void)
     
     }
    ```

11. 创建NN任务:

    ```c
    static UINT32 g_NN_tskHandle; // 神经网络任务全局句柄
    ```

    NN任务入口:

    ```c
    void NN_task_entry(void)
    {
        while (1)
        {
            printf("running NN task....\n");
            MX_X_CUBE_AI_Process();
            printf("done\n");
            atiny_delay(4);
        }
    }
    ```

    创建NN任务函数:

    ```c
    UINT32 create_NN_task(VOID)
    {
        UINT32 uwRet = LOS_OK;
        TSK_INIT_PARAM_S task_init_param;
    
        task_init_param.usTaskPrio = 4;
        task_init_param.pcName = "NN_task";
        task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)NN_task_entry;
        task_init_param.uwStackSize = 0x1800;
    
        uwRet = LOS_TaskCreate(&g_NN_tskHandle, &task_init_param);
        if (LOS_OK != uwRet)
        {
            return uwRet;
        }
        return uwRet;
    }
    ```

    执行创建任务:

    ```c
    UINT32 create_work_tasks(VOID)
    {
        UINT32 uwRet = LOS_OK;
    
        uwRet = create_agenttiny_task();
        if (uwRet != LOS_OK)
        {
            return LOS_NOK;
        }
        uwRet = create_NN_task(); //创建NN任务
        if (uwRet != LOS_OK)
        {
            return LOS_NOK;
        }
    	...
        return uwRet;
    }
    ```

----

修改Makefile, 基本过程:

- 添加.c文件
- 添加.h文件
- 添加.a文件

----

12. 添加NN需要的新的HAL driver:

```diff
@@ -74,15 +74,13 @@ ASM_SOURCES_s =  \
 
 
 # C sources
+# 这里把所有的.c文件放到了C_SOURCES中
+# 新添加stm32l4xx_hal_crc.c和stm32l4xx_hal_crc_ex.c两个HAL文件
 HAL_DRIVER_SRC =  \
+	$(TOP_DIR)/targets/STM32L431_BearPi/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc.c \
+	$(TOP_DIR)/targets/STM32L431_BearPi/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_crc_ex.c \
         $(TOP_DIR)/targets/STM32L431_BearPi/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_flash.c \
         $(TOP_DIR)/targets/STM32L431_BearPi/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_tim_ex.c \
         $(TOP_DIR)/targets/STM32L431_BearPi/Drivers/STM32L4xx_HAL_Driver/Src/stm32l4xx_hal_rcc.c \
```

13. 添加crc初始化.c文件和神经网络用到的,c文件:

```diff
+# 倒数第二行添加NN用到的CRC外设
 USER_SRC =  \
         $(TOP_DIR)/targets/STM32L431_BearPi/Src/main.c \
         $(TOP_DIR)/targets/STM32L431_BearPi/Src/system_stm32l4xx.c \
@@ -209,9 +208,17 @@ USER_SRC =  \
         $(TOP_DIR)/targets/STM32L431_BearPi/Src/spi.c \
         $(TOP_DIR)/targets/STM32L431_BearPi/Src/gpio.c \
         $(TOP_DIR)/targets/STM32L431_BearPi/Src/adc.c \
+		$(TOP_DIR)/targets/STM32L431_BearPi/Src/crc.c \
         $(TOP_DIR)/targets/STM32L431_BearPi/Src/user_task.c
         C_SOURCES += $(USER_SRC)
 
+# 这里添加NN用到的.c文件:
+NN_SRC =  \
+		$(TOP_DIR)/targets/STM32L431_BearPi/Src/app_x-cube-ai.c \
+		$(TOP_DIR)/targets/STM32L431_BearPi/Src/network_test.c \
+		$(TOP_DIR)/targets/STM32L431_BearPi/Src/network_test_data.c
+		C_SOURCES += $(NN_SRC)
```

14. 添加NN的.h文件:

```diff
@@ -486,6 +493,12 @@ FLAG_INC = \
         -I $(TOP_DIR)/components/ota/flag_operate
         C_INCLUDES += $(FLAG_INC)
 endif
+
+# NN的include:
+NN_INC = \
+		-I $(TOP_DIR)/targets/STM32L431_BearPi/AI/Inc
+		C_INCLUDES += $(NN_INC)
+
 # C_INCLUDES包含.h文件结束
 # compile gcc flags
 ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections
```

15. 添加NN用到的.a文件:

```diff
@@ -512,7 +525,10 @@ LIBS = -lc -lm -lnosys
 ifeq ($(USE_OTA), yes)
 LIBS += -lhwpatch
 endif
-LIBDIR = -L$(PROJECTBASE)/../Lib
+# 添加NN的lib:
+LIBS += -l:NetworkRuntime500_CM4_GCC.a
+LIBDIR = -L$(PROJECTBASE)/../Lib \
```

16. 迁移完毕

## 现有功能

### 电测量

### OTA 同时解决 wifi配置和模型升级

### NN迁移到MCU

## 未来功能
### 增加暂态特性数据
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
特别说明一下，0x08010000是app的起始地址，0x08000000是bootloader的起始地址

###  Makefile语法没错, 编译失败

每行的`\`后面不能有空格

