#include "sec_3v3_en.h"

//初始化PB0为输出口.并使能这个口的时钟		    
//POWER_LED IO初始化
void SEC_3V3_EN_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(SEC_3V3_EN_CLK, ENABLE);	         

	GPIO_InitStructure.GPIO_Pin   = SEC_3V3_EN_PIN;				   
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;           
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;         //
	GPIO_Init(SEC_3V3_EN_PORT, &GPIO_InitStructure);         
	GPIO_SetBits(SEC_3V3_EN_PORT,SEC_3V3_EN_PIN);					 //输出低
}