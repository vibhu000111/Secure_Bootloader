/*
 * flash_driver.h
 *
 *  Created on: 06-Jun-2026
 *      Author: vibhu
 */

#ifndef INC_FLASH_DRIVER_H_
#define INC_FLASH_DRIVER_H_

#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_SPI_driver.h"
#include "timebase.h"
#define DUMMY_BYTE 0xFF
#define JEDEC_ID_COMMAND 0x9F
#define UNIQUE_ID_COMMAND 0x4B
#define READ_STATUS_REGISTER 0x05
#define WRITE_ENABLE_COMMAND 0x06
#define SECTOR_ERASE_COMMAND 0x20
#define FLASH_READ_COMMAND 0x03
#define FLASH_WRITE_COMMAND 0x02
#define FLASH_ERASE_COMMAND 0Xc7

void SPI1_GPIOInits();
void flash_init();
uint8_t send_dummy_byte();
void Flash_read_jedec();
void Flash_read_unique_id();
uint8_t Flash_Read_status_register();
void Flash_Write_Enable();
void wait_rxne_flag_clear_garbage();
void wait_busy();
void sector_erase(uint32_t address) ;//address = sector base addres
void Flash_Read(uint32_t address, uint8_t* buffer , uint32_t length);
void Flash_Page_Program(uint32_t address,uint8_t *data,uint16_t length);
void Flash_ChipErase(void);



#endif /* INC_FLASH_DRIVER_H_ */
