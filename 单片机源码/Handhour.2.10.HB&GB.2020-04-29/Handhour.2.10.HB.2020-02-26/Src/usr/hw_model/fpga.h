#ifndef __FPGA_H__
#define __FPGA_H__

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//GPIO口

	#ifdef __cplusplus
	 extern "C" {
	#endif
		 
		#define FPGA_START_OUT_IO     GPIO_Pin_2 //给FPGA 的start脚          MCU2FPGA_IO1
		#define FPGA_START_Port		    GPIOE
		#define FPGA_RWOVER_IN_IO     GPIO_Pin_3 //FGPA读写操作结束指示脚    MCU2FPGA_IO2
		#define FPGA_RWOVER_Port	    GPIOE
		#define FPGA_NEEDCOMM_IN_IO   GPIO_Pin_4 //FPGA需要通信指示脚        MCU2FPGA_IO0
		#define FPGA_NEEDCOMM_Port    GPIOE
		//该号用以配合多标签
		//该脚只在行标新FPGA上才用到
		#define FPGA_SINGLEEND_OUT_IO  GPIO_Pin_5 //FPGA单张标签结束脚       MCU2FPGA_IO4
		#define FPGA_SINGLEEND_Port		 GPIOE
		#define FPGA_TURNEND_IN_IO     GPIO_Pin_6 //FPGA单轮结束指示脚			 MCU2FPGA_IO5
		#define FPGA_TURNEND_Port			 GPIOE

		//低功耗脚
	    #define FPGA_LOWPOWER_IO         GPIO_Pin_9 //FPGA进入低功耗脚       MCU2FPGA_IO3 	 
		#define FPGA_LOWPOWER_Port			 GPIOE
		 
		#define FPGA_POWER_OUT_IO      GPIO_Pin_12                            //FPGA_PWR_EN 
		#define FPGA_POWER_Port        GPIOD
		#define FPGA_STATE_IN_IO       GPIO_Pin_7                     //FPGA_nState信号输入脚
		#define FPGA_STATE_Port 	     GPIOC
		#define FPGA_CE_OUT_IO		     GPIO_Pin_9                            //FPGA_nCE 
		#define FPGA_CE_Port		       GPIOC 
		#define FPGA_CONF_OUT_IO       GPIO_Pin_8                              //FPGA_nCONFIG 
		#define FPGA_CONF_Port         GPIOC 
		#define FPGA_CONFDONE_IN_IO    GPIO_Pin_8                            //FPGA_CONF_DONE
		#define FPGA_CONFDONE_Port     GPIOA

		#define FPGA_SIGNGPIO_ENABLE()     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE)
		
		#define FPGA_POWER_ENABLE()			   RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE)
		#define FPGA_STATE_ENABLE()        RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
		#define FPGA_CE_ENABLE()           RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
		#define FPGA_CONF_ENABLE()         RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
		#define FPGA_CONFDONE_ENABLE()     RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE)
	
		#define FPGA_LOWPOWER_H          GPIO_SetBits(FPGA_LOWPOWER_Port , FPGA_LOWPOWER_IO)
		#define FPGA_LOWPOWER_L          GPIO_ResetBits(FPGA_LOWPOWER_Port , FPGA_LOWPOWER_IO)	
		//#define FPGA_LOWPOWER_L        FPGA_LOWPOWER_H
		
		#define FPGA_POWER_H          	 GPIO_SetBits(FPGA_POWER_Port , FPGA_POWER_OUT_IO)
		#define FPGA_POWER_L        	   GPIO_ResetBits(FPGA_POWER_Port , FPGA_POWER_OUT_IO) 
		#define FPGA_START_H       	    GPIO_SetBits(FPGA_START_Port , FPGA_START_OUT_IO) 
		#define FPGA_START_L       	    GPIO_ResetBits(FPGA_START_Port , FPGA_START_OUT_IO) 
		#define FPGA_RWOVER        	    GPIO_ReadInputDataBit(FPGA_RWOVER_Port,FPGA_RWOVER_IN_IO)
		#define FPGA_NEEDCOMM     	     GPIO_ReadInputDataBit(FPGA_NEEDCOMM_Port,FPGA_NEEDCOMM_IN_IO)
		#define FPGA_SINGLEEND_H   	    GPIO_SetBits(FPGA_SINGLEEND_Port , FPGA_SINGLEEND_OUT_IO) 
		#define FPGA_SINGLEEND_L   	    GPIO_ResetBits(FPGA_SINGLEEND_Port , FPGA_SINGLEEND_OUT_IO) 
		#define FPGA_TURNEND           GPIO_ReadInputDataBit(FPGA_TURNEND_Port,FPGA_TURNEND_IN_IO)
		
		#define FPGA_STATE             GPIO_ReadInputDataBit(FPGA_STATE_Port,FPGA_STATE_IN_IO)
		#define FPGA_CONFDONE          GPIO_ReadInputDataBit(FPGA_CONFDONE_Port,FPGA_CONFDONE_IN_IO)
		#define FPGA_CE_H              GPIO_SetBits(FPGA_CE_Port , FPGA_CE_OUT_IO) 
		#define FPGA_CE_L              GPIO_ResetBits(FPGA_CE_Port , FPGA_CE_OUT_IO) 
		#define FPGA_CONF_H            GPIO_SetBits(FPGA_CONF_Port , FPGA_CONF_OUT_IO) 
		#define FPGA_CONF_L            GPIO_ResetBits(FPGA_CONF_Port , FPGA_CONF_OUT_IO) 

		void FPGA_Init(void);
		
		void FPGA_LowPower(int isLow);
		//extern unsigned char lowpower_change;
		 
	#ifdef __cplusplus
	}
	#endif	

#endif		 