#ifndef __BUZZER_LED_H
#define __BUZZER_LED_H
#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
	//Ӳ����ԴPC5
	#define BUZZER_LED_Port			     GPIOC	
	#define BUZZER_LED_PIN           GPIO_Pin_5 
	#define BUZZER_LED_CLK           RCC_AHB1Periph_GPIOC

	//��ʼ������
	extern void BUZZER_LED_Init(void);

	//��������
	#define BUZZER_LED_H             GPIO_SetBits(BUZZER_LED_Port , BUZZER_LED_PIN)
	#define BUZZER_LED_L             GPIO_ResetBits(BUZZER_LED_Port , BUZZER_LED_PIN)
	
	#ifdef __cplusplus
	}
	#endif
#endif
	