#ifndef _AD5602_H_
#define _AD5602_H_

#include "stm32f4xx.h"
#include "i2c.h"


//#define PA_POWER  650//750 //2960

	extern uint16_t PA_POWER;

	#ifdef __cplusplus
	 extern "C" {
	#endif

		void RFPowerON(uint16_t x);
		void RFPowerOFF(void);
		 
	#ifdef __cplusplus
	}
	#endif	

#endif
