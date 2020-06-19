/*
ע�����
��1������STM32��Ҫ����nCE��nConfig�źţ���nCE�øߣ�nConfig���ͣ����EPCS�Ŀ���Ȩ�������EPCS������������ɺ���Ҫ�ͷ��������ܽţ���nCE���ͣ�nConfig�øߡ�
��2��EPCS�ļ���Ϊ��sckΪ����״̬Ϊ�ߵ�ƽ����������ΪSCK�ĵڶ��������أ����������أ�ע��ǰ����SCK����Ϊ�ߣ�
*/
#include "fpga_download.h"
#include "fpga_epcs.h"
#include "sim_spi1.h"
#include "stdlib.h"
#include "fpga_pwr_en.h"
#include <stdio.h>
#include "systick.h"
//epcsд��ʱ�ĵ�ַ
uint32_t epcswriteaddr = 0;
uint32_t lastepcswriteaddr = 0;
void delay_us(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=10;  //�Լ�����
      while(i--) ;    
   }
}
int init_epcs()
{
	//оƬ�ϵ���ID	
	FPGA_PWR_EN_Init();
	FPGA_PWR_EN_H;
	//����epcs���ƹܽ��������״̬
	FPGA_EPCS_Write();
	//(1) nCE�ø�  nConfig����
	EPCS_nCE_H;   EPCS_nCONFIG_L;
	//��ʼ�� SPI
	Sim_SPI1_Init();

	Delay_ms(5);
	//sckΪ����״̬Ϊ�ߵ�ƽ����������ΪSCK�ĵڶ���������
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
	//�ϵ�
	FPGA_PWR_EN_Init();
	FPGA_PWR_EN_H;	
	//(1) nCE�õ�  nConfig����
	FPGA_EPCS();
	EPCS_nCE_L;   EPCS_nCONFIG_H;	
}

void RestFpga()
{
	//�ϵ�
	FPGA_PWR_EN_Init();
	FPGA_PWR_EN_L;	
	//�ϵ�50ms ��֤���ͷ����
	Delay_ms(50);
	//�ϵ�
	init_fpga();
	
}
void EPCS_Bus_Free(void )
{
	  //EPCS_nCE_L;
	  //���Ӧ���ڶ��FPGA�����������أ���NCONFIG��Ȼ���ֵ͵�ƽ�����������FPGA������
	  //EPCS_nCONFIG_H;
 	  NSS1_H;SCK1_H;MOSI1_H;
	  //�ж�����
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
* ����ȫ�� �ȼ��ڽ�flash --> 0xff
* Ȼ��д�����ݾ���������д��ֻ��ͷ����ȫ��
*/

void epc_erase_allSector()
{
    //Step 1: ����оƬ�����������������̴��80S��������ÿ�������һ��оƬ״̬�����á�.����������
	printf("\r\nErase Chip");
	epcs_erase_bulk();
	do{
		printf(".");
    Delay_ms(100);//0.1S���һ��
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

	//Step 2: д���ļ�
	page_addr = WriteAddr;	//��ǰ��Ҫ��д���ַ
	off_data = 0;			//д������ݵ��ֽ���  ��ʼΪ0 ��ÿ������ EPCS_PAGE_SIZE
		
		
	while(off_data < len )
	{
		//��д������ݴ���1ҳ��������д��1ҳ��С������д��ʣ���С
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

		//��������ƫƫ������ҳ���ַ
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
	while(page_addr<0x1ffff0)//2M�ռ��β
	{	
		epcs_read_page(page_addr,readbuf,0x100);	

		//��ӡ���������ݣ�ÿҳǰ16�ֽڣ�������
		//printf("\n%8.8x: ",page_addr);
		for(int i = 0;i < 0x100;++i) {
			if(i %16  == 0)
			{
				printf("\r\n");
				printf("%8.8x: ",page_addr+i);
			}
			printf("%2.2X ",*(readbuf+i));
		}	

		//��������ƫƫ������ҳ���ַ
		page_addr += EPCS_PAGE_ADDR;	
	}

}