#include "AD5602.h"
#include "systick.h"
#include <stdio.h>

#define AD_DA_REF 3000
uint8_t AD5602_Add = 0x1E;

uint16_t PA_POWER = 3000;

void RFPowerON(uint16_t x)
{
	printf("ad5602 rfpoweron ..\n");
    uint8_t data1,data2;
	  uint32_t temp;
	  temp=x*255/AD_DA_REF;
	  temp=(uint8_t)temp;
	  data1=0x0f&(temp>>4);
	  data2=0xf0&(temp<<4);
	  //Clr_ASK_DATA();
	  I2C_Write(I2C1, AD5602_Add, data1, &data2, 1);
    Delay_ms(1);
}

void RFPowerOFF(void)
{
    uint8_t data1,data2;
	  data1=0x00;
	  data2=0x00;
	  I2C_Write(I2C1, AD5602_Add, data1, &data2, 1);
}
