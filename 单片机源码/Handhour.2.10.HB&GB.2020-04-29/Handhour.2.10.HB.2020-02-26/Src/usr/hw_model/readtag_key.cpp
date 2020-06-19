#include "readtag_key.h"
#include "stm32f4xx.h"
#include <stdio.h>
#include "main_application.h"
//�ļ���׺���������޸ĳ���c++
//ʹ��C������ȷ�жϺ��������ӹ淶
//extern "C" void EXTI1_IRQHandler(void);
extern "C" void EXTI9_5_IRQHandler(void);
volatile unsigned char KEY_TYPE=0;		  //��ǰ���µļ�ֵ����
/*
*��������readtagkeyinit
*˵�������ð����ť�Ĺ���ģʽ
*��ע��PA1 SW_KEY:Ĭ���������������µ���
*���°������,��ƽ������һֱ�����ж�,�������óɱ��ش���
*/
void readtagkeyinit()
{
    EXTI_InitTypeDef EXTI_InitStructure;//�ж���
    GPIO_InitTypeDef GPIO_InitStructure;//IO������	
		NVIC_InitTypeDef NVIC_InitStructure;//�ж�����	
	
		/* Enable GPIOA clock */
		RCC_AHB1PeriphClockCmd(READTAG_KEY_CLK, ENABLE);								  
		/* Enable SYSCFG clock */
		RCC_APB2PeriphClockCmd(READTAG_KEY_SYSCFG_CLK, ENABLE);	

		/* Configure SW_KEY pin as input floating */
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;	 //ѡ������ģʽ
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;     //ѡ������
		GPIO_InitStructure.GPIO_Pin =  READTAG_KEY_PIN;		 //ѡ��io
		GPIO_Init(READTAG_KEY_PORT, &GPIO_InitStructure);
		
		/* Connect EXTI Line to SW_KEY pin */
		SYSCFG_EXTILineConfig(READTAG_KEY_SYSCFG_EXTI_Port_Sources, READTAG_KEY_EXTI_PinSource);
	
		/* Configure EXTI Line */
		EXTI_InitStructure.EXTI_Line    = READTAG_KEY_EXTI_Line;			   //ѡ��·��1
		EXTI_InitStructure.EXTI_Mode    = EXTI_Mode_Interrupt;	        //ѡ���ж�ģʽ
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  //ѡ����ش���
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;				   //ʹ���ж�
		EXTI_Init(&EXTI_InitStructure);

		/* Enable and set EXTI Line1 Interrupt to the lowest priority */
		NVIC_InitStructure.NVIC_IRQChannel = READTAG_KEY_EXTI_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);	
}

extern u8  func_index;//�������ʱ���ڽ��������ֵ
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
//��ʼ�����Ĵ���
//mode :1 ��λ���·��Ķ�������ɾ����Ĭ�Ϲ���
//mode :0 ��������Ĺ���ɾ����λ���Ĺ�������Ĭ�Ϲ���
void start_readTag(uint8_t mode)
{
	//�˳��͹���
	FPGA_LowPower(0);

	//��λ���·�����й����ǻ�ɾ��Ĭ��3001����
	if(mode == 1)
	{
		//�����·�ʱ ֹͣGPI�����Ĺ���
		OperationSpec_Manager *opManger = g_pMainApplication->getOpProcess()->getOpSpecManager();
		opManger->stopSelectSpec(3001);
//		KEY_TYPE = BLUE_PRES;//�������� ��������ʾ����
	}else{
		//�Ȳ鿴ֹͣ��λ���·��Ĺ���
		g_pMainApplication->getOpProcess()-> getOpSpecManager()->deleteSelectSpec(1);
		//Ϊ�ֳֻ��¼�һĬ�ϵĶ�������
//		g_pMainApplication->getOpProcess()->getOpSpecManager()->initDefaultSelectSpec();
//		g_pMainApplication->getOpProcess()->getOpSpecManager()->addSelectSpec(g_pMainApplication->getOpProcess()->getOpSpecManager()->m_pCurrentSelectSpec,false);
//		OperationSpec_Manager *opManger = g_pMainApplication->getOpProcess()->getOpSpecManager();
		//opManger->startSelectSpec(3001);
		KEY_TYPE = TRIG_DOWN;//GPI������������
	}
}


void stop__readTag(uint8_t mode)
{
	//���ֽ���͹���
	FPGA_LowPower(1);			
	//����  ͣ�ڳ�����ʾ����
	KEY_TYPE = TRIG_UP;


//			FPGA_LOWPOWER_L;
//			TIM_Cmd(TIM3, ENABLE);//���������򿪱ն�ʱ��
//			printf("KEY_TYPE = TRIG_UP && open TIM3 \n");					
}
/*
*��������EXTI1_IRQHandler
*˵����  �����ť���´������ж�
*��ע��PA1 SW_KEY:Ĭ���������������µ���
*���������ݵ�ƽ�ж��ǰ��� ��������
*/
//void EXTI1_IRQHandler(void)//Key_Exit_Init PA1 SW_KEY
void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(READTAG_KEY_EXTI_Line) != RESET)//ȷ���Ƿ������EXTI Line�ж�
	{			
		if(Tag_key==0)
		{
			//waitTime(20);
			if(Tag_key == 0)
			{
				//OLED_Display_On();//�ָ���ʾ
				start_readTag(0);			
			}

		}
		else
		{	
			waitTime(50);
			if(Tag_key != 0)
			{
				//���ֽ���͹���
				FPGA_LowPower(1);			
				//����  ͣ�ڳ�����ʾ����
				KEY_TYPE = TRIG_UP;
				//���������ж��Ƿ��ȡ������
				//g_pMainApplication->gui_menu->state2_2();
				//�ָ�����ˢ��
				//start_tim_refresh();
	//			FPGA_LOWPOWER_L;
	//			TIM_Cmd(TIM3, ENABLE);//���������򿪱ն�ʱ��
	//			printf("KEY_TYPE = TRIG_UP && open TIM3 \n");					
				//func_index = 18;//�������ֱ�ӵ���������
			}
		}
		printf("--------------EXTI1_IRQHandler------------------\n");	
		EXTI_ClearITPendingBit(READTAG_KEY_EXTI_Line); //����жϱ�־λ    
	}
}