#ifndef __READTAG_KEY_H
#define __READTAG_KEY_H
#include "stm32f4xx.h"
/* GPIO */
#define READTAG_KEY_PIN           				 	    GPIO_Pin_7
#define READTAG_KEY_PORT            					  GPIOC
/* GPIO clock */
#define READTAG_KEY_CLK            						  RCC_AHB1Periph_GPIOC
/* SYSCFG clock */
#define READTAG_KEY_SYSCFG_CLK       						RCC_APB2Periph_SYSCFG
/* Connect EXTI Line to SW_KEY pin */
#define READTAG_KEY_SYSCFG_EXTI_Port_Sources    EXTI_PortSourceGPIOC
#define READTAG_KEY_EXTI_PinSource              EXTI_PinSource7
//选择路径1
#define READTAG_KEY_EXTI_Line				            EXTI_Line7
/* Enable and set EXTI Line1 Interrupt to the lowest priority */
#define READTAG_KEY_EXTI_IRQn             		  EXTI9_5_IRQn

#define TRIG_DOWN   6
#define TRIG_UP     0	//中断二次按下	

	#ifdef __cplusplus
	 extern "C" {
	#endif
		 
		//配置扳机按钮的工作模式
		void readtagkeyinit();
		void waitTime(unsigned int cnt);	 
	#ifdef __cplusplus
	}
	#endif
		void start_readTag(uint8_t mode);
		void stop__readTag(uint8_t mode);	
	extern volatile unsigned char  KEY_TYPE;		  //当前按下的键值类型 
//读取扳机IO口电平判断是按下还是松手
#define Tag_key GPIO_ReadInputDataBit(READTAG_KEY_PORT,READTAG_KEY_PIN)


#endif