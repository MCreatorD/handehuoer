#ifndef __UPDATE_MCU_FIRMWARE
#define __UPDATE_MCU_FIRMWARE
#include "stm32f4xx.h"

#include "EN25Q256.h"

#define update_length   1024

extern uint32_t FlashSource;

class Update_Mcu_Firmware{

public:
	void FLASH_Message(uint32_t Size,char *Name , uint8_t Flag);
	bool sendUpdateBlockToMcu(unsigned char *packet_data ,u32 WriteAddr,uint16_t pack_len);
};

#endif