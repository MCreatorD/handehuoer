#include "sec_3v3_en.h"

//��ʼ��PB0Ϊ�����.��ʹ������ڵ�ʱ��		    
//POWER_LED IO��ʼ��
void SEC_3V3_EN_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(SEC_3V3_EN_CLK, ENABLE);	         

	GPIO_InitStructure.GPIO_Pin   = SEC_3V3_EN_PIN;				   
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;           
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;         //
	GPIO_Init(SEC_3V3_EN_PORT, &GPIO_InitStructure);         
	GPIO_SetBits(SEC_3V3_EN_PORT,SEC_3V3_EN_PIN);					 //�����
}