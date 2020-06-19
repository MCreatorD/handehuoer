#include "fpga_epcs.h"
#include "sim_spi1.h"

void FPGA_EPCS_Write(void)
{
	
	GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_AHB1PeriphClockCmd(EPCS_CLK_1, ENABLE);
	RCC_AHB1PeriphClockCmd(EPCS_CLK_2, ENABLE);
	RCC_AHB1PeriphClockCmd(EPCS_CLK_3, ENABLE);	
	
	
	GPIO_InitStructure.GPIO_Pin = EPCS_SPI_MO_PIN | EPCS_NCS_PIN | EPCS_SPI_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(EPCS_PORT_1, &GPIO_InitStructure);//EPCS_SPI_MO EPCS_NCS EPCS_SPI_CLK
	
	GPIO_InitStructure.GPIO_Pin = EPCS_SPI_MI_PIN; //EPCS_SPI_MI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(EPCS_PORT_2, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FPGA_nCE_PIN|FPGA_nCONFIG_PIN; //FPGA_nCE,FPGA_nCONFIG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(EPCS_PORT_2, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FPGA_CONF_DONE_PIN; //FPGA_CONF_DONE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(EPCS_PORT_3, &GPIO_InitStructure);	
}


void FPGA_EPCS(void)
{
	//以下将与EPCS相连的几个脚全部置为输入状态，以使得FPGA可以正常启动
  GPIO_InitTypeDef  GPIO_InitStructure;	
	RCC_AHB1PeriphClockCmd(EPCS_CLK_1, ENABLE);
	RCC_AHB1PeriphClockCmd(EPCS_CLK_2, ENABLE);
	RCC_AHB1PeriphClockCmd(EPCS_CLK_3, ENABLE);	
	
	
	GPIO_InitStructure.GPIO_Pin = EPCS_SPI_MO_PIN | EPCS_NCS_PIN | EPCS_SPI_CLK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(EPCS_PORT_1, &GPIO_InitStructure);//EPCS_SPI_MO EPCS_NCS EPCS_SPI_CLK
	
	GPIO_InitStructure.GPIO_Pin = EPCS_SPI_MI_PIN; //EPCS_SPI_MI
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(EPCS_PORT_2, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = FPGA_nCONFIG_PIN; //FPGA_nCE,FPGA_nCONFIG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(EPCS_PORT_2, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = FPGA_nCE_PIN; //FPGA_nCE,FPGA_nCONFIG
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Init(EPCS_PORT_2, &GPIO_InitStructure);
		////修改 2018/5/10 FPGA下载测试	
	GPIO_ResetBits(GPIOC , GPIO_Pin_9); //FPGA片选设为0
	GPIO_SetBits(GPIOC , GPIO_Pin_8);   //FPGA_nCONFIG高为1
	
	GPIO_InitStructure.GPIO_Pin = FPGA_CONF_DONE_PIN; //FPGA_CONF_DONE
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(EPCS_PORT_3, &GPIO_InitStructure);
	
}



