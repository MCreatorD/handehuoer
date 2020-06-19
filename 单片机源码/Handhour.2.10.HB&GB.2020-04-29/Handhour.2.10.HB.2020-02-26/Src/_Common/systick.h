#ifndef SYSTICK_H
#define SYSTICK_H

	#ifdef __cplusplus
	 extern "C" {
	#endif
		 
		#include <stddef.h>
		#include "stm32f4xx.h"

		extern u16 runLed_Count;
		extern u16 simRun_Count;
		extern u16 rfOper_OvertimerCount; //用作射频操作的超时。
		extern u16 rfOper_DelayCount; //用作射频操作的延时控制 
		 
		 
		void configMsTick();
		void Delay_ms(u16 time);
		 
		 
	#ifdef __cplusplus
	}
	#endif
	
#endif