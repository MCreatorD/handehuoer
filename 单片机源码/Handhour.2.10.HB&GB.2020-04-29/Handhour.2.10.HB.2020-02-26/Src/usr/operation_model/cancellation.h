#ifndef _Cancellation_H_
#define _Cancellation_H_

#include "stm32f4xx.h"


	#ifdef __cplusplus
	 extern "C" {
	#endif
		
		 void Cancellation(void);
		 void Cancellation00(void);
		 
		 //两路给出电压
		extern uint16_t DA_Q_G[4];
		extern  uint16_t DA_I_G[4];
		//一路返回电压
		//extern  uint16_t AD_Value;
		 void configcancellparam(uint16_t I_value,uint16_t Q_value);
		 u8 Getcancellparam();

	#ifdef __cplusplus
	}
	#endif	

#endif
	