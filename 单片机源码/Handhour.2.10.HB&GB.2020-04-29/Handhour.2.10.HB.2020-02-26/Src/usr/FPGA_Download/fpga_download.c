/*
注意事项：
（1）首先STM32需要设置nCE和nConfig信号，即nCE置高，nConfig拉低，获得EPCS的控制权，而后对EPCS操作，操作完成后需要释放这两个管脚，即nCE拉低，nConfig置高。
（2）EPCS的极性为：sck为空闲状态为高电平，采样边沿为SCK的第二个跳变沿，（即上升沿，注意前提是SCK空闲为高）
*/
#include "fpga_download.h"
#include "fpga_epcs.h"
#include "sim_spi1.h"
#include "stdlib.h"
#include "fpga_pwr_en.h"
#include <stdio.h>
#include "systick.h"
//epcs写入时的地址
uint32_t epcswriteaddr = 0;
uint32_t lastepcswriteaddr = 0;
void delay_us(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=10;  //自己定义
      while(i--) ;    
   }
}
int init_epcs()
{
	//芯片上电检测ID	
	FPGA_PWR_EN_Init();
	FPGA_PWR_EN_H;
	//配置epcs控制管脚输入输出状态
	FPGA_EPCS_Write();
	//(1) nCE置高  nConfig拉低
	EPCS_nCE_H;   EPCS_nCONFIG_L;
	//初始化 SPI
	Sim_SPI1_Init();

	Delay_ms(5);
	//sck为空闲状态为高电平，采样边沿为SCK的第二个跳变沿
	uint8_t tmp = epcs_read_silicon_id();
	printf("The Epcs ID :%d",tmp);
	if(tmp != EPCS16_ID)
	{
		printf("The Epcs Device error\n");
		
		return 0;
	}
	
	return 1;
}

void init_fpga()
{
	//上电
	FPGA_PWR_EN_Init();
	FPGA_PWR_EN_H;	
	//(1) nCE置低  nConfig拉高
	FPGA_EPCS();
	EPCS_nCE_L;   EPCS_nCONFIG_H;	
}

void RestFpga()
{
	//断电
	FPGA_PWR_EN_Init();
	FPGA_PWR_EN_L;	
	//断电50ms 保证电释放完毕
	Delay_ms(50);
	//上电
	init_fpga();
	
}
void EPCS_Bus_Free(void )
{
	  //EPCS_nCE_L;
	  //如果应用于多块FPGA复用总线下载，让NCONFIG仍然保持低电平，以完成其他FPGA的下载
	  //EPCS_nCONFIG_H;
 	  NSS1_H;SCK1_H;MOSI1_H;
	  //切断总线
	  //FPGA_EPCS();
}

uint8_t epcs_read_silicon_id(){
	uint8_t outdata = 0x00;
	uint8_t epcs_code = EPCS_READ_SILICON_ID;
	uint8_t epcs_zero = 0x00;
    NSS1_L;

	Sim_SPI1_ReadWrite_Byte(epcs_code);
	Sim_SPI1_ReadWrite_Byte(epcs_zero);
	Sim_SPI1_ReadWrite_Byte(epcs_zero);
	Sim_SPI1_ReadWrite_Byte(epcs_zero);
	outdata = Sim_SPI1_ReadWrite_Byte(0xff);

	NSS1_H;

	return outdata;
}

uint8_t epcs_read_status()
{
	uint8_t outdata = 0x00;
	uint8_t epcs_code = EPCS_READ_STATUS;

	NSS1_L;

    Sim_SPI1_ReadWrite_Byte(epcs_code);
    outdata = Sim_SPI1_ReadWrite_Byte(0xff);
	
	NSS1_H;
	
	return outdata;
}

void epcs_write_enable(){
	uint8_t epcs_code = EPCS_WRITE_ENABLE;

    NSS1_L;

    Sim_SPI1_ReadWrite_Byte(epcs_code);

    NSS1_H;

}


void epcs_erase_bulk(){
	epcs_write_enable();

	uint8_t epcs_code = EPCS_ERASE_BULK;

    NSS1_L;

    Sim_SPI1_ReadWrite_Byte(epcs_code);

    NSS1_H;
}

void epcs_erase_sector(unsigned int addr){
	epcs_write_enable();

	uint8_t epcs_code = EPCS_ERASE_SECTOR;
	uint8_t addrlow = addr;
	uint8_t addrmiddle = addr >> 8;
	uint8_t addrhigh = addr >> 16;

    NSS1_L;

    Sim_SPI1_ReadWrite_Byte(epcs_code);
    Sim_SPI1_ReadWrite_Byte(addrhigh);
    Sim_SPI1_ReadWrite_Byte(addrmiddle);
    Sim_SPI1_ReadWrite_Byte(addrlow);

    NSS1_H;

}

void epcs_write_page(unsigned int addr,uint8_t *data,int len){
	uint8_t readbuf[0x100];
	epcs_write_enable();

	uint8_t epcs_code = EPCS_WRITE_BYTE;
	uint8_t addrlow = addr;
	uint8_t addrmiddle = addr >> 8;
	uint8_t addrhigh = addr >> 16;

    NSS1_L;

    Sim_SPI1_ReadWrite_Byte(epcs_code);
    Sim_SPI1_ReadWrite_Byte(addrhigh);
    Sim_SPI1_ReadWrite_Byte(addrmiddle);
    Sim_SPI1_ReadWrite_Byte(addrlow);
    Sim_SPI1_ReadWriteBuf(data,readbuf,len);

    NSS1_H;

}

void epcs_read_page(unsigned int addr,uint8_t *data,int len){
	uint8_t writebuf[100];
	uint8_t epcs_code = EPCS_READ_BYTE;
	uint8_t addrlow = addr;
	uint8_t addrmiddle = addr >> 8;
	uint8_t addrhigh = addr >> 16;


    NSS1_L;

    Sim_SPI1_ReadWrite_Byte(epcs_code);
    Sim_SPI1_ReadWrite_Byte(addrhigh);
    Sim_SPI1_ReadWrite_Byte(addrmiddle);
    Sim_SPI1_ReadWrite_Byte(addrlow);
    Sim_SPI1_ReadWriteBuf(writebuf,data,len);

    NSS1_H;

}
/*
* 擦除全部 等价于将flash --> 0xff
* 然后写入数据就是慢慢的写，只开头擦出全部
*/

void epc_erase_allSector()
{
    //Step 1: 擦除芯片所有扇区，擦除过程大概80S，这器件每半秒访问一次芯片状态，并用“.”当进度条
	printf("\r\nErase Chip");
	epcs_erase_bulk();
	do{
		printf(".");
    Delay_ms(100);//0.1S检测一次
	}while(epcs_read_status() != 0);	
	printf("Erase Finish\r\n");
}

char epcs_write(uint8_t *data,u32 WriteAddr,unsigned int len)
{
	
	unsigned int 	page_addr = 0;
	unsigned int 	off_data = 0;

	unsigned char 	*pdata = data;

	unsigned int 	size;
	
	unsigned char 	chksumW = 0;
	unsigned char 	chksumR = 0;

	//Step 2: 写入文件
	page_addr = WriteAddr;	//当前需要的写入地址
	off_data = 0;			//写入的数据的字节量  开始为0 后每次增加 EPCS_PAGE_SIZE
		
		
	while(off_data < len )
	{
		//当写入的内容大于1页容量，则写入1页大小，否则写入剩余大小
		if(len - off_data > EPCS_PAGE_SIZE) {
			size = EPCS_PAGE_SIZE;
		}
		else {
			size = len - off_data;
		}
		
		if(page_addr == 0x3af00)
		{
			page_addr = 0x3af00;
		}
		//printf("\rwrite page %8.8x,size=%d",page_addr,size);
		epcs_write_page(page_addr,pdata + off_data,size);
		
		delay_us(10);
		int overtime = 0;
		while(epcs_read_status() != 0){
			delay_us(10);
			overtime++;
			if(overtime>100000){
				//printf("Timer over!overtime=%d\n",overtime);
				return 0;
			}
		}

		//更新数据偏偏移量和页面地址
		off_data  += EPCS_PAGE_SIZE;
		page_addr += EPCS_PAGE_ADDR;
	}
	
	//memset(data,0,len);
	return 1;
}


void epcs_read_all(void)
{
	unsigned int 	page_addr = 0;
	unsigned int 	off_data = 0;
	
	uint8_t readbuf[0x100];//0x100
	while(page_addr<0x1ffff0)//2M空间结尾
	{	
		epcs_read_page(page_addr,readbuf,0x100);	

		//打印读出的内容，每页前16字节，调试用
		//printf("\n%8.8x: ",page_addr);
		for(int i = 0;i < 0x100;++i) {
			if(i %16  == 0)
			{
				printf("\r\n");
				printf("%8.8x: ",page_addr+i);
			}
			printf("%2.2X ",*(readbuf+i));
		}	

		//更新数据偏偏移量和页面地址
		page_addr += EPCS_PAGE_ADDR;	
	}

}