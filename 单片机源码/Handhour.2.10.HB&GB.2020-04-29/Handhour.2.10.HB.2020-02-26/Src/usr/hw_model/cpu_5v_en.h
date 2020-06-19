#ifndef __CPU_5V_EN_H
#define __CPU_5V_EN_H
#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
		 
		//Ӳ����ԴPA1
		#define CPU_5V_EN_Port			GPIOA	
		#define CPU_5V_EN_PIN           GPIO_Pin_1 
		#define CPU_5V_EN_CLK           RCC_AHB1Periph_GPIOA

		//��ʼ������
		void CPU_5V_EN_Init(void);

		//��������
		#define CPU_5V_EN_H             GPIO_SetBits(CPU_5V_EN_Port , CPU_5V_EN_PIN)
		#define CPU_5V_EN_L             GPIO_ResetBits(CPU_5V_EN_Port , CPU_5V_EN_PIN)
		//#define CPU_5V_EN_L             CPU_5V_EN_H
		 
	#ifdef __cplusplus
	}
	#endif		
#endif
	