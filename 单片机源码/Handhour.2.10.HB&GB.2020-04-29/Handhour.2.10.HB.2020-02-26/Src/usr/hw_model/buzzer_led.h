#ifndef __BUZZER_LED_H
#define __BUZZER_LED_H
#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
	//硬件资源PC5
	#define BUZZER_LED_Port			     GPIOC	
	#define BUZZER_LED_PIN           GPIO_Pin_5 
	#define BUZZER_LED_CLK           RCC_AHB1Periph_GPIOC

	//初始化函数
	extern void BUZZER_LED_Init(void);

	//方法函数
	#define BUZZER_LED_H             GPIO_SetBits(BUZZER_LED_Port , BUZZER_LED_PIN)
	#define BUZZER_LED_L             GPIO_ResetBits(BUZZER_LED_Port , BUZZER_LED_PIN)
	
	#ifdef __cplusplus
	}
	#endif
#endif
	