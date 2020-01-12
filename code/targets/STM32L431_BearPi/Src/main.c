/*----------------------------------------------------------------------------
 * Copyright (c) <2016-2018>, <Huawei Technologies Co., Ltd>
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 * conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list
 * of conditions and the following disclaimer in the documentation and/or other materials
 * provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used
 * to endorse or promote products derived from this software without specific prior written
 * permission.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *---------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------
 * Notice of Export Control Law
 * ===============================================
 * Huawei LiteOS may be subject to applicable export control laws and regulations, which might
 * include those applicable to Huawei LiteOS of U.S. and the country in which you are located.
 * Import, export and usage of Huawei LiteOS in any manner by you shall be in compliance with such
 * applicable export control laws and regulations.
 *---------------------------------------------------------------------------*/
#include "main.h"
#include "sys_init.h"


#include "los_base.h"
#include "los_task.ph"
#include "los_typedef.h"
#include "los_sys.h"

char s_resp_buf[14] = {0};
uint32_t reply_sem;
msg_for_BH1750 BH1750_send;

#define BUFFER_SIZE  480 
unsigned char flag=0;  
unsigned char xtemp[BUFFER_SIZE]; 
unsigned char ytemp[BUFFER_SIZE]; 

unsigned char hlw8032_raw[48]; 


VOID HardWare_Init(VOID)
{
	HAL_Init();
	/* Configure the system clock */
	SystemClock_Config();
	MX_GPIO_Init();
    MX_USART3_UART_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	MX_SPI2_Init();
	dwt_delay_init(SystemCoreClock);
	LCD_Init();					
	LCD_Clear(WHITE);		   	
	POINT_COLOR = RED;			
	LCD_ShowString(10, 50, 240, 24, 24, "Welcome to IoTCluB!");
	LCD_ShowString(20, 90, 240, 16, 16, "BearPi IoT Develop Board");
	LCD_ShowString(20, 130, 240, 16, 16, "Powerd by Huawei LiteOS!");
	LCD_ShowString(10, 170, 240, 16, 16, "Please wait for system init");
}
/* mux handle id */

static UINT32 g_Testmux01_hlw8032;


void process_hlw8032_data(void)
{
	UINT32 uwIntSave;
	printf("enter %s \r\n",__FUNCTION__);
	memset(xtemp,0,24);
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_PE);
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_FE);
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_NE);
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_ORE);
	
	//uwIntSave = LOS_IntLock();
	//HAL_UART_Receive(&huart3,xtemp,24,5);
	//LOS_IntRestore(uwIntSave);

	/* get mux */
    uwIntSave = LOS_MuxPend(g_Testmux01_hlw8032, LOS_WAIT_FOREVER);
	
	HAL_UART_Receive(&huart3,xtemp,48,1000);
	memcpy(hlw8032_raw,xtemp,48);
	 /* release mux */
	LOS_MuxPost(g_Testmux01_hlw8032);
	
	printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",xtemp[0],xtemp[1],xtemp[2],xtemp[3],xtemp[4],xtemp[5],xtemp[6],xtemp[7],xtemp[8],xtemp[9],xtemp[10],xtemp[11]);
	printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",xtemp[12],xtemp[13],xtemp[14],xtemp[15],xtemp[16],xtemp[17],xtemp[18],xtemp[19],xtemp[20],xtemp[21],xtemp[22],xtemp[23]);
	printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",xtemp[24],xtemp[25],xtemp[26],xtemp[27],xtemp[28],xtemp[29],xtemp[30],xtemp[31],xtemp[32],xtemp[33],xtemp[34],xtemp[35]);
	printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\r\n",xtemp[36],xtemp[37],xtemp[38],xtemp[39],xtemp[40],xtemp[41],xtemp[42],xtemp[43],xtemp[44],xtemp[45],xtemp[46],xtemp[47]);

}
int main(void)
{
    UINT32 uwRet = LOS_OK;
    HardWare_Init();
    uwRet = LOS_KernelInit();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }

    extern UINT32 create_work_tasks(VOID);
    uwRet = create_work_tasks();
    if (uwRet != LOS_OK)
    {
        return LOS_NOK;
    }


    (void)LOS_Start();
    return 0;
}
