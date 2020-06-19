#include <stddef.h>
#include <rt_heap.h>  //_init_alloc 
#include <string.h>

#include "Bsp_init.h"

#include "main_application.h"

#include "main.h"
#include "readtag_key.h"

/*
*	main函数。
*
*
*/
int main(void){	
	//先进入关机状态
	//1. IO口初始化
	init_gpio();
	//2. 外设初始化
	init_StdPeriph();
	printf("start\r\n");
	Beep_Delay(250);
	//3. FLASH（En25a256） SPI3
	EN25Q256_Init();	
	//4. 低功耗模式时钟初始化
	init_lowpower();
	//5. 与FPGA 和 安全模块通信口初始化
	init_Fpga_Sm();	
	//Delay_ms(2000);
	FPGA_START_H;
	FPGA_SINGLEEND_H;
	//6. ADC采集和DAC输出初始化，配置射频3.3v
	init_ADC_DAC();
	//8. 注册C++相关的程序 NEW初始化 

	Main_Application m_MainApp;
	//9. 开机对消完毕 进入低功耗
	FPGA_LowPower(1);
	//10 扳机读卡初始化
	readtagkeyinit();
	//11. 开机初始化完毕后的提示
	Beep_Delay(250);
	//上报上位机开机完成
	m_MainApp.Report_OpenFinish();
	//---------------------前方测试通过------------------------------//	
	while (1)
	{		
		m_MainApp.run();
	}
}


//蜂鸣器提示函数
void Beep_Times(u8 n_times)
{
	while(n_times--)
	{
		BUZZER_LED_H;	
		Delay_ms(250);
		BUZZER_LED_L;
		if(n_times<=0){
			return;
		}
		Delay_ms(250);		
	}
}

void Beep_Delay(unsigned short ms){
	
		BUZZER_LED_H;	
		Delay_ms(ms);
		BUZZER_LED_L;
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */


  /* Infinite loop */
  while (1)
  {

  }
}
#endif
