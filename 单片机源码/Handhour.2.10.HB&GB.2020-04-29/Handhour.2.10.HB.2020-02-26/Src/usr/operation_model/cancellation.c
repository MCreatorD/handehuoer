#include "Cancellation.h"
#include "systick.h"
#include "board.h"
#include <stdio.h>
#include "EN25Q256.h"
//给出两路电压后 等待稳定时间
#define Dwell_Time 20
//粗调和细调的步长
#define Coarse_step_len 62    
#define Fine_step_len   24
//数据波动的ADC值 误差精度
#define AD_Min_Error_Precision 0
//微调精度范围 mini +-   
#define Precision_range 124 
//基准参考电压3.3V
#define AD_DA_REF 3300  
//两路给出电压
uint16_t DA_Q_G[4]={1302,1240,1116,1054};
uint16_t DA_I_G[4]={1448,1426,1178,1880};
//一路返回电压
uint16_t AD_V_G[4]={4095,4095,4095,4095};
//uint16_t AD_Min = 4095;
//1.	粗调 
//bool     	0则查询到小的值,-1      没有查询到
//*AD_Min 	对消最优时或者缺省的 	对消电压返回
//DA_I_mini 对消最优时 I路电压输出
//DA_Q_C  DA_I_C 指针 传出两路ADC的值  
int Cancellation_Coarse_ctrl(uint16_t *AD_Min/*in out*/,uint16_t *DA_I_mini/*in out*/,uint16_t *DA_Q_mini/*in out*/)
{
	
	//1 先跨度大 I 不变  Q扫描 
	//I DA_I_mini 之前或者初始的设定最小值DA_I
	uint16_t DA_Q;		//局部变量 扫描 Q路电压
	uint16_t DA_I;		//局部变量 扫描 I路电压
	uint16_t AD_Value;	//局部变量 采样对消返回电压
	uint16_t temp_i = *DA_I_mini; //备份传入的指针数据 使用局部变量运算
	uint16_t temp_q = *DA_Q_mini;
	uint16_t ad_min_tmp = *AD_Min;
printf("-------------------------------------1-----------------------------------------");		
	DAC_SetChannel2Data(DAC_Align_12b_R, temp_i);     //DA_I固定为1V输出
	for(DA_Q=124; DA_Q<=1860; DA_Q += Coarse_step_len)	
	{
		DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q);
		Delay_ms(Dwell_Time);
		AD_Value=Get_ADC_Value(ADC1);
		if(AD_Value < ad_min_tmp + AD_Min_Error_Precision)
		{
			 ad_min_tmp =  AD_Value;//更新最小值
			 //temp_i =  DA_I_mini;    //记录最小值y坐标
			 temp_q =  DA_Q;    //记录最小值x坐标
		}
	//	printf("第一轮 粗调DA_Q:%d,ad_min :%d\n",DA_Q,ad_min_tmp);
	}
	
		printf("第一轮 粗调后temp_q:%d,ad_min :%d\n",temp_q,ad_min_tmp);
	//if(temp_q == *DA_Q_mini)	return -1;//判断有没有新的定位 没有返回假 有继续定位	

printf("-------------------------------------2-----------------------------------------");  
	//2 Q不变 I扫描
	DAC_SetChannel1Data(DAC_Align_12b_R, temp_q);     //DA_Q固定为上轮输出最小点	
	for(DA_I=124; DA_I<=1860; DA_I += Coarse_step_len)
	{
		
		DAC_SetChannel2Data(DAC_Align_12b_R, DA_I);
		Delay_ms(Dwell_Time);
		AD_Value=Get_ADC_Value(ADC1);
		if(AD_Value <= ad_min_tmp + AD_Min_Error_Precision)
		{
			 ad_min_tmp =  AD_Value;//更新最小值
			 temp_i =  DA_I;    //记录最小值y坐标
			 //temp_q =  temp_q;    //记录最小值x坐标
		}
	//	printf("第一轮 粗调DA_I:%d,ad_min :%d\n",DA_I,ad_min_tmp);		
	}
		printf("第一轮 粗调后temp_i:%d,ad_min :%d\n",temp_i,ad_min_tmp);
		if(temp_i != *DA_I_mini)
		{//新定位的坐标 和值
			*DA_I_mini = temp_i;
			*DA_Q_mini = temp_q;
			*AD_Min = ad_min_tmp;
			printf("ad_min_tmp1  :%d\n",ad_min_tmp);
			printf("DA_I :%d \n",temp_i);
			printf("DA_Q :%d \n",temp_q);			
			return 0;	
		}
}
//细调方案二
int Cancellation_Fine_Ctrl(uint16_t *AD_Min/*in out*/,uint16_t *DA_I_mini/*in out*/,uint16_t *DA_Q_mini/*in out*/)
{

	uint16_t DA_Q;		//局部变量 扫描 Q路电压
	uint16_t DA_I;		//局部变量 扫描 I路电压
	uint16_t AD_Value;	//局部变量 采样对消返回电压
	uint16_t temp_i ; //备份传入的指针数据 使用局部变量运算
	uint16_t temp_q ;
	uint16_t ad_min_tmp ;

	uint16_t right_i;//i 高位边界
	uint32_t right_q;//q 高位边界
	uint16_t left_i;//i 低边界
	uint16_t left_q;//q 低边界
	//确定第一组粗调之后以这个做范围微调
	if(Cancellation_Coarse_ctrl(AD_Min,DA_I_mini,DA_Q_mini) == -1) return -1; //没有重新定位
	
	temp_q = DA_Q = *DA_Q_mini;
	temp_i = DA_I = *DA_I_mini;
	ad_min_tmp = *AD_Min;
	
	//开始在重新定位下 做微调 
	if(DA_I > Precision_range)
		DA_I=DA_I - Precision_range;
	else
		DA_I = 0;
	
	if(DA_Q > Precision_range)
		DA_Q=DA_Q - Precision_range;
	else
		DA_Q = 0;
	//运算出左边界
	left_i =  DA_I;     left_q =  DA_Q;
	//右边界 是左边界加 2倍Precision_range
	right_i = left_i+2*Precision_range;
	right_q = left_q+2*Precision_range;
	printf("-------------------------------------3-----------------------------------------");
	for(DA_I = left_i; DA_I<=right_i; DA_I += Fine_step_len)
	{
		for(DA_Q = left_q;DA_Q <= right_q;DA_Q += Fine_step_len) //先往加的方向
		{
			DAC_SetChannel2Data(DAC_Align_12b_R, DA_I);
			DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q);
			Delay_ms(Dwell_Time);
			AD_Value=Get_ADC_Value(ADC1);
			if(AD_Value <= ad_min_tmp)
			{
				ad_min_tmp =  AD_Value;//更新最小值
				temp_i =  DA_I;    //记录最小值y坐标
				temp_q =  DA_Q;    //记录最小值x坐标

			}
		//	printf("第二轮 细调DA_I:%d,DA_Q:%d,ad_min :%d\n",DA_I,DA_Q,ad_min_tmp);					
		}
	}
	printf("第二轮 细调后temp_i:%d,temp_q:%d,ad_min :%d\n",temp_i,temp_q,ad_min_tmp);			
	if(temp_i != *DA_I_mini || temp_q != *DA_Q_mini)
	{//新定位的坐标 和值
		*DA_I_mini = temp_i;
		*DA_Q_mini = temp_q;
		*AD_Min = ad_min_tmp;
		
		printf("ad_min_tmp2  :%d\n",ad_min_tmp);
		printf("DA_I :%d \n",temp_i);
		printf("DA_Q :%d \n",temp_q);
		return 0;	
	}
	
}

void Cancellation_Fast(void)//快速对消
{
	int i = 0 ;
	int temp = 0;
	uint16_t AD_Value;	//局部变量 采样对消返回电压
	for(i=0;i < 4;i++)
	{
		Cancellation_Fine_Ctrl(&AD_V_G[i],&DA_I_G[i],&DA_Q_G[i]);			
		printf("\n AD_V_G[%d] is %d \n",i,AD_V_G[i]) ;
		
	}
	
	printf("-------------------------------------4-----------------------------------------\n");	
	//排序
	for(i = 0;i < 4;i++)
	{
		//4 3 2 1   最小数往低位传
		if(AD_V_G[i] <= AD_V_G[temp])	
		{
			//记录最小值序号
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
	uint16_t DA_Q;		//局部变量 扫描 Q路电压
	uint16_t DA_I;		//局部变量 扫描 I路电压
	uint16_t temp_i ; //备份传入的指针数据 使用局部变量运算
	uint16_t temp_q ;
	uint16_t AD_Value;	//局部变量 采样对消返回电压	
	uint16_t AD_Min = 4095;
	printf("=--------------------------5------------------------=");
	for(DA_I=124; DA_I<=1860; DA_I += 62)
	{
		for(DA_Q = 124;DA_Q <= 1860;DA_Q += 62) //先往加的方向
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
				AD_Min =  AD_Value;//更新最小值
				temp_i =  DA_I;    //记录最小值y坐标
				temp_q =  DA_Q;    //记录最小值x坐标
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

	//将慢速对消保留给快速对消参考
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

//5. 配置对消参数
void configcancellparam(uint16_t I_value,uint16_t Q_value)
{
	u8 tmp_write[4] = {0};
	u8 tmp_read[4] = {0};
	//组帧
	tmp_write[0] = (I_value>>8)&0xff;
	tmp_write[1] = (I_value)&0xff;	
	
	tmp_write[2] = (Q_value>>8)&0xff;
	tmp_write[3] = (Q_value)&0xff;	

	printf("config ...\n");	
	//if(flash_Dev_ok == flash_Isok)//flash 设备正常
	{
		//写入
		//Qp_DAValue高位
		EN25Q256_Write(&tmp_write[0],cancelldi_davalue_h,1);
		//Qp_DAValue低位
		EN25Q256_Write(&tmp_write[1],cancelldi_davalue_l,1);
		
		//Qn_DAValue高位
		EN25Q256_Write(&tmp_write[2],cancelldq_davalue_h,1);
		//Qn_DAValue低位
		EN25Q256_Write(&tmp_write[3],cancelldq_davalue_l,1);
			
		//读取4个连续内存的对消控制电压数据
		EN25Q256_Read(tmp_read,cancelldi_davalue_h,4);
		//比对
		if( ( tmp_write[0] == tmp_read[0] )&&( tmp_write[1] == tmp_read[1])
			&&( tmp_write[2] == tmp_read[2] )&&( tmp_write[3] == tmp_read[3]))
		{
			//更新ram 状态标志位和 flash 标志位
			u8 cancell_save_flag = flash_Isok;		
			EN25Q256_Write(&cancell_save_flag,cancellsave_flag,1);	
			
			//更新全局ram 数据
//			min_SquDAValue = tmp;
			//射频参数还需要配置出去
			DAC_SetChannel2Data(DAC_Align_12b_R, I_value);
			DAC_SetChannel1Data(DAC_Align_12b_R, Q_value);	
			//。。。。。。。。。。。。
		}else{
			printf("The cancell is not save\n");
			u8 cancell_save_flag = flash_Notok;		
			EN25Q256_Write(&cancell_save_flag,cancellsave_flag,1);				
		}
		
	}		
}

//6.查询对消的四路电压  需要先初始化DAC 输出
u8 Getcancellparam()
{
	u8 res = 0;
	//存储临时读取数据的临时变量
	unsigned short int flash_temp_h = 0;
	unsigned short int flash_temp_l = 0;	
	u8 flashMessage_temp = 0;
	printf("Read flash Getcancellparam....\r\n");
	EN25Q256_Read(&flashMessage_temp,cancellsave_flag,1);	
	if(flashMessage_temp == flash_Isok)//0x0A:正常
	{
		//flash存储正常 更新到全局变量
		//1.更新标志位
		//2.读取存储设备射频配置参数  射频功率
		flash_temp_h = flash_temp_l = 0;
		EN25Q256_Read((u8 *)&flash_temp_h,cancelldi_davalue_h,1);//Qp_DAValue高位
		EN25Q256_Read((u8 *)&flash_temp_l,cancelldi_davalue_l,1);//Qp_DAValue低位
		DA_I_G[0] = (flash_temp_h<<8)|flash_temp_l;

		flash_temp_h = flash_temp_l = 0;
		EN25Q256_Read((u8 *)&flash_temp_h,cancelldq_davalue_h,1);//Qn_DAValue高位
		EN25Q256_Read((u8 *)&flash_temp_l,cancelldq_davalue_l,1);//Qn_DAValue低位
		DA_Q_G[0] = (flash_temp_h<<8)|flash_temp_l;
					
		
		//射频参数还需要配置出去
		DAC_SetChannel2Data(DAC_Align_12b_R, DA_I_G[0]);
		DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q_G[0]);	
		res = 0;
	}else{
		//射频参数还需要配置出去
		DAC_SetChannel2Data(DAC_Align_12b_R, DA_I_G[0]);
		DAC_SetChannel1Data(DAC_Align_12b_R, DA_Q_G[0]);
		res = 1;
		printf("Getcancellparam is not ok\n");
	}
//	printf("min_SquDAValue.Squ_Value=%lld,Qp_DA=%d,Qn_DA=%d,Ip_DA=%d,In_DA=%d\n",
//			min_SquDAValue.Squ_Value,min_SquDAValue.Qp_DAValue,min_SquDAValue.Qn_DAValue,min_SquDAValue.Ip_DAValue,min_SquDAValue.In_DAValue);			
		
	return res;	
}
