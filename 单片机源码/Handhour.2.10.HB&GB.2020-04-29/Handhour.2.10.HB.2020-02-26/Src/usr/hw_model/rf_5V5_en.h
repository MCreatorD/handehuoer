#ifndef __RF_5V5_EN_H
#define __RF_5V5_EN_H
#include "stm32f4xx.h"
	#ifdef __cplusplus
	 extern "C" {
	#endif
		 
	//Ӳ����ԴPE7
	#define RF_5V5_EN_PORT 		    	GPIOE	
	#define RF_5V5_EN_PIN           GPIO_Pin_7 
	#define RF_5V5_EN_CLK           RCC_AHB1Periph_GPIOE

	//��ʼ������
	extern void RF_5V5_EN_Init(void);
		 
	/* ֱ�Ӳ����Ĵ����ķ�������IO */
	#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬	 
	//��������
	#define RF_5V5_EN_H             GPIO_SetBits(RF_5V5_EN_PORT , RF_5V5_EN_PIN)
	#define RF_5V5_EN_L             GPIO_ResetBits(RF_5V5_EN_PORT , RF_5V5_EN_PIN)
//	#define POWER_LED_TOGGLE        digitalToggle(SEC_3V3_EN_PORT,SEC_3V3_EN_PIN)

///* ֱ�Ӳ����Ĵ����ķ�������IO */
//#define	digitalHi(p,i)			{p->BSRR=i;}			//����Ϊ�ߵ�ƽ		
//#define digitalLo(p,i)			{p->BRR=i;}				//����͵�ƽ
//#define digitalToggle(p,i)		{p->ODR ^=i;}			//�����ת״̬


///* �������IO�ĺ� */
//#define LED1_TOGGLE		digitalToggle(GPIOB,GPIO_Pin_0)
//#define LED1_OFF		digitalHi(GPIOB,GPIO_Pin_0)
//#define LED1_ON			digitalLo(GPIOB,GPIO_Pin_0)
		 
	#ifdef __cplusplus
	}
	#endif
#endif
