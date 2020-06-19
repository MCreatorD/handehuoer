#ifndef __FPGA_EPCS_H
#define __FPGA_EPCS_H
#include "stm32f4xx.h"
//GPIO¿Ú

	#ifdef __cplusplus
	 extern "C" {
	#endif

	#define EPCS_SPI_MO_PIN 			GPIO_Pin_13
	#define EPCS_NCS_PIN			 	GPIO_Pin_14
	#define EPCS_SPI_CLK_PIN 			GPIO_Pin_15
	#define EPCS_PORT_1                 GPIOD
	#define EPCS_CLK_1 					RCC_AHB1Periph_GPIOD

	#define EPCS_SPI_MI_PIN				GPIO_Pin_6
	#define FPGA_nCONFIG_PIN			GPIO_Pin_8
	#define FPGA_nCE_PIN				GPIO_Pin_9
	#define EPCS_PORT_2					GPIOC
	#define EPCS_CLK_2					RCC_AHB1Periph_GPIOC
		 
	#define FPGA_CONF_DONE_PIN    		GPIO_Pin_8
	#define EPCS_PORT_3           		GPIOA
	#define EPCS_CLK_3					RCC_AHB1Periph_GPIOA		
	
	#define EPCS_nCE_H			 	GPIO_SetBits(EPCS_PORT_2 , FPGA_nCE_PIN)
	#define EPCS_nCE_L		 		GPIO_ResetBits(EPCS_PORT_2 , FPGA_nCE_PIN)
	
	#define EPCS_nCONFIG_H			 GPIO_SetBits(EPCS_PORT_2 , FPGA_nCONFIG_PIN)
	#define EPCS_nCONFIG_L		 	GPIO_ResetBits(EPCS_PORT_2 , FPGA_nCONFIG_PIN)	

	#define FPGA_CONF_DONE    GPIO_ReadInputDataBit(EPCS_PORT_3,FPGA_CONF_DONE_PIN)
	void FPGA_EPCS(void);
	void FPGA_EPCS_Write(void);
	uint8_t epcs_read_silicon_id();
		
	#ifdef __cplusplus
	}
	#endif	
#endif
	