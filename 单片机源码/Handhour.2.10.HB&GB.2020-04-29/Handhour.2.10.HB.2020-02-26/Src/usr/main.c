#include <stddef.h>
#include <rt_heap.h>  //_init_alloc 
#include <string.h>

#include "Bsp_init.h"

#include "main_application.h"

#include "main.h"
#include "readtag_key.h"

/*
*	main������
*
*
*/
int main(void){	
	//�Ƚ���ػ�״̬
	//1. IO�ڳ�ʼ��
	init_gpio();
	//2. �����ʼ��
	init_StdPeriph();
	printf("start\r\n");
	Beep_Delay(250);
	//3. FLASH��En25a256�� SPI3
	EN25Q256_Init();	
	//4. �͹���ģʽʱ�ӳ�ʼ��
	init_lowpower();
	//5. ��FPGA �� ��ȫģ��ͨ�ſڳ�ʼ��
	init_Fpga_Sm();	
	//Delay_ms(2000);
	FPGA_START_H;
	FPGA_SINGLEEND_H;
	//6. ADC�ɼ���DAC�����ʼ����������Ƶ3.3v
	init_ADC_DAC();
	//8. ע��C++��صĳ��� NEW��ʼ�� 

	Main_Application m_MainApp;
	//9. ����������� ����͹���
	FPGA_LowPower(1);
	//10 ���������ʼ��
	readtagkeyinit();
	//11. ������ʼ����Ϻ����ʾ
	Beep_Delay(250);
	//�ϱ���λ���������
	m_MainApp.Report_OpenFinish();
	//---------------------ǰ������ͨ��------------------------------//	
	while (1)
	{		
		m_MainApp.run();
	}
}


//��������ʾ����
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
