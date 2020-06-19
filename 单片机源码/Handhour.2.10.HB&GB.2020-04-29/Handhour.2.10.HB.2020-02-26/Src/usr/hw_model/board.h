#ifndef _BOARD_H_
#define _BOARD_H_

#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
		 void Board_DAC_Init(void);
		 void Board_ADC_Init(void);
		 uint16_t Get_ADC_Value(ADC_TypeDef* ADCx);

	#ifdef __cplusplus
	}
	#endif	

#endif
	