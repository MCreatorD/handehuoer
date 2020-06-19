#ifndef __SEC_3V3_EN_H
#define __SEC_3V3_EN_H
#include "stm32f4xx.h"
	#ifdef __cplusplus
	 extern "C" {
	#endif
		 
	//硬件资源PA6
	#define SEC_3V3_EN_PORT 		 		 GPIOA	
	#define SEC_3V3_EN_PIN           GPIO_Pin_6 
	#define SEC_3V3_EN_CLK           RCC_AHB1Periph_GPIOA

	//初始化函数
	extern void SEC_3V3_EN_Init(void);
		 
	/* 直接操作寄存器的方法控制IO */
	#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态	 
	//方法函数
	#define SEC_3V3_EN_H             GPIO_SetBits(SEC_3V3_EN_PORT , SEC_3V3_EN_PIN)
	#define SEC_3V3_EN_L             GPIO_ResetBits(SEC_3V3_EN_PORT , SEC_3V3_EN_PIN)
//	#define POWER_LED_TOGGLE        digitalToggle(SEC_3V3_EN_PORT,SEC_3V3_EN_PIN)

///* 直接操作寄存器的方法控制IO */
//#define	digitalHi(p,i)			{p->BSRR=i;}			//设置为高电平		
//#define digitalLo(p,i)			{p->BRR=i;}				//输出低电平
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//输出反转状态


///* 定义控制IO的宏 */
//#define LED1_TOGGLE		digitalToggle(GPIOB,GPIO_Pin_0)
//#define LED1_OFF		digitalHi(GPIOB,GPIO_Pin_0)
//#define LED1_ON			digitalLo(GPIOB,GPIO_Pin_0)
		 
	#ifdef __cplusplus
	}
	#endif
#endif
