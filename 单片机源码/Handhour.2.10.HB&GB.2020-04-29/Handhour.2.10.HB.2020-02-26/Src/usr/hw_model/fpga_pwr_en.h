#ifndef __FPGA_PWR_EN_H
#define __FPGA_PWR_EN_H
#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
	//硬件资源
	#define FPGA_PWR_EN_Port			    GPIOD	
	#define FPGA_PWR_EN_PIN           GPIO_Pin_12 
	#define FPGA_PWR_EN_CLK           RCC_AHB1Periph_GPIOD

	//初始化函数
	void FPGA_PWR_EN_Init(void);

	//方法函数
	#define FPGA_PWR_EN_H             GPIO_SetBits(FPGA_PWR_EN_Port , FPGA_PWR_EN_PIN)
	#define FPGA_PWR_EN_L             GPIO_ResetBits(FPGA_PWR_EN_Port , FPGA_PWR_EN_PIN) 

		 #ifdef __cplusplus
	}
	#endif	
#endif
