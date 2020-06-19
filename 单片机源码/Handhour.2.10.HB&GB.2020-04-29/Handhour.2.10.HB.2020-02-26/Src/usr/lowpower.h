#ifndef __LOWPOWER_H__
#define __LOWPOWER_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
		 
	void init_lowpower();		 
	void lowpower();	
	void PWR_STOP();		
	void PWR_SLEEP();//Ë¯ÃßÄ£Ê½		 
	void SoftReset(void);		
		 
	#ifdef __cplusplus
	}
	#endif	

#endif			
	