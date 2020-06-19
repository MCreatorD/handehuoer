#include "lowpower.h"
#include "fpga_pwr_en.h"
#include "cpu_5v_en.h"
#include <stdio.h>
#include "Bsp_init.h"
//����ģʽ��PA0����

void init_lowpower()
{
	//������Դ����ĵ�Ԫ��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	
	/*���WU״̬λ*/
	PWR_ClearFlag (PWR_FLAG_WU);
	/* ʹ��WKUP���ŵĻ��ѹ��� ��ʹ��PA0  WKUP����������*/
	//PWR_WakeUpPinCmd (ENABLE);	
}

void lowpower()//�������ģʽ
{
	/* �������ģʽ WKUP���������ػ���*/
	//�ر����������ĵ�Դ
	//1.�ر�FPGA��
	FPGA_PWR_EN_L;//�����Ƶ��Ҳ�Ͷ���
	CPU_5V_EN_L;
	init_lowpower();
	PWR_EnterSTANDBYMode();	
}

void PWR_STOP()//ֹͣģʽ
{
	//�ر����������ĵ�Դ
	//1.�ر�FPGA��
	FPGA_PWR_EN_L;//�����Ƶ��Ҳ�Ͷ���
	CPU_5V_EN_L;
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);  //ͣ��ģʽ
	//SystemInit();//�˳��͹��ĺ��ʱ��������
}

void PWR_SLEEP()//˯��ģʽ   CM3 �ں�ֹͣ��������Ȼ����
{
	//���»�������˯��ģʽ �����жϻ���
	//PWR_Regulator_ON  ��Դ�����͹��� ���ѻ���û�ӳ�
	//PWR_STOPEntry_WFI �жϻ���
	printf("����˯��ģʽ\n");
	//PWR_EnterUnderDriveSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
	//����˯��ģʽ
	__WFI();	//WFIָ�����˯��,�����жϻ���
	//printf("�˳�˯��ģʽ\n");
	//PWR_EnterSleepMode();
	//SystemInit();//�˳��͹��ĺ��ʱ��������
}

//�����λ
void SoftReset(void)
{
	//������������Ͱ�װ�������������
	//set_reset_config(flash_Isok);
	__set_FAULTMASK(1); // �ر������ж�
	NVIC_SystemReset(); // ��λ
}

            

            
