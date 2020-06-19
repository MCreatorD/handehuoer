#ifndef __BSP_INIT_H
#define __BSP_INIT_H

//IO口初始化的头文件
#include "sec_3v3_en.h"
#include "rf_5V5_en.h"
#include "cpu_5v_en.h"
#include "buzzer_led.h"
#include "mcu_led.h"

//外设初始化
#include "config.h"
#include "i2c.h"
#include "uart_bsp_comm.h"
#include "usart.h"
#include "rtc.h"
#include "systick.h"

//FPGA 和 安全模块通信口初始化头文件
#include "sim_spi.h"
#include "spi.h"
#include "sm.h"
#include "fpga_pwr_en.h"
#include "fpga.h"
#include "fpga_epcs.h"
#include "sim_spi1.h"
	
//ADC  DAC  ad5602
#include "AD5602.h"
#include "board.h"

//外部FLASH
#include "EN25Q256.h"

//低功耗的头文件
#include "lowpower.h"

//主控平台
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif
		//IO口初始化
		//外设初始化
		//与FPGA 和 安全模块通信口初始化
		//ADC采集和DAC输出初始化，配置射频3.3v
		void init_gpio(void);
		void init_StdPeriph(void);
		void init_Fpga_Sm(void); 
		void init_ADC_DAC();
		void All_Off(void);
		void All_On(void);
		u8 Reset_Read();
		void hw_uart_init(void);
		void hw_init(void);
		void set_reset_config(u8 set_flag);
	#ifdef __cplusplus
	}
	#endif
#endif 