#ifndef _I2C_H_
#define _I2C_H_

#include "stm32f4xx.h"

	#ifdef __cplusplus
	 extern "C" {
	#endif

	void I2C1_Configuration(void);
	void I2C2_Configuration(void);	 
	uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);
	uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t *buf,uint16_t num);

	uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint8_t addr,uint8_t value);		 
	#ifdef __cplusplus
	}
	#endif	

#endif
