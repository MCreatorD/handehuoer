#ifndef _RTC_H_
#define _RTC_H_

#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif

	int setRtc(unsigned int utc);
	void RTC_TimeStampShow(void);
	void initRtc();

	#ifdef __cplusplus
	}
	#endif		 
		 
#endif
