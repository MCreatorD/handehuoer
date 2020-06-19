#include "lowpower.h"
#include "fpga_pwr_en.h"
#include "cpu_5v_en.h"
#include <stdio.h>
#include "Bsp_init.h"
//待机模式和PA0唤醒

void init_lowpower()
{
	//开启电源管理的单元的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	
	/*清除WU状态位*/
	PWR_ClearFlag (PWR_FLAG_WU);
	/* 使能WKUP引脚的唤醒功能 ，使能PA0  WKUP引脚上升沿*/
	//PWR_WakeUpPinCmd (ENABLE);	
}

void lowpower()//进入待机模式
{
	/* 进入待机模式 WKUP引脚上升沿唤醒*/
	//关闭其他器件的电源
	//1.关闭FPGA电
	FPGA_PWR_EN_L;//相关射频电也就断了
	CPU_5V_EN_L;
	init_lowpower();
	PWR_EnterSTANDBYMode();	
}

void PWR_STOP()//停止模式
{
	//关闭其他器件的电源
	//1.关闭FPGA电
	FPGA_PWR_EN_L;//相关射频电也就断了
	CPU_5V_EN_L;
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);  //停机模式
	//SystemInit();//退出低功耗后的时钟重新配
}

void PWR_SLEEP()//睡眠模式   CM3 内核停止，外设仍然运行
{
	//汉德霍尔设置睡眠模式 串口中断唤醒
	//PWR_Regulator_ON  电源不进低功耗 唤醒基本没延迟
	//PWR_STOPEntry_WFI 中断唤醒
	printf("进入睡眠模式\n");
	//PWR_EnterUnderDriveSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
	//进入睡眠模式
	__WFI();	//WFI指令进入睡眠,任意中断唤醒
	//printf("退出睡眠模式\n");
	//PWR_EnterSleepMode();
	//SystemInit();//退出低功耗后的时钟重新配
}

//软件复位
void SoftReset(void)
{
	//设置软件重启和安装电池重启的区别
	//set_reset_config(flash_Isok);
	__set_FAULTMASK(1); // 关闭所有中断
	NVIC_SystemReset(); // 复位
}

            

            
