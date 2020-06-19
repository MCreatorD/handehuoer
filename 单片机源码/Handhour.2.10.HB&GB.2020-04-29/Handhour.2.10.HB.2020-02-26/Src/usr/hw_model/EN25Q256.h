#ifndef __EN25Q256_H
#define __EN25Q256_H

#include "stm32f4xx.h"
#include "usart.h"
#include "spi.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif

#define EN25Q256 0x1c18

extern u16 EN25Q256_TYPE;					//定义W25QXX芯片型号

#define EN25Q256_SPIx_CS_PIN                   GPIO_Pin_15
#define EN25Q256_SPIx_CS_GPIO_PORT             GPIOA
#define EN25Q256_SPIx_CS_GPIO_CLK              RCC_AHB1Periph_GPIOA

		 
//W25QXX的片选信号
#define EN25Q256_Enable 	GPIO_ResetBits(EN25Q256_SPIx_CS_GPIO_PORT,EN25Q256_SPIx_CS_PIN)
#define EN25Q256_Disable 	GPIO_SetBits(EN25Q256_SPIx_CS_GPIO_PORT,EN25Q256_SPIx_CS_PIN)
     		
				 
////////////////////////////////////////////////////////////////////////////
 
//指令表
#define EN25Q256_WriteEnable		 	    0x06 
#define EN25Q256_WriteDisable		 		0x04 
#define EN25Q256_ReadStatusReg	  			0x05 
#define EN25Q256_WriteStatusReg	  			0x01 
#define EN25Q256_ReadData		      		0x03 
#define EN25Q256_FastReadData		    	0x0B 
#define EN25Q256_FastReadDual			 	0x3B 
#define EN25Q256_PageProgram				0x02 
#define EN25Q256_BlockErase					0xD8 
#define EN25Q256_SectorErase				0x20 
#define EN25Q256_ChipErase					0xC7 
#define EN25Q256_PowerDn					0xB9 
#define EN25Q256_ReleasePowerDown		    0xAB 
#define EN25Q256_DeviceID					0xAB 
#define EN25Q256_ManufactDeviceID			0x90 
#define EN25Q256_JedecDeviceID				0x9F 

void EN25Q256_Init(void);
u32  EN25Q256_ReadID(void);  	    		//读取FLASH ID
u8	 EN25Q256_ReadSR(void);        		//读取状态寄存器 
void EN25Q256_Write_SR(u8 sr);  			//写状态寄存器
void EN25Q256_Write_Enable(void);  		//写使能 
void EN25Q256_Write_Disable(void);		//写保护
void EN25Q256_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void EN25Q256_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //读取flash
void EN25Q256_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//写入flash
void EN25Q256_Erase_Chip(void);    	  	//整片擦除
void EN25Q256_Erase_Sector(u32 Dst_Addr);	//扇区擦除
void EN25Q256_Wait_Busy(void);           	//等待空闲
void EN25Q256_PowerDown(void);        	//进入掉电模式
void EN25Q256_WAKEUP(void);				//唤醒

/*状态标志*/
#define flash_Isok      0x0A
#define flash_Notok     0x00

/*
*外部flash分配
*用户存储配置信息（1M  0 ~ 0x100000） 字库(6M 0x100000 ~ 0x600000+0x100000) 历史数据（24M 0x700000 ~ 0x700000+0x1800000 ） 升级（1M 1F00000~2000000）
*日志存储索引2K 信息占用256K 分配在历史数据区域
*修改统一与汉德霍尔版本 6M字库空间未使用
*用户存储配置信息（1M  0 ~ 0x100000） 字库(6M 0x100000 ~ 0x600000+0x100000) 日志数据1M(0x700000 ~ 0x800000) 历史数据（23M 0x800000 ~ 0x700000+0x1800000 ） 升级（1M 1F00000~2000000）
*/

//宏定义 扇区 块 总容量
#define CHIPSET_PAGE               (uint32_t)256
#define CHIPSET_SECTOR_SIZE		   (uint32_t)(CHIPSET_PAGE * 16)
#define CHIPSET_BLOCK_SIZE		   (uint32_t)(CHIPSET_SECTOR_SIZE * 16)
//#define CHIPSET_TOTAL_SIZE		   (uint32_t)(CHIPSET_BLOCK_SIZE  * 128) //8M


//----------------用户存储配置信息（1M  0 ~ 0x100000）-----------------------
#define SN_STORAGE_flag  		0
#define SN_STORAGE_ADDR  		SN_STORAGE_flag+1
#define SN_STORAGE_SIZE  		32

#define CONFIG_STORAGE_flag 	SN_STORAGE_ADDR+SN_STORAGE_SIZE
#define CONFIG_STORAGE_ADDR 	CONFIG_STORAGE_flag+1
#define CONFIG_STORAGE_SIZE 	4096

#define POWER_PA_flag     		CONFIG_STORAGE_ADDR+CONFIG_STORAGE_SIZE
#define POWER_PA_ADDR     		POWER_PA_flag+1
#define POWER_PA_SIZE     		4

#define GETCFG__flag			POWER_PA_ADDR+POWER_PA_SIZE
#define GETCFG_ADDR 			GETCFG__flag+1
#define GETCFG_SIZE         	4096


#define cancellsave_flag        GETCFG_ADDR+GETCFG_SIZE //对消值存储正常标志位	0x0A:正常 00:不正常
#define cancelldi_davalue_h     cancellsave_flag+1 //Di_DAValue高位
#define cancelldi_davalue_l     cancellsave_flag+2 //Di_DAValue低位

#define cancelldq_davalue_h     cancellsave_flag+3 //Dq_DAValue高位
#define cancelldq_davalue_l     cancellsave_flag+4 //Dq_DAValue低位

#define Rest_config              cancelldq_davalue_l+1
		
//----------------字库(6M 0x100000 ~ 0x600000+0x100000)-----------------------

#define FontisOK 		 0x100000-1
#define Fontbin_unigbk   0x100000
#define ugbksize         0x2C800     //178k * 1024
#define Fontbin_gbk12    Fontbin_unigbk+ugbksize
#define gbk12size        0x8D400     //565 *1024

#define Fontbin_gbk16    Fontbin_gbk12+gbk12size
#define gbk16size        0xBCC00     //755*1024

#define Fontbin_gbk24    Fontbin_gbk16+gbk16size
#define gkb24size        0x1A5000    //1684K*1024

#define Fontbin_gbk32    Fontbin_gbk24+gkb24size

//-----------------日志数据（1M 0x700000  ~ 0x800000 ）-----------------
#define LOG_SYS_INDEX_ADDRESS      (uint32_t)(CHIPSET_SECTOR_SIZE * 1792)
#define LOG_SYS_INFO_ADDRESS       (uint32_t)(CHIPSET_SECTOR_SIZE * 1794)

//----------------历史数据（24M 0x800000 ~ 0x700000+0x1800000 ）----------------


//----------------升级（1M 1F00000 ~ 2000000）-----------------------
//最后1M存储程序文件
//FLASH_SIZE      =32*1024*1024   //FLASH 大小为32M字节  扇区4k对齐
//Dev_version_flag 31*1024*1024 = 32505856 
//#define bin_flag        32505856-5 //占1个字节程序文件正常标志位	0x0A:正常 00:不正常
//#define bin_size        bin_flag+1  //占4个字节
//#define bin_start       bin_size+4  //程序文件开始地址32505856  保证起始地址4K对齐效率更高
#define bin_flag        32505856-25 //占1个字节程序文件正常标志位	0x0A:正常 00:不正常
#define bin_size        bin_flag+1  //占4个字节
#define bin_name        bin_size+4  //占20个字节
#define bin_start       bin_name+20  //程序文件开始地址32505856  保证起始地址4K对齐效率更高



	#ifdef __cplusplus
	}
	#endif
	
#endif 
	