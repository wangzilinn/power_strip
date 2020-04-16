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
#include "internals.h"

#include "agent_tiny_demo.h"
#include "bh1750.h"
#include "lcd.h"
#include "adc.h"
#include "LM75AD.h"
#include "stm32l4xx_hal.h"


#if defined WITH_AT_FRAMEWORK
#include "at_frame/at_api.h"
#endif

//#define DEFAULT_SERVER_IP "180.101.147.115" /*dianxin*/

#define DEFAULT_SERVER_IP "49.4.85.232" /*local ipv4*/


#define LWM2M_LIFE_TIME     50000

// NB-IOT device IMEI number
//char *g_endpoint_name = "869505047090426";//char *g_endpoint_name = "20190412";//869505047090426
// wifi device mac  bc:dd:c2:2f:54:26
char *g_endpoint_name = "bc:dd:c2:2f:54:26CHANGE";

#ifdef WITH_DTLS

char *g_endpoint_name_s = "2019032219";
char *g_endpoint_name_iots = "2019032219";
char *g_endpoint_name_bs = "2019032219";
unsigned char g_psk_iot_value[] = {0x79,0x1e,0xb9,0x37,0x14,0x03,0x10,0x50,0x70,0xbb,0x8f,0xee,0xfc,0xa3,0xb1,0x97}; //0x33 -> 0x32
unsigned char g_psk_bs_value[] = {0x79,0x1e,0xb9,0x37,0x14,0x03,0x10,0x50,0x70,0xbb,0x8f,0xee,0xfc,0xa3,0xb1,0x97};
//unsigned char g_psk_value[16] = {0x58,0xea,0xfd,0xab,0x2f,0x38,0x4d,0x39,0x80,0x69,0x4d,0x1c,0xda,0x69,0xb0,0x43};


#endif

static void *g_phandle = NULL;
static atiny_device_info_t g_device_info;
static atiny_param_t g_atiny_params;

#define cn_buf_len    256   //may be bigger enough
static char t_report_buf[cn_buf_len];
static char s_report_buf[cn_buf_len];
static char r_report_buf[cn_buf_len];

static void HexStrToStr(char *source, char *dest, int sourceLen)
{
    short i;
    unsigned char highByte, lowByte;
    for (i = 0; i < sourceLen; i += 2)
    {
        highByte = toupper(source[i]);
        lowByte  = toupper(source[i + 1]);
        if (highByte > 0x39)
            highByte -= 0x37;
        else
            highByte -= 0x30;
        if (lowByte > 0x39)
            lowByte -= 0x37;
        else
            lowByte -= 0x30;
        dest[i / 2] = (highByte << 4) | lowByte;
    }
    return ;
}

void ack_callback(atiny_report_type_e type, int cookie, data_send_status_e status)
{
    ATINY_LOG(LOG_DEBUG, "type:%d cookie:%d status:%d\n", type, cookie, status);
}
extern void process_hlw8032_data(void);
extern unsigned char hlw8032_raw[48]; 

UINT8 hlw8032_real_data[24];
int vol;
float cur;
char voltage[2];
char current[2];
char power[2];
char luminance[2];
char boardtemp[2];
UINT8 pre_data;
UINT8 cur_data;
UINT8 start_index;

void hlw8032_data_handling(UINT8 *pData, UINT8 Size)
{
	unsigned int i;
	for(i=0;i<Size;i++)
	{
		pre_data=cur_data;
		cur_data=pData[i];
		if((0x55==pre_data)&&(0x5a==cur_data))
		{
			start_index=i-1;
			printf("start_index=%d\r\n",start_index);
			memcpy(hlw8032_real_data,pData+start_index,24);
			printf("hlw8032_real_data1 %2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,\r\n",hlw8032_real_data[0],hlw8032_real_data[1],hlw8032_real_data[2],hlw8032_real_data[3],hlw8032_real_data[4],hlw8032_real_data[5],hlw8032_real_data[6],hlw8032_real_data[7],hlw8032_real_data[8],hlw8032_real_data[9],hlw8032_real_data[10],hlw8032_real_data[11]);
			printf("hlw8032_real_data2 %2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,\r\n",hlw8032_real_data[12],hlw8032_real_data[13],hlw8032_real_data[14],hlw8032_real_data[15],hlw8032_real_data[16],hlw8032_real_data[17],hlw8032_real_data[18],hlw8032_real_data[19],hlw8032_real_data[20],hlw8032_real_data[21],hlw8032_real_data[22],hlw8032_real_data[23]);
			vol=((hlw8032_real_data[2]*65536+hlw8032_real_data[3]*256+hlw8032_real_data[4])*1.88/(hlw8032_real_data[5]*65536+hlw8032_real_data[6]*256+hlw8032_real_data[7]));
			printf("vol=%d\r\n",vol);

			cur=((hlw8032_real_data[8]*65536+hlw8032_real_data[9]*256+hlw8032_real_data[10])*1/(hlw8032_real_data[11]*65536+hlw8032_real_data[12]*256+hlw8032_real_data[13]));
			printf("cur=%d\r\n",cur);
			
			break;
		}
	}
}


VOID app_data_report_collection(VOID)
{
	UINT32 uwRet = LOS_OK;
	
	short int Lux; 

    uint16_t ADCvalue=0;
	float Voltage=0.0f;
	float Rest=0.0f;
    char str[10]={0};
    uint16_t intv;

    float temp;
    uint16_t  temp2; 

	MX_ADC1_Init();
	Init_BH1750();	
    LM75AD_Init();		

	while (1)
  {
		process_hlw8032_data();
		hlw8032_data_handling(hlw8032_raw,48);
		
		Lux=(int)Convert_BH1750();
        //Lux++;	
		printf("\r\n******************************BH1750 Value is  %d\r\n",Lux);
		LCD_ShowString(10, 200, 200, 16, 16, "BH1750 Value is:");
		LCD_ShowNum(140, 200, Lux, 5, 16);
		sprintf(BH1750_send.Lux,"%04X", Lux);	

       // temp=LM75_Temp();
       temp =123.45;
        temp2= temp;
        printf("LM75temp：%d.",temp2);
        temp2 = (temp -temp2)*100;
        printf("%d\r\n",temp2);

        # if 1     
        HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,50);
		if(HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1),HAL_ADC_STATE_REG_EOC))
		{
			ADCvalue =  HAL_ADC_GetValue(&hadc1);
            Voltage=ADCvalue*3.3/4096;//2^12=4096	
            printf("\r\n #########################\r\n");
             // Voltage=Voltage*1000;
             intv = Voltage*1000;
             printf("\r\n ADCValue:%d,Voltage: %d \r\n",ADCvalue,intv);//打印转换数据和换算电压
           // sprintf(str, "%f", Voltage);	
         //  printf("\r\n ADCValue:%d,Voltage: %s \r\n",ADCvalue,str);//打印转换数据和换算电压
            Rest = Voltage*10/(3.3-Voltage);
            intv = Rest*1000;
             printf("\r\n Rest: %d \r\n",intv);
           // sprintf(str, "%f", Rest);
           //printf("\r\n Rest: %sK \r\n",str);
		}


        # endif

		uwRet=LOS_TaskDelay(2000);
		if(uwRet !=LOS_OK)
		return;
	
  }
}
UINT32 creat_collection_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
	UINT32 TskHandle;
    task_init_param.usTaskPrio = 0;
    task_init_param.pcName = "app_data_report_collection";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)app_data_report_collection;
    task_init_param.uwStackSize = 0x800;

    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}

/*lint -e550*/
void app_data_report(void)
{
	data_report_t report_data;
    int ret = 0;
    int cnt = 0;
	
	UINT32 msgid;
	msgid = 0;
	sprintf(t_report_buf,"%02d", msgid);
	
    report_data.buf = (uint8_t *)s_report_buf;
    report_data.callback = ack_callback;
    report_data.cookie = 0;
	//this data len is 256
    report_data.len = sizeof(s_report_buf);
    report_data.type = APP_DATA;
    (void)ret;
    while(1)
    {
		//copy the voltage value from BH1750
		LOG_ARG("BH1750_send.Lux string=0x%x,0x%x,0x%x,0x%x,0x%x",BH1750_send.Lux[0],BH1750_send.Lux[1],BH1750_send.Lux[2],BH1750_send.Lux[3],BH1750_send.Lux[4]);
		memcpy(t_report_buf + 2, &BH1750_send, sizeof(BH1750_send));	
		

		
		report_data.cookie = cnt;
        cnt++;
		LOG_ARG("strlen(t_report_buf): %02X", strlen(t_report_buf));
		LOG_ARG("t_report_buf=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,",t_report_buf[0],t_report_buf[1],t_report_buf[2],t_report_buf[3],t_report_buf[4],t_report_buf[5]);
		HexStrToStr(t_report_buf,s_report_buf,strlen(t_report_buf));
		LOG_ARG("s_report_buf=0x%x,0x%x,0x%x,0x%x,0x%x,0x%x,",s_report_buf[0],s_report_buf[1],s_report_buf[2],s_report_buf[3],s_report_buf[4],s_report_buf[5]);
		luminance[0]=s_report_buf[1];
		luminance[1]=s_report_buf[2];

		voltage[0]=0x00;
		//voltage[1]=0xDC;
		voltage[1]=vol;
		memcpy(report_data.buf + 1, voltage, 2);	
		
		
		//copy the current value from constant
		current[0]=0x00;
		//current[1]=0x02;
		current[1]=cur;
		memcpy(report_data.buf + 3, current, 2);	
		
		power[0]=0x01;
		power[1]=0xB8;
		memcpy(report_data.buf + 5, power, 2);	

	
		//luminance[0]=0x00;
		//luminance[1]=0x01;
		
		LOG_ARG("luminace=0x%x %x", luminance[0],luminance[1]);
		memcpy(report_data.buf + 7, luminance, 2);
		LOG_ARG("report_data.buf=0x%x %x", report_data.buf[7],report_data.buf[8]);

		boardtemp[0]=0x00;
		boardtemp[1]=0x18;
		memcpy(report_data.buf + 9, boardtemp, 2);
		
        ret = atiny_data_report(g_phandle, &report_data);
        ATINY_LOG(LOG_DEBUG, "data report ret: %d\n", ret);
        ret = atiny_data_change(g_phandle, DEVICE_MEMORY_FREE);
        ATINY_LOG(LOG_DEBUG, "data change ret: %d\n", ret);
        (void)LOS_TaskDelay(500 * 8);
    }
}
/*lint +e550*/
UINT32 creat_report_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
    UINT32 TskHandle;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "app_data_report";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)app_data_report;
    task_init_param.uwStackSize = 0x800;

    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;

}

VOID reply_report_task(VOID)
{
		data_report_t reply_data;
    int ret = 0;
    int cnt = 0;
	
    reply_data.buf = (uint8_t *)r_report_buf;
    reply_data.callback = ack_callback;
    reply_data.cookie = 0;
    reply_data.len = sizeof(r_report_buf);
    reply_data.type = APP_DATA;
		while (1)
		{
				LOS_SemPend(reply_sem, LOS_WAIT_FOREVER);
				HexStrToStr(s_resp_buf,r_report_buf,strlen(s_resp_buf));
				if(atiny_data_report(g_phandle, &reply_data)>=0)		//上报数据			
				{
						printf("ocean_send_reply OK!\n");                                                  
				}
				else                                                                                  
				{
						printf("ocean_send_reply Fail!\n"); 	
				}
				memset(s_resp_buf, 0, sizeof(s_resp_buf));
		}
}

UINT32 creat_reply_report_task()
{
    UINT32 uwRet = LOS_OK;
    TSK_INIT_PARAM_S task_init_param;
	  UINT32 TskHandle;

    task_init_param.usTaskPrio = 1;
    task_init_param.pcName = "reply_report_task";
    task_init_param.pfnTaskEntry = (TSK_ENTRY_FUNC)reply_report_task;
    task_init_param.uwStackSize = 0x800;
    uwRet = LOS_TaskCreate(&TskHandle, &task_init_param);
    if(LOS_OK != uwRet)
    {
        return uwRet;
    }
    return uwRet;
}



void agent_tiny_entry(void)
{
    UINT32 uwRet = LOS_OK;
    atiny_param_t *atiny_params;
    atiny_security_param_t  *iot_security_param = NULL;
    atiny_security_param_t  *bs_security_param = NULL;

    atiny_device_info_t *device_info = &g_device_info;


#ifdef WITH_DTLS
    device_info->endpoint_name = g_endpoint_name_s;
#else
    device_info->endpoint_name = g_endpoint_name;
#endif
#ifdef CONFIG_FEATURE_FOTA
    device_info->manufacturer = "Lwm2mFota";
    device_info->dev_type = "Lwm2mFota";
#else
    device_info->manufacturer = "Agent_Tiny";
#endif
    atiny_params = &g_atiny_params;
    atiny_params->server_params.binding = "UQ";
    //atiny_params->server_params.life_time = LWM2M_LIFE_TIME;
    atiny_params->server_params.life_time = 20;
    atiny_params->server_params.storing_cnt = 3;

    atiny_params->server_params.bootstrap_mode = BOOTSTRAP_FACTORY;
    atiny_params->server_params.hold_off_time = 10;

    //pay attention: index 0 for iot server, index 1 for bootstrap server.
    iot_security_param = &(atiny_params->security_params[0]);
    bs_security_param = &(atiny_params->security_params[1]);


    iot_security_param->server_ip = DEFAULT_SERVER_IP;
    bs_security_param->server_ip  = DEFAULT_SERVER_IP;


#ifdef WITH_DTLS
    iot_security_param->server_port = "5684";
    bs_security_param->server_port = "5684";

    iot_security_param->psk_Id = g_endpoint_name_iots;
    iot_security_param->psk = (char *)g_psk_iot_value;
    iot_security_param->psk_len = sizeof(g_psk_iot_value);

    bs_security_param->psk_Id = g_endpoint_name_bs;
    bs_security_param->psk = (char *)g_psk_bs_value;
    bs_security_param->psk_len = sizeof(g_psk_bs_value);
#else
    iot_security_param->server_port = "5683";
    bs_security_param->server_port = "5683";

    iot_security_param->psk_Id = NULL;
    iot_security_param->psk = NULL;
    iot_security_param->psk_len = 0;

    bs_security_param->psk_Id = NULL;
    bs_security_param->psk = NULL;
    bs_security_param->psk_len = 0;
#endif
    if(ATINY_OK != atiny_init(atiny_params, &g_phandle))
    {
        return;
    }
	LCD_Clear(WHITE);		   
	POINT_COLOR = RED;	
    LCD_ShowString(40, 10, 200, 16, 24, "IoTCluB BearPi");
	LCD_ShowString(50, 50, 200, 16, 24, "E53_SC1_Demo");
    LCD_ShowString(10, 100, 200, 16, 16, "NCDP_IP:");
    LCD_ShowString(80, 100, 200, 16, 16, DEFAULT_SERVER_IP);
    LCD_ShowString(10, 150, 200, 16, 16, "NCDP_PORT:");
    #ifdef WITH_DTLS
    LCD_ShowString(100, 150, 200, 16, 16, "5684");
    #else
    LCD_ShowString(100, 150, 200, 16, 16, "5683");
    #endif	
    uwRet = creat_collection_task();
    if (uwRet != LOS_OK)
    {
        return ;
    }
		
    uwRet = creat_report_task();
    if(LOS_OK != uwRet)
    {
        return;
    }

		uwRet = LOS_SemCreate(0,&reply_sem);
		if (uwRet != LOS_OK)
    {
        return ;
    }
		
		uwRet = creat_reply_report_task();
    if (uwRet != LOS_OK)
    {
        return ;
    }
		

		
    (void)atiny_bind(device_info, g_phandle);
}





