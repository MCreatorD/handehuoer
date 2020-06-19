#ifndef __EN25Q256_H
#define __EN25Q256_H

#include "stm32f4xx.h"
#include "usart.h"
#include "spi.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif

#define EN25Q256 0x1c18

extern u16 EN25Q256_TYPE;					//����W25QXXоƬ�ͺ�

#define EN25Q256_SPIx_CS_PIN                   GPIO_Pin_15
#define EN25Q256_SPIx_CS_GPIO_PORT             GPIOA
#define EN25Q256_SPIx_CS_GPIO_CLK              RCC_AHB1Periph_GPIOA

		 
//W25QXX��Ƭѡ�ź�
#define EN25Q256_Enable 	GPIO_ResetBits(EN25Q256_SPIx_CS_GPIO_PORT,EN25Q256_SPIx_CS_PIN)
#define EN25Q256_Disable 	GPIO_SetBits(EN25Q256_SPIx_CS_GPIO_PORT,EN25Q256_SPIx_CS_PIN)
     		
				 
////////////////////////////////////////////////////////////////////////////
 
//ָ���
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
u32  EN25Q256_ReadID(void);  	    		//��ȡFLASH ID
u8	 EN25Q256_ReadSR(void);        		//��ȡ״̬�Ĵ��� 
void EN25Q256_Write_SR(u8 sr);  			//д״̬�Ĵ���
void EN25Q256_Write_Enable(void);  		//дʹ�� 
void EN25Q256_Write_Disable(void);		//д����
void EN25Q256_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void EN25Q256_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);   //��ȡflash
void EN25Q256_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);//д��flash
void EN25Q256_Erase_Chip(void);    	  	//��Ƭ����
void EN25Q256_Erase_Sector(u32 Dst_Addr);	//��������
void EN25Q256_Wait_Busy(void);           	//�ȴ�����
void EN25Q256_PowerDown(void);        	//�������ģʽ
void EN25Q256_WAKEUP(void);				//����

/*״̬��־*/
#define flash_Isok      0x0A
#define flash_Notok     0x00

/*
*�ⲿflash����
*�û��洢������Ϣ��1M  0 ~ 0x100000�� �ֿ�(6M 0x100000 ~ 0x600000+0x100000) ��ʷ���ݣ�24M 0x700000 ~ 0x700000+0x1800000 �� ������1M 1F00000~2000000��
*��־�洢����2K ��Ϣռ��256K ��������ʷ��������
*�޸�ͳһ�뺺�»����汾 6M�ֿ�ռ�δʹ��
*�û��洢������Ϣ��1M  0 ~ 0x100000�� �ֿ�(6M 0x100000 ~ 0x600000+0x100000) ��־����1M(0x700000 ~ 0x800000) ��ʷ���ݣ�23M 0x800000 ~ 0x700000+0x1800000 �� ������1M 1F00000~2000000��
*/

//�궨�� ���� �� ������
#define CHIPSET_PAGE               (uint32_t)256
#define CHIPSET_SECTOR_SIZE		   (uint32_t)(CHIPSET_PAGE * 16)
#define CHIPSET_BLOCK_SIZE		   (uint32_t)(CHIPSET_SECTOR_SIZE * 16)
//#define CHIPSET_TOTAL_SIZE		   (uint32_t)(CHIPSET_BLOCK_SIZE  * 128) //8M


//----------------�û��洢������Ϣ��1M  0 ~ 0x100000��-----------------------
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


#define cancellsave_flag        GETCFG_ADDR+GETCFG_SIZE //����ֵ�洢������־λ	0x0A:���� 00:������
#define cancelldi_davalue_h     cancellsave_flag+1 //Di_DAValue��λ
#define cancelldi_davalue_l     cancellsave_flag+2 //Di_DAValue��λ

#define cancelldq_davalue_h     cancellsave_flag+3 //Dq_DAValue��λ
#define cancelldq_davalue_l     cancellsave_flag+4 //Dq_DAValue��λ

#define Rest_config              cancelldq_davalue_l+1
		
//----------------�ֿ�(6M 0x100000 ~ 0x600000+0x100000)-----------------------

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

//-----------------��־���ݣ�1M 0x700000  ~ 0x800000 ��-----------------
#define LOG_SYS_INDEX_ADDRESS      (uint32_t)(CHIPSET_SECTOR_SIZE * 1792)
#define LOG_SYS_INFO_ADDRESS       (uint32_t)(CHIPSET_SECTOR_SIZE * 1794)

//----------------��ʷ���ݣ�24M 0x800000 ~ 0x700000+0x1800000 ��----------------


//----------------������1M 1F00000 ~ 2000000��-----------------------
//���1M�洢�����ļ�
//FLASH_SIZE      =32*1024*1024   //FLASH ��СΪ32M�ֽ�  ����4k����
//Dev_version_flag 31*1024*1024 = 32505856 
//#define bin_flag        32505856-5 //ռ1���ֽڳ����ļ�������־λ	0x0A:���� 00:������
//#define bin_size        bin_flag+1  //ռ4���ֽ�
//#define bin_start       bin_size+4  //�����ļ���ʼ��ַ32505856  ��֤��ʼ��ַ4K����Ч�ʸ���
#define bin_flag        32505856-25 //ռ1���ֽڳ����ļ�������־λ	0x0A:���� 00:������
#define bin_size        bin_flag+1  //ռ4���ֽ�
#define bin_name        bin_size+4  //ռ20���ֽ�
#define bin_start       bin_name+20  //�����ļ���ʼ��ַ32505856  ��֤��ʼ��ַ4K����Ч�ʸ���



	#ifdef __cplusplus
	}
	#endif
	
#endif 
	