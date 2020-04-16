#ifndef __LM75AD_H
#define __LM75AD_H	 
#include "gpio.h"
 
#define LM75AD_ADDR 0x90   //默认地址0x90 
 
void LM75AD_Init(void);
float LM75_Temp(void);
		 				    
#endif
