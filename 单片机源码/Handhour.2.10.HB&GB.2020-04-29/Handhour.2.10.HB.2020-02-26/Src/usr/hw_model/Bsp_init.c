#include "Bsp_init.h"
#include "fpga_download.h"
//IO口初始化
void init_gpio(void)
{
	SEC_3V3_EN_Init();//PA6 产生3.3 安全模块
	RF_5V5_EN_Init(); //PE7 新版rf5v5的使能
	CPU_5V_EN_Init(); //PA1 原扳机改为新版的cpu5v_en
	BUZZER_LED_Init();//PC5	读卡提示（LED + Beep）
	LED_Init();       //PC4 mcu_led 单片机指示灯
	
	//各个单元上电
	All_On();
}

//外设初始化
void init_StdPeriph(void)
{
	//总中断分组 1 bits for pre-emption priority  3 bits for subpriority
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	I2C1_Configuration();	
	//配置printf命令所用的串口（串口1）
	USART_Configuration();  //1   3
	//配置通信用的串口（串口3）
	Comm_USART_Configuration(115200); // 0  0
	//配置实时时钟
	initRtc();         
	configMsTick();//设定1ms的系统时钟滴答	
}

//与FPGA 和 安全模块通信口初始化
void init_Fpga_Sm(void)
{
	FPGA_PWR_EN_Init();
	FPGA_PWR_EN_L;   //配置FPGA供电 上电检测到低功耗进入
	Sim_SPI_Init(); //FPGA通信口初始化     模拟SPI
	SPI2_Init();    //安全模块通信口初始化 硬件SPI2
	SM_GPIO_Init(); //控制安全模块的IO口
	FPGA_Init();    //FPGA IO口和电源 和低功耗模式
	FPGA_EPCS();	//下载所需要的口 和 配置运行模式
	//init_fpga();
}

//ADC采集和DAC输出初始化，配置射频3.3v
void init_ADC_DAC(void)
{
	Board_DAC_Init();
	Board_ADC_Init();
	//RFPowerON(2700);//开射频电源 进行调试	
}

void All_Off(void)
{
	FPGA_LowPower(1);
	SEC_3V3_EN_L;   //产生3.3 安全模块
	RF_5V5_EN_L;	//产生射频5V5
	CPU_5V_EN_L;	//ADC/DAC低功耗 需要时开启	
	RFPowerOFF();//PA ctrl 关闭
	FPGA_PWR_EN_L;//相关射频电也就断了
	MCU_LED_L;  	//关闭  自身LED	
}

void All_On(void)
{
	SEC_3V3_EN_H;   //产生3.3 安全模块
	RF_5V5_EN_H;	//产生射频5V5
	CPU_5V_EN_H;	//ADC/DAC低功耗 需要时开启	
	MCU_LED_H;  	//关闭  点亮自身	
}

u8 Reset_Read()
{
	u8 flashMessage_temp = 0;

	EN25Q256_Read(&flashMessage_temp,Rest_config,1); 
	if(flashMessage_temp == flash_Isok)//0x0A:正常
	{
		
		set_reset_config(flash_Notok);
		return 1;
		
	}else{
		
		return 0;
	}
}

void set_reset_config(u8 set_flag)
{
	//更新存储的外部标志位	
	unsigned char Rest_Flash_Flag= set_flag;//0x0A:正常					
	EN25Q256_Write(&Rest_Flash_Flag,Rest_config,1);

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
void hw_uart_init(void)
{
	//1. IO口初始化
	init_gpio();
	//2. 外设初始化
	init_StdPeriph();
	//3. FLASH（En25a256） SPI3
	EN25Q256_Init();	
	//printf("start\r\n");	
}
void hw_init(void)
{

	Beep_Delay(250);
	//4. 低功耗模式时钟初始化
	init_lowpower();	
		//5. 与FPGA 和 安全模块通信口初始化
	init_Fpga_Sm();	
	//Delay_ms(2000);
	FPGA_START_H;
	FPGA_SINGLEEND_H;
	//6. ADC采集和DAC输出初始化，配置射频3.3v
	init_ADC_DAC();
}
