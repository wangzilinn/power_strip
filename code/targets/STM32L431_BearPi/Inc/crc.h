/*
 * @Description: crc硬件初始化
 * @Author: Wang Zilin
 * @Date: 2020-04-22 14:17:29
 * @LastEditors: Wang Zilin
 * @LastEditTime: 2020-04-22 14:21:14
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __i2c_H
#define __i2c_H
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
