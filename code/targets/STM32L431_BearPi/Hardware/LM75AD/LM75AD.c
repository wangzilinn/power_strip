#include "LM75AD.h"
#include "i2c.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
/*
0x00  Temperature Register      
0x01  Configuration register  器件模式 写00普通模式(100ms更新一次温度值) 写01为ShutDown模式
0x02  Hysteresis register
0x03  Over_temperature shutdown register
Temp Register 
MSByte                LSByte
7   6  5  4  3  2  1  0  7  6  5  4 3 2 1 0
D10 D9 D8 D7 D6 D5 D4 D3 D2 D1 D0 X X X X X
D10=0    ℃=+(Temp Data×0.125) 	
D10=1    ℃=-(Temp Data×0.125)
Address Table
MSB          LSB
1 0 0 1 A2 A1 A0
1 0 0 1 0  0  0 0/1       =0x90
*/
/////////////////////////////////////////////////////////////////////////////////
 unsigned char dat[]={0x01,0x00};
 
//LM75AD IO初始化
void LM75AD_Init(void)
{
  	HAL_I2C_Master_Transmit(&hi2c2,LM75AD_ADDR, dat,2,0xff);
}
 
float LM75_Temp(void)
{ unsigned char temp_buff[2];
	short temp_data;
	float temp_out;
	HAL_I2C_Master_Receive(&hi2c2,LM75AD_ADDR,temp_buff,2, 0xff);
	temp_data = (temp_buff[0] << 3) | (temp_buff[1] >> 5);
	if(temp_data & 0x400)
	{//负温度
		temp_data = ~temp_data + 1;
		temp_data &= 0x7ff;
		temp_data = -temp_data;
	}
		
	temp_out = temp_data * 0.125f;
		
	return temp_out;

}
