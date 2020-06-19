#include "stmflash.h"
#include "stm32f4xx.h"
#include "BspTime.h"
#include "common.h"
#include "bsp_uart_update.h"
#include "delay.h"
#include "iap.h"
#include "flash_download.h"
#include "fpga_download.h"
#include "printf_usart.h"

/*************�ⲿ����****************/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
/***********************************/
/*************�汾����***************/
/*		
	@BaseIAP						�����汾����Ӧ��������(ApplicationAddress)
	@BaseFlashIAP 			��������ʵ�ֶ���FLASH�Ķ�ȡ����Ӧ�ó���
	@BaseFlashUartIAP 	��������ʵ��ͨ��Uartʹ��YmodleЭ������,APP����,FPGA,�ֿ��
*/
#define BaseIAP			 				0
#define BaseFlashIAP 				1
#define BaseFlashUartIAP		0



int main(void) 	
{  
	//����version�Ĳ�ͬԤ����ѡ��ͬ����
	#if BaseIAP
			//DownLoad_USART_Configuration();	//��������ӡ��Ϣʹ��
			//SerialPutString("Handset bootloader BaseIAP\r\n");
			iap_load_app(ApplicationAddress);
			//SerialPutString("no user Program\r\n\n");	
			while(1); 		
	
	#elif BaseFlashIAP 
	
			//SPI ��ʼ���ⲿFLASH ������ȡ�ⲿFLASH����ı�־λ�жϳ�����д���ڲ�FLASH
			EN25Q256_Init();
			//DownLoad_USART_Configuration();	//��������ӡ��Ϣʹ��
			//SerialPutString("Handset bootloader BaseFlashIAP\r\n");
			uint8_t tmp = FlashDownload();
//			if (tmp == 1){
//				//SerialPutString( "\rFlash update success\n\r");//
//			}else{
//				//SerialPutString("\rFlash don't have Application\n\r"); //	
//			}				
					
			
			iap_load_app(ApplicationAddress);
			//SerialPutString("no user Program\r\n\n");	
			while(1); 
				
	#elif BaseFlashUartIAP
				
			DownLoad_USART_Configuration();	
			Printf_USART_Configuration(); //ʹ��Printf ��Ҫ��ѡMicroLIB
			EN25Q256_Init();
			//FLASH_TEST();
			delay_init();
			//init_epcs();
			//init_fpga();//�ָ�fpga��ȡepcsȨ��
			SerialPutString("Handset bootloader BaseFlashUartIAP\r\n");
			BspTim2Init();//������ʱ��
			Main_Menu();
			BspTim2Close();//�رյ���ʱ
			iap_load_app(ApplicationAddress);
			SerialPutString("no user Program\r\n\n");	
			while(1); 		
				
	#endif
	
	
	
//	DownLoad_USART_Configuration();	
//	Printf_USART_Configuration();
//	//printf("hello \n");
//	EN25Q256_Init();
//	//FLASH_TEST();
//	delay_init();
//	//init_epcs();
//	//init_fpga();//�ָ�fpga��ȡepcsȨ��
//	SerialPutString("Handset bootloader V1.0\r\n");//116k
//	BspTim2Init();//������ʱ��
//	Main_Menu();
//	
//	
//	BspTim2Close();//�رյ���ʱ
//	iap_load_app(ApplicationAddress);
//	SerialPutString("no user Program\r\n\n");	
//	while(1); 
}
