#include "Cancellation.h"
#include "systick.h"
#include "board.h"
#include <stdio.h>
#include "EN25Q256.h"
//������·��ѹ�� �ȴ��ȶ�ʱ��
#define Dwell_Time 20
//�ֵ���ϸ���Ĳ���
#define Coarse_step_len 62    
#define Fine_step_len   24
//���ݲ�����ADCֵ ����
#define AD_Min_Error_Precision 0
//΢�����ȷ�Χ mini +-   
#define Precision_range 124 
//��׼�ο���ѹ3.3V
#define AD_DA_REF 3300  
//��·������ѹ
uint16_t DA_Q_G[4]={1302,1240,1116,1054};
uint16_t DA_I_G[4]={1448,1426,1178,1880};
//һ·���ص�ѹ
uint16_t AD_V_G[4]={4095,4095,4095,4095};
//uint16_t AD_Min = 4095;
//1.	�ֵ� 
//bool     	0���ѯ��С��ֵ,-1      û�в�ѯ��
//*AD_Min 	��������ʱ����ȱʡ�� 	������ѹ����
//DA_I_mini ��������ʱ I·��ѹ���
//DA_Q_C  DA_I_C ָ�� ������·ADC��ֵ  
int Cancellation_Coarse_ctrl(uint16_t *AD_Min/*in out*/,uint16_t *DA_I_mini/*in out*/,uint16_t *DA_Q_mini/*in out*/)
{
	
	//1 �ȿ�ȴ� I ����  Qɨ�� 
	//I DA_I_mini ֮ǰ���߳�ʼ���趨��СֵDA_I
	uint16_t DA_Q;		//�ֲ����� ɨ�� Q·��ѹ
	uint16_t DA_I;		//�ֲ����� ɨ�� I·��ѹ
	uint16_t AD_Value;	//�ֲ����� �����������ص�ѹ
	uint16_t temp_i = *DA_I_mini; //���ݴ����ָ������ ʹ�þֲ���������
	uint16_t temp_q = *DA_Q_mini;
	uint16_t ad_min_tmp = *AD_Min;
printf("-------------------------------------1-----------------------------------------");		
	DAC_SetChannel2Data(DAC_Align_12b_R, temp_i);     //DA_I�̶�Ϊ1V���
	for(DA_Q=124; DA_Q<=1860; DA_Q += Coarse_step_len)	
	{
		DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q);
		Delay_ms(Dwell_Time);
		AD_Value=Get_ADC_Value(ADC1);
		if(AD_Value < ad_min_tmp + AD_Min_Error_Precision)
		{
			 ad_min_tmp =  AD_Value;//������Сֵ
			 //temp_i =  DA_I_mini;    //��¼��Сֵy����
			 temp_q =  DA_Q;    //��¼��Сֵx����
		}
	//	printf("��һ�� �ֵ�DA_Q:%d,ad_min :%d\n",DA_Q,ad_min_tmp);
	}
	
		printf("��һ�� �ֵ���temp_q:%d,ad_min :%d\n",temp_q,ad_min_tmp);
	//if(temp_q == *DA_Q_mini)	return -1;//�ж���û���µĶ�λ û�з��ؼ� �м�����λ	

printf("-------------------------------------2-----------------------------------------");  
	//2 Q���� Iɨ��
	DAC_SetChannel1Data(DAC_Align_12b_R, temp_q);     //DA_Q�̶�Ϊ���������С��	
	for(DA_I=124; DA_I<=1860; DA_I += Coarse_step_len)
	{
		
		DAC_SetChannel2Data(DAC_Align_12b_R, DA_I);
		Delay_ms(Dwell_Time);
		AD_Value=Get_ADC_Value(ADC1);
		if(AD_Value <= ad_min_tmp + AD_Min_Error_Precision)
		{
			 ad_min_tmp =  AD_Value;//������Сֵ
			 temp_i =  DA_I;    //��¼��Сֵy����
			 //temp_q =  temp_q;    //��¼��Сֵx����
		}
	//	printf("��һ�� �ֵ�DA_I:%d,ad_min :%d\n",DA_I,ad_min_tmp);		
	}
		printf("��һ�� �ֵ���temp_i:%d,ad_min :%d\n",temp_i,ad_min_tmp);
		if(temp_i != *DA_I_mini)
		{//�¶�λ������ ��ֵ
			*DA_I_mini = temp_i;
			*DA_Q_mini = temp_q;
			*AD_Min = ad_min_tmp;
			printf("ad_min_tmp1  :%d\n",ad_min_tmp);
			printf("DA_I :%d \n",temp_i);
			printf("DA_Q :%d \n",temp_q);			
			return 0;	
		}
}
//ϸ��������
int Cancellation_Fine_Ctrl(uint16_t *AD_Min/*in out*/,uint16_t *DA_I_mini/*in out*/,uint16_t *DA_Q_mini/*in out*/)
{

	uint16_t DA_Q;		//�ֲ����� ɨ�� Q·��ѹ
	uint16_t DA_I;		//�ֲ����� ɨ�� I·��ѹ
	uint16_t AD_Value;	//�ֲ����� �����������ص�ѹ
	uint16_t temp_i ; //���ݴ����ָ������ ʹ�þֲ���������
	uint16_t temp_q ;
	uint16_t ad_min_tmp ;

	uint16_t right_i;//i ��λ�߽�
	uint32_t right_q;//q ��λ�߽�
	uint16_t left_i;//i �ͱ߽�
	uint16_t left_q;//q �ͱ߽�
	//ȷ����һ��ֵ�֮�����������Χ΢��
	if(Cancellation_Coarse_ctrl(AD_Min,DA_I_mini,DA_Q_mini) == -1) return -1; //û�����¶�λ
	
	temp_q = DA_Q = *DA_Q_mini;
	temp_i = DA_I = *DA_I_mini;
	ad_min_tmp = *AD_Min;
	
	//��ʼ�����¶�λ�� ��΢�� 
	if(DA_I > Precision_range)
		DA_I=DA_I - Precision_range;
	else
		DA_I = 0;
	
	if(DA_Q > Precision_range)
		DA_Q=DA_Q - Precision_range;
	else
		DA_Q = 0;
	//�������߽�
	left_i =  DA_I;     left_q =  DA_Q;
	//�ұ߽� ����߽�� 2��Precision_range
	right_i = left_i+2*Precision_range;
	right_q = left_q+2*Precision_range;
	printf("-------------------------------------3-----------------------------------------");
	for(DA_I = left_i; DA_I<=right_i; DA_I += Fine_step_len)
	{
		for(DA_Q = left_q;DA_Q <= right_q;DA_Q += Fine_step_len) //�����ӵķ���
		{
			DAC_SetChannel2Data(DAC_Align_12b_R, DA_I);
			DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q);
			Delay_ms(Dwell_Time);
			AD_Value=Get_ADC_Value(ADC1);
			if(AD_Value <= ad_min_tmp)
			{
				ad_min_tmp =  AD_Value;//������Сֵ
				temp_i =  DA_I;    //��¼��Сֵy����
				temp_q =  DA_Q;    //��¼��Сֵx����

			}
		//	printf("�ڶ��� ϸ��DA_I:%d,DA_Q:%d,ad_min :%d\n",DA_I,DA_Q,ad_min_tmp);					
		}
	}
	printf("�ڶ��� ϸ����temp_i:%d,temp_q:%d,ad_min :%d\n",temp_i,temp_q,ad_min_tmp);			
	if(temp_i != *DA_I_mini || temp_q != *DA_Q_mini)
	{//�¶�λ������ ��ֵ
		*DA_I_mini = temp_i;
		*DA_Q_mini = temp_q;
		*AD_Min = ad_min_tmp;
		
		printf("ad_min_tmp2  :%d\n",ad_min_tmp);
		printf("DA_I :%d \n",temp_i);
		printf("DA_Q :%d \n",temp_q);
		return 0;	
	}
	
}

void Cancellation_Fast(void)//���ٶ���
{
	int i = 0 ;
	int temp = 0;
	uint16_t AD_Value;	//�ֲ����� �����������ص�ѹ
	for(i=0;i < 4;i++)
	{
		Cancellation_Fine_Ctrl(&AD_V_G[i],&DA_I_G[i],&DA_Q_G[i]);			
		printf("\n AD_V_G[%d] is %d \n",i,AD_V_G[i]) ;
		
	}
	
	printf("-------------------------------------4-----------------------------------------\n");	
	//����
	for(i = 0;i < 4;i++)
	{
		//4 3 2 1   ��С������λ��
		if(AD_V_G[i] <= AD_V_G[temp])	
		{
			//��¼��Сֵ���
			temp = i;
		}				
	}
	printf("temp : %d\n",temp);
	DAC_SetChannel2Data(DAC_Align_12b_R, DA_I_G[temp]);
    DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q_G[temp]);	
	Delay_ms(Dwell_Time);
	AD_Value=Get_ADC_Value(ADC1);
	printf("\n DA_I is :%d   \n",DA_I_G[temp]);		
	printf("\n DA_Q is :%d   \n",DA_Q_G[temp]);
	printf("\n AD_Min is :%d \n",AD_Value);	
	DA_I_G[0] = DA_I_G[temp];
	DA_Q_G[0] = DA_Q_G[temp];
}

void Cancellation_Low(void)
{
	uint16_t DA_Q;		//�ֲ����� ɨ�� Q·��ѹ
	uint16_t DA_I;		//�ֲ����� ɨ�� I·��ѹ
	uint16_t temp_i ; //���ݴ����ָ������ ʹ�þֲ���������
	uint16_t temp_q ;
	uint16_t AD_Value;	//�ֲ����� �����������ص�ѹ	
	uint16_t AD_Min = 4095;
	printf("=--------------------------5------------------------=");
	for(DA_I=124; DA_I<=1860; DA_I += 62)
	{
		for(DA_Q = 124;DA_Q <= 1860;DA_Q += 62) //�����ӵķ���
		{
			DAC_SetChannel2Data(DAC_Align_12b_R, DA_I);
			DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q);
			printf("DA_Q is :%d\n",DA_Q);
			printf("DA_I is :%d\n",DA_I);
			Delay_ms(Dwell_Time);
			AD_Value=Get_ADC_Value(ADC1);
			printf("AD_Value is :%d\n",AD_Value);	
			if(AD_Value <= AD_Min)
			{
				AD_Min =  AD_Value;//������Сֵ
				temp_i =  DA_I;    //��¼��Сֵy����
				temp_q =  DA_Q;    //��¼��Сֵx����
//				printf("AD_Min  :%d\n",AD_Value);
//				printf("DA_I :%d \n",DA_I);
//				printf("DA_Q :%d \n",DA_Q);
			}		
		}
	}
	DAC_SetChannel2Data(DAC_Align_12b_R, temp_i);
	DAC_SetChannel1Data(DAC_Align_12b_R, temp_q);	
	Delay_ms(Dwell_Time);
	AD_Value=Get_ADC_Value(ADC1);
	printf("\n DA_I is :%d   \n",temp_i);		
	printf("\n DA_Q is :%d   \n",temp_q);
	printf("\n AD_Min is :%d \n",AD_Value);

	//�����ٶ������������ٶ����ο�
	DA_Q_G[0] = temp_q;
	DA_I_G[0] = temp_i;
	AD_V_G[0] = AD_Value;	
}
void Cancellation(void)
{
	Cancellation_Fast();
	configcancellparam(DA_I_G[0],DA_Q_G[0]);
}

void Cancellation00(void)
{
	Cancellation_Low();
	configcancellparam(DA_I_G[0],DA_Q_G[0]);
}

//5. ���ö�������
void configcancellparam(uint16_t I_value,uint16_t Q_value)
{
	u8 tmp_write[4] = {0};
	u8 tmp_read[4] = {0};
	//��֡
	tmp_write[0] = (I_value>>8)&0xff;
	tmp_write[1] = (I_value)&0xff;	
	
	tmp_write[2] = (Q_value>>8)&0xff;
	tmp_write[3] = (Q_value)&0xff;	

	printf("config ...\n");	
	//if(flash_Dev_ok == flash_Isok)//flash �豸����
	{
		//д��
		//Qp_DAValue��λ
		EN25Q256_Write(&tmp_write[0],cancelldi_davalue_h,1);
		//Qp_DAValue��λ
		EN25Q256_Write(&tmp_write[1],cancelldi_davalue_l,1);
		
		//Qn_DAValue��λ
		EN25Q256_Write(&tmp_write[2],cancelldq_davalue_h,1);
		//Qn_DAValue��λ
		EN25Q256_Write(&tmp_write[3],cancelldq_davalue_l,1);
			
		//��ȡ4�������ڴ�Ķ������Ƶ�ѹ����
		EN25Q256_Read(tmp_read,cancelldi_davalue_h,4);
		//�ȶ�
		if( ( tmp_write[0] == tmp_read[0] )&&( tmp_write[1] == tmp_read[1])
			&&( tmp_write[2] == tmp_read[2] )&&( tmp_write[3] == tmp_read[3]))
		{
			//����ram ״̬��־λ�� flash ��־λ
			u8 cancell_save_flag = flash_Isok;		
			EN25Q256_Write(&cancell_save_flag,cancellsave_flag,1);	
			
			//����ȫ��ram ����
//			min_SquDAValue = tmp;
			//��Ƶ��������Ҫ���ó�ȥ
			DAC_SetChannel2Data(DAC_Align_12b_R, I_value);
			DAC_SetChannel1Data(DAC_Align_12b_R, Q_value);	
			//������������������������
		}else{
			printf("The cancell is not save\n");
			u8 cancell_save_flag = flash_Notok;		
			EN25Q256_Write(&cancell_save_flag,cancellsave_flag,1);				
		}
		
	}		
}

//6.��ѯ��������·��ѹ  ��Ҫ�ȳ�ʼ��DAC ���
u8 Getcancellparam()
{
	u8 res = 0;
	//�洢��ʱ��ȡ���ݵ���ʱ����
	unsigned short int flash_temp_h = 0;
	unsigned short int flash_temp_l = 0;	
	u8 flashMessage_temp = 0;
	printf("Read flash Getcancellparam....\r\n");
	EN25Q256_Read(&flashMessage_temp,cancellsave_flag,1);	
	if(flashMessage_temp == flash_Isok)//0x0A:����
	{
		//flash�洢���� ���µ�ȫ�ֱ���
		//1.���±�־λ
		//2.��ȡ�洢�豸��Ƶ���ò���  ��Ƶ����
		flash_temp_h = flash_temp_l = 0;
		EN25Q256_Read((u8 *)&flash_temp_h,cancelldi_davalue_h,1);//Qp_DAValue��λ
		EN25Q256_Read((u8 *)&flash_temp_l,cancelldi_davalue_l,1);//Qp_DAValue��λ
		DA_I_G[0] = (flash_temp_h<<8)|flash_temp_l;

		flash_temp_h = flash_temp_l = 0;
		EN25Q256_Read((u8 *)&flash_temp_h,cancelldq_davalue_h,1);//Qn_DAValue��λ
		EN25Q256_Read((u8 *)&flash_temp_l,cancelldq_davalue_l,1);//Qn_DAValue��λ
		DA_Q_G[0] = (flash_temp_h<<8)|flash_temp_l;
					
		
		//��Ƶ��������Ҫ���ó�ȥ
		DAC_SetChannel2Data(DAC_Align_12b_R, DA_I_G[0]);
		DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q_G[0]);	
		res = 0;
	}else{
		//��Ƶ��������Ҫ���ó�ȥ
		DAC_SetChannel2Data(DAC_Align_12b_R, DA_I_G[0]);
		DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q_G[0]);
		res = 1;
		printf("Getcancellparam is not ok\n");
	}
//	printf("min_SquDAValue.Squ_Value=%lld,Qp_DA=%d,Qn_DA=%d,Ip_DA=%d,In_DA=%d\n",
//			min_SquDAValue.Squ_Value,min_SquDAValue.Qp_DAValue,min_SquDAValue.Qn_DAValue,min_SquDAValue.Ip_DAValue,min_SquDAValue.In_DAValue);			
		
	return res;	
}
