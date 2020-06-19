#include "Bsp_init.h"
#include "fpga_download.h"
//IO�ڳ�ʼ��
void init_gpio(void)
{
	SEC_3V3_EN_Init();//PA6 ����3.3 ��ȫģ��
	RF_5V5_EN_Init(); //PE7 �°�rf5v5��ʹ��
	CPU_5V_EN_Init(); //PA1 ԭ�����Ϊ�°��cpu5v_en
	BUZZER_LED_Init();//PC5	������ʾ��LED + Beep��
	LED_Init();       //PC4 mcu_led ��Ƭ��ָʾ��
	
	//������Ԫ�ϵ�
	All_On();
}

//�����ʼ��
void init_StdPeriph(void)
{
	//���жϷ��� 1 bits for pre-emption priority  3 bits for subpriority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	I2C1_Configuration();	
	//����printf�������õĴ��ڣ�����1��
	USART_Configuration();  //1   3
	//����ͨ���õĴ��ڣ�����3��
	Comm_USART_Configuration(115200); // 0  0
	//����ʵʱʱ��
	initRtc();         
	configMsTick();//�趨1ms��ϵͳʱ�ӵδ�	
}

//��FPGA �� ��ȫģ��ͨ�ſڳ�ʼ��
void init_Fpga_Sm(void)
{
	FPGA_PWR_EN_Init();
	FPGA_PWR_EN_L;   //����FPGA���� �ϵ��⵽�͹��Ľ���
	Sim_SPI_Init(); //FPGAͨ�ſڳ�ʼ��     ģ��SPI
	SPI2_Init();    //��ȫģ��ͨ�ſڳ�ʼ�� Ӳ��SPI2
	SM_GPIO_Init(); //���ư�ȫģ���IO��
	FPGA_Init();    //FPGA IO�ں͵�Դ �͵͹���ģʽ
	FPGA_EPCS();	//��������Ҫ�Ŀ� �� ��������ģʽ
	//init_fpga();
}

//ADC�ɼ���DAC�����ʼ����������Ƶ3.3v
void init_ADC_DAC(void)
{
	Board_DAC_Init();
	Board_ADC_Init();
	//RFPowerON(2700);//����Ƶ��Դ ���е���	
}

void All_Off(void)
{
	FPGA_LowPower(1);
	SEC_3V3_EN_L;   //����3.3 ��ȫģ��
	RF_5V5_EN_L;	//������Ƶ5V5
	CPU_5V_EN_L;	//ADC/DAC�͹��� ��Ҫʱ����	
	RFPowerOFF();//PA ctrl �ر�
	FPGA_PWR_EN_L;//�����Ƶ��Ҳ�Ͷ���
	MCU_LED_L;  	//�ر�  ����LED	
}

void All_On(void)
{
	SEC_3V3_EN_H;   //����3.3 ��ȫģ��
	RF_5V5_EN_H;	//������Ƶ5V5
	CPU_5V_EN_H;	//ADC/DAC�͹��� ��Ҫʱ����	
	MCU_LED_H;  	//�ر�  ��������	
}

u8 Reset_Read()
{
	u8 flashMessage_temp = 0;

	EN25Q256_Read(&flashMessage_temp,Rest_config,1); 
	if(flashMessage_temp == flash_Isok)//0x0A:����
	{
		
		set_reset_config(flash_Notok);
		return 1;
		
	}else{
		
		return 0;
	}
}

void set_reset_config(u8 set_flag)
{
	//���´洢���ⲿ��־λ	
	unsigned char Rest_Flash_Flag= set_flag;//0x0A:����					
	EN25Q256_Write(&Rest_Flash_Flag,Rest_config,1);

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
void hw_uart_init(void)
{
	//1. IO�ڳ�ʼ��
	init_gpio();
	//2. �����ʼ��
	init_StdPeriph();
	//3. FLASH��En25a256�� SPI3
	EN25Q256_Init();	
	//printf("start\r\n");	
}
void hw_init(void)
{

	Beep_Delay(250);
	//4. �͹���ģʽʱ�ӳ�ʼ��
	init_lowpower();	
		//5. ��FPGA �� ��ȫģ��ͨ�ſڳ�ʼ��
	init_Fpga_Sm();	
	//Delay_ms(2000);
	FPGA_START_H;
	FPGA_SINGLEEND_H;
	//6. ADC�ɼ���DAC�����ʼ����������Ƶ3.3v
	init_ADC_DAC();
}
