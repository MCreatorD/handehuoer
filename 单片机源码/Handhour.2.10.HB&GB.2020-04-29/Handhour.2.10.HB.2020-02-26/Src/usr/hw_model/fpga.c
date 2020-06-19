#include "fpga.h"
#include "fpga_pwr_en.h"
#include "cpu_5v_en.h"
//unsigned char lowpower_change = 0;

void FPGA_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
	
	/* Enable GPIO clock */
	
	FPGA_SIGNGPIO_ENABLE();
	
	FPGA_POWER_ENABLE();
	FPGA_STATE_ENABLE();
	FPGA_CE_ENABLE();
	FPGA_CONF_ENABLE();
	FPGA_CONFDONE_ENABLE();
	
 
	/* Config GPIO Mode */	
  GPIO_InitStruct.GPIO_Pin    =  FPGA_START_OUT_IO;
  GPIO_InitStruct.GPIO_Mode   =  GPIO_Mode_OUT;
  GPIO_InitStruct.GPIO_OType  =  GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd   =  GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed  =  GPIO_Speed_100MHz;
	GPIO_Init(FPGA_START_Port, &GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin    = FPGA_RWOVER_IN_IO;
  GPIO_InitStruct.GPIO_Mode   = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_RWOVER_Port, &GPIO_InitStruct);	
	
	GPIO_InitStruct.GPIO_Pin       = FPGA_NEEDCOMM_IN_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	//GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_NEEDCOMM_Port, &GPIO_InitStruct);	

	GPIO_InitStruct.GPIO_Pin       = FPGA_SINGLEEND_OUT_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_SINGLEEND_Port, &GPIO_InitStruct);	
  
	GPIO_InitStruct.GPIO_Pin       = FPGA_TURNEND_IN_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_TURNEND_Port, &GPIO_InitStruct);	
	
	//////////////////////////////////////////////////////////////
	GPIO_InitStruct.GPIO_Pin       = FPGA_POWER_OUT_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_POWER_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin       = FPGA_STATE_IN_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_STATE_Port, &GPIO_InitStruct);	

	GPIO_InitStruct.GPIO_Pin       = FPGA_CE_OUT_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_CE_Port, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin       = FPGA_CONFDONE_IN_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;//GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_CONFDONE_Port, &GPIO_InitStruct);	

	GPIO_InitStruct.GPIO_Pin       = FPGA_CONF_OUT_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_CONF_Port, &GPIO_InitStruct);
	
	
//低功耗
	GPIO_InitStruct.GPIO_Pin       = FPGA_LOWPOWER_IO;
  GPIO_InitStruct.GPIO_Mode      = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType     = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd      = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed     = GPIO_Speed_100MHz;
	GPIO_Init(FPGA_LOWPOWER_Port, &GPIO_InitStruct);

	
	__nop();	

}

void FPGA_Init(void)
{
	FPGA_GPIO_Init();
	FPGA_PWR_EN_Init();
	
	FPGA_LowPower(1); //先配置是否低功耗
	FPGA_PWR_EN_H;   //配置FPGA供电 上电检测到低功耗进入
}

void FPGA_LowPower(int isLow){
	if(isLow!=0){
		FPGA_LOWPOWER_L; //低功耗
		CPU_5V_EN_L;
		
		//FPGA_LOWPOWER_H; //低功耗
		//CPU_5V_EN_H;		
	}else{

		CPU_5V_EN_H;
		FPGA_LOWPOWER_H; 
		//lowpower_change = 1;//退出低功耗记录及记录开始读卡
	}
}