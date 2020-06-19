#ifndef __MCU_LED_H
#define __MCU_LED_H
#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
	//硬件资源
	#define MCU_LED_Port			    GPIOC	
	#define MCU_LED_PIN           GPIO_Pin_4 
	#define MCU_LED_CLK           RCC_AHB1Periph_GPIOC

	//初始化函数
	extern void LED_Init(void);

	//方法函数
	#define MCU_LED_H             GPIO_SetBits(MCU_LED_Port , MCU_LED_PIN)
	#define MCU_LED_L             GPIO_ResetBits(MCU_LED_Port , MCU_LED_PIN) 

		 #ifdef __cplusplus
	}
	#endif	
#endif
	