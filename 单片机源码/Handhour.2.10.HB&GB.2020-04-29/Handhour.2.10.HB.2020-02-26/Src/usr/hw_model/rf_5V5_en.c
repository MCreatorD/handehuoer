#include "rf_5V5_en.h"

//��ʼ��PB0Ϊ�����.��ʹ������ڵ�ʱ��		    
//POWER_LED IO��ʼ��
void RF_5V5_EN_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RF_5V5_EN_CLK, ENABLE);	         

	GPIO_InitStructure.GPIO_Pin   = RF_5V5_EN_PIN;				   
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;           
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;         //����
	GPIO_Init(RF_5V5_EN_PORT, &GPIO_InitStructure);         
	GPIO_SetBits(RF_5V5_EN_PORT,RF_5V5_EN_PIN);					 //�����
}