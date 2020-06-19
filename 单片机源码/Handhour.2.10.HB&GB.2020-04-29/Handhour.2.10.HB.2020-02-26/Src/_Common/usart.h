#ifndef _USART_H
#define _USART_H

	#ifdef __cplusplus
	 extern "C" {
	#endif

#include <stdio.h>
#include "stm32f4xx.h"
//#include "config.h"
int sendchar(int ch);
void USART_Configuration(void);
void USART_NVIC_Config(void);

	#ifdef __cplusplus
	}
	#endif

#endif /*_USART_H*/
