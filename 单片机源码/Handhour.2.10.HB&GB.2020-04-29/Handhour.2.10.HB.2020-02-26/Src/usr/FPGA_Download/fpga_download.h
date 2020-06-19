#ifndef __FPGA_DOWNLOAD_H
#define __FPGA_DOWNLOAD_H
#include "stm32f4xx.h"

#define EPCS_PAGE_SIZE 0x100		//页面大小
#define EPCS_PAGE_ADDR 0x100		//页面起始地址
#define EPCS_PAGE_MASK 0x0ff		//页面大小掩码

#define EPCS_SECTOR_SIZE  0x010000
#define EPCS_SECTOR_ADDR  0x010000
#define EPCS_SECTOR_MASK  0x00ffff

//EPCS命令

#define EPCS_WRITE_ENABLE		0x06
#define EPCS_WRITE_DISABLE	  	0x04

#define EPCS_READ_STATUS		0x05
#define EPCS_READ_BYTE			0x03
#define EPCS_READ_SILICON_ID	0xab//读取silicon ID
#define EPCS_Fast_read          0x0b

#define EPCS_WRITE_STATUS		0x01
#define EPCS_WRITE_BYTE			0x02

#define EPCS_ERASE_BULK			0xc7//擦除所有扇区，大约需要5-10s
#define EPCS_ERASE_SECTOR		0xd8//擦除指定扇区

//器件ID
#define EPCS16_ID 				0x14

	#ifdef __cplusplus
	 extern "C" {
	#endif
		//epcs写入时的地址
		extern uint32_t epcswriteaddr;
		extern uint32_t lastepcswriteaddr;
		int init_epcs();//上电初始化
		void init_fpga();
		void RestFpga();
		void epc_erase_allSector();//擦除全部扇区
		char epcs_write(uint8_t *data,u32 WriteAddr,unsigned int len);//写入函数
		void epcs_read_all(void);
		void EPCS_Bus_Free(void );
		char epcs_writeTest();
		void epcs_readTest();
	
	#ifdef __cplusplus
	}
	#endif		
#endif