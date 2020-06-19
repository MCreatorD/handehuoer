#include "usart.h"
#include "config.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/*设置是否使用硬件流控制*/
//#define HwFlowControl

/*******************************************************************************
* Function Name  : USART_Configuration
* Description    : Configure Open205R_USARTx 
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void USART_Configuration(void)
{ 												
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure; 

  RCC_AHB1PeriphClockCmd(Open205R_USARTx_TX_GPIO_CLK,ENABLE);
  RCC_AHB1PeriphClockCmd(Open205R_USARTx_RX_GPIO_CLK,ENABLE);

  #if defined OpenUSART1 || defined OpenUSART6
  	RCC_APB2PeriphClockCmd(Open205R_USARTx_CLK,ENABLE);
  #else
	RCC_APB1PeriphClockCmd(Open205R_USARTx_CLK,ENABLE);
  #endif

  GPIO_PinAFConfig(Open205R_USARTx_TX_GPIO_PORT, Open205R_USARTx_TX_SOURCE, Open205R_USARTx_TX_AF);
  GPIO_PinAFConfig(Open205R_USARTx_RX_GPIO_PORT, Open205R_USARTx_RX_SOURCE, Open205R_USARTx_RX_AF);

  /*
  *  Open205R_USARTx_TX -> PA9 , Open205R_USARTx_RX -PA10
  */
  GPIO_InitStructure.GPIO_Pin = Open205R_USARTx_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(Open205R_USARTx_TX_GPIO_PORT, &GPIO_InitStructure);


  GPIO_InitStructure.GPIO_Pin = Open205R_USARTx_RX_PIN;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_Init(Open205R_USARTx_RX_GPIO_PORT, &GPIO_InitStructure);

  #if defined HwFlowControl && !defined Open205RUART4 && !defined Open205RUART5 
  	RCC_AHB1PeriphClockCmd(Open205R_USARTx_RTS_GPIO_CLK,ENABLE);
	RCC_AHB1PeriphClockCmd(Open205R_USARTx_CTS_GPIO_CLK,ENABLE);
	
	GPIO_PinAFConfig(Open205R_USARTx_RTS_GPIO_PORT, Open205R_USARTx_RTS_SOURCE, Open205R_USARTx_RTS_AF);
	GPIO_PinAFConfig(Open205R_USARTx_CTS_GPIO_PORT, Open205R_USARTx_CTS_SOURCE, Open205R_USARTx_CTS_AF);

	GPIO_InitStructure.GPIO_Pin = Open205R_USARTx_RTS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(Open205R_USARTx_RTS_GPIO_PORT, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = Open205R_USARTx_CTS_PIN;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(Open205R_USARTx_CTS_GPIO_PORT, &GPIO_InitStructure);
  #endif
/*
   		 USARTx configured as follow:
         - BaudRate = 115200 baud  
		 - Word Length = 8 Bits
         - One Stop Bit
         - No parity
         - Hardware flow control disabled (RTS and CTS signals)
         - Receive and transmit    
 */

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  #if defined HwFlowControl && !defined Open205RUART4 && !defined Open205RUART5
  	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_RTS_CTS;
  #else
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  #endif
	
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(Open205R_USARTx, &USART_InitStructure);
  /* Enable the Open205R_USARTx Transmit interrupt: this interrupt is generated when the 
     Open205R_USARTx transmit data register is empty */
  USART_ITConfig(Open205R_USARTx,USART_IT_RXNE,ENABLE);

  USART_Cmd(Open205R_USARTx, ENABLE);
  
  USART_NVIC_Config();

}

void USART_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the USARTx Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = Open205R_USARTx_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

/* Use no semihosting */
/*#if 1
#pragma import(__use_no_semihosting) 
struct __FILE
{  
	int handle;
};
FILE __stdout;
//FILE __stdin;

_sys_exit(int x)
{
	x = x;
}

_ttywrch(int ch)
{
ch = ch;
}

#endif
*/

/*PUTCHAR_PROTOTYPE
{

  USART_SendData(Open205R_USARTx, (uint8_t) ch);


  while (USART_GetFlagStatus(Open205R_USARTx, USART_FLAG_TC) == RESET)
  {}

  return ch;
}*/

int sendchar(int ch)
{
    /*while(!(USART3->SR & USART_FLAG_TXE));
    USART3->DR = (ch & 0x1FF);

//    return ch;*/
//	 /* 清SR寄存器中的TC标志 */
//		USART_ClearFlag(Open205R_USARTx,USART_FLAG_TC);
	
	  USART_SendData(Open205R_USARTx, (uint8_t) ch);


  while (USART_GetFlagStatus(Open205R_USARTx, USART_FLAG_TC) == RESET)
  {}

  return ch;
}

int getkey(void)
{
    /*while(!(USART3->SR & USART_FLAG_RXNE));
    return ((int)(USART3->DR & 0X1FF));*/
	return 0;
}
