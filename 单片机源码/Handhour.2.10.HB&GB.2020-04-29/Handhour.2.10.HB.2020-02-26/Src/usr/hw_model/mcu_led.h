#ifndef __MCU_LED_H
#define __MCU_LED_H
#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
	//Ӳ����Դ
	#define MCU_LED_Port			    GPIOC	
	#define MCU_LED_PIN           GPIO_Pin_4 
	#define MCU_LED_CLK           RCC_AHB1Periph_GPIOC

	//��ʼ������
	extern void LED_Init(void);

	//��������
	#define MCU_LED_H             GPIO_SetBits(MCU_LED_Port , MCU_LED_PIN)
	#define MCU_LED_L             GPIO_ResetBits(MCU_LED_Port , MCU_LED_PIN) 

		 #ifdef __cplusplus
	}
	#endif	
#endif
	