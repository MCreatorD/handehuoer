#include "cpu_5v_en.h"

//��ʼ��PA1Ϊ�����.��ʹ������ڵ�ʱ��		    
//CPU_5V_EN IO��ʼ��
void CPU_5V_EN_Init(void)
{
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_AHB1PeriphClockCmd(CPU_5V_EN_CLK, ENABLE);	         
	
 GPIO_InitStructure.GPIO_Pin   = CPU_5V_EN_PIN;				   
 GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;           
 GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;          //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;  
 GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;         //����
 GPIO_Init(CPU_5V_EN_Port, &GPIO_InitStructure);         
 GPIO_ResetBits(CPU_5V_EN_Port,CPU_5V_EN_PIN);					 //�����
}