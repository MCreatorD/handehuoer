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

/*************外部变量****************/
extern pFunction Jump_To_Application;
extern uint32_t JumpAddress;
/***********************************/
/*************版本控制***************/
/*		
	@BaseIAP						基础版本引导应用区程序(ApplicationAddress)
	@BaseFlashIAP 			引导程序实现对于FLASH的读取升级应用程序
	@BaseFlashUartIAP 	引导程序实现通过Uart使用Ymodle协议下载,APP程序,FPGA,字库等
*/
#define BaseIAP			 				0
#define BaseFlashIAP 				1
#define BaseFlashUartIAP		0



int main(void) 	
{  
	//对于version的不同预编译选择不同功能
	#if BaseIAP
			//DownLoad_USART_Configuration();	//这里做打印信息使用
			//SerialPutString("Handset bootloader BaseIAP\r\n");
			iap_load_app(ApplicationAddress);
			//SerialPutString("no user Program\r\n\n");	
			while(1); 		
	
	#elif BaseFlashIAP 
	
			//SPI 初始化外部FLASH 用来读取外部FLASH里面的标志位判断程序再写入内部FLASH
			EN25Q256_Init();
			//DownLoad_USART_Configuration();	//这里做打印信息使用
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
			Printf_USART_Configuration(); //使用Printf 需要勾选MicroLIB
			EN25Q256_Init();
			//FLASH_TEST();
			delay_init();
			//init_epcs();
			//init_fpga();//恢复fpga读取epcs权限
			SerialPutString("Handset bootloader BaseFlashUartIAP\r\n");
			BspTim2Init();//启动定时器
			Main_Menu();
			BspTim2Close();//关闭倒计时
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
//	//init_fpga();//恢复fpga读取epcs权限
//	SerialPutString("Handset bootloader V1.0\r\n");//116k
//	BspTim2Init();//启动定时器
//	Main_Menu();
//	
//	
//	BspTim2Close();//关闭倒计时
//	iap_load_app(ApplicationAddress);
//	SerialPutString("no user Program\r\n\n");	
//	while(1); 
}
