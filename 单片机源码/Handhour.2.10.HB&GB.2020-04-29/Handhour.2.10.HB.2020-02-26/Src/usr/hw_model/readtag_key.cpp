#include "readtag_key.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "main_application.h"
//文件后缀在配置中修改成了c++
//使用C语言明确中断函数名连接规范
//extern "C" void EXTI1_IRQHandler(void);
extern "C" void EXTI9_5_IRQHandler(void);
volatile unsigned char KEY_TYPE=0;		  //当前按下的键值类型
/*
*函数名：readtagkeyinit
*说明：配置扳机按钮的工作模式
*备注：PA1 SW_KEY:默认上拉，触发按下到地
*按下扳机读卡,电平触发则一直进入中断,所以配置成边沿触发
*/
void readtagkeyinit()
{
    EXTI_InitTypeDef EXTI_InitStructure;//中断线
    GPIO_InitTypeDef GPIO_InitStructure;//IO口配置	
		NVIC_InitTypeDef NVIC_InitStructure;//中断配置	
	
		/* Enable GPIOA clock */
		RCC_AHB1PeriphClockCmd(READTAG_KEY_CLK, ENABLE);								  
		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(READTAG_KEY_SYSCFG_CLK, ENABLE);	

		/* Configure SW_KEY pin as input floating */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	 //选择输入模式
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     //选择上拉
		GPIO_InitStructure.GPIO_Pin =  READTAG_KEY_PIN;		 //选择io
		GPIO_Init(READTAG_KEY_PORT, &GPIO_InitStructure);
		
		/* Connect EXTI Line to SW_KEY pin */
		SYSCFG_EXTILineConfig(READTAG_KEY_SYSCFG_EXTI_Port_Sources, READTAG_KEY_EXTI_PinSource);
	
		/* Configure EXTI Line */
		EXTI_InitStructure.EXTI_Line    = READTAG_KEY_EXTI_Line;			   //选择路径1
		EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;	        //选择中断模式
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //选择边沿触发
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;				   //使能中断
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set EXTI Line1 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = READTAG_KEY_EXTI_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);	
}

extern u8  func_index;//主程序此时所在界面的索引值
void waitTime(unsigned int cnt)
{
	int base_cnt  = 0xff;
	while(cnt--){
		while(base_cnt--)
		{
			__nop();
			__nop();		
		}
		base_cnt = 0xff;
	}
}
//开始读卡的触发
//mode :1 上位机下发的读卡规则，删除了默认规则
//mode :0 扳机触发的规则，删除上位机的规则，配置默认规则
void start_readTag(uint8_t mode)
{
	//退出低功耗
	FPGA_LowPower(0);

	//上位机下发则会有股则但是会删除默认3001规则
	if(mode == 1)
	{
		//蓝牙下发时 停止GPI触发的规则
		OperationSpec_Manager *opManger = g_pMainApplication->getOpProcess()->getOpSpecManager();
		opManger->stopSelectSpec(3001);
//		KEY_TYPE = BLUE_PRES;//蓝牙触发 反馈到显示界面
	}else{
		//先查看停止上位机下发的规则
		g_pMainApplication->getOpProcess()-> getOpSpecManager()->deleteSelectSpec(1);
		//为手持机新加一默认的读卡规则。
//		g_pMainApplication->getOpProcess()->getOpSpecManager()->initDefaultSelectSpec();
//		g_pMainApplication->getOpProcess()->getOpSpecManager()->addSelectSpec(g_pMainApplication->getOpProcess()->getOpSpecManager()->m_pCurrentSelectSpec,false);
//		OperationSpec_Manager *opManger = g_pMainApplication->getOpProcess()->getOpSpecManager();
		//opManger->startSelectSpec(3001);
		KEY_TYPE = TRIG_DOWN;//GPI触发读卡条件
	}
}


void stop__readTag(uint8_t mode)
{
	//松手进入低功耗
	FPGA_LowPower(1);			
	//松手  停在车牌显示界面
	KEY_TYPE = TRIG_UP;


//			FPGA_LOWPOWER_L;
//			TIM_Cmd(TIM3, ENABLE);//读卡结束打开闭定时器
//			printf("KEY_TYPE = TRIG_UP && open TIM3 \n");					
}
/*
*函数名：EXTI1_IRQHandler
*说明：  扳机按钮按下触发的中断
*备注：PA1 SW_KEY:默认上拉，触发按下到地
*触发后依据电平判断是按下 还是松手
*/
//void EXTI1_IRQHandler(void)//Key_Exit_Init PA1 SW_KEY
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(READTAG_KEY_EXTI_Line) != RESET)//确保是否产生了EXTI Line中断
	{			
		if(Tag_key==0)
		{
			//waitTime(20);
			if(Tag_key == 0)
			{
				//OLED_Display_On();//恢复显示
				start_readTag(0);			
			}

		}
		else
		{	
			waitTime(50);
			if(Tag_key != 0)
			{
				//松手进入低功耗
				FPGA_LowPower(1);			
				//松手  停在车牌显示界面
				KEY_TYPE = TRIG_UP;
				//立即触发判断是否读取到数据
				//g_pMainApplication->gui_menu->state2_2();
				//恢复界面刷新
				//start_tim_refresh();
	//			FPGA_LOWPOWER_L;
	//			TIM_Cmd(TIM3, ENABLE);//读卡结束打开闭定时器
	//			printf("KEY_TYPE = TRIG_UP && open TIM3 \n");					
				//func_index = 18;//扳机按下直接到读卡界面
			}
		}
		printf("--------------EXTI1_IRQHandler------------------\n");	
		EXTI_ClearITPendingBit(READTAG_KEY_EXTI_Line); //清除中断标志位    
	}
}