/*
 * flash_driver.c
 *
 * W25Q16 SPI NOR Flash Driver
 *
 * Features:
 * - Read JEDEC ID
 * - Read Unique ID
 * - Read Status Register
 * - Write Enable
 * - Sector Erase
 * - Chip Erase
 * - Page Program
 * - Read Data
 *
 * This driver uses polling-based SPI transfers.
 * Intended for bootloader firmware update applications.
 */
#include "flash_driver.h"


uint8_t jedec_id[3]={0};
uint8_t unique_id[12]={0};
uint8_t status_register=0;
SPI_Handle_t SPI1_init;
GPIO_Handle_t pflash_cs;
/*
 * Configures SPI1 pins:
 *
 * PB3 -> SCLK
 * PB4 -> MISO
 * PB5 -> MOSI
 *
 * PB0 -> Flash Chip Select (CS)
 */
void SPI1_GPIOInits(void)
{
	GPIO_Handle_t SPIpins;

	SPIpins.pGPIOx = GPIOB;
	SPIpins.GPIO_pin.GPIO_PinMode = ALTFN;
	SPIpins.GPIO_pin.GPIO_PinAltFunMode = 5;
	SPIpins.GPIO_pin.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	SPIpins.GPIO_pin.GPIO_PinPUPD = GPIO_NO_PUPD;
	SPIpins.GPIO_pin.GPIO_PinSpeed = GPIO_SPEED_VERY_HIGH;

	//SCLK
	SPIpins.GPIO_pin.GPIO_PinNumber=3;
	GPIO_init(&SPIpins);

	//SPIpins.GPIO_pin.GPIO_PinNumber=14;
	//GPIO_init(&SPIpins);

	SPIpins.GPIO_pin.GPIO_PinNumber=4;
	GPIO_init(&SPIpins);
	SPIpins.GPIO_pin.GPIO_PinNumber=5;
	GPIO_init(&SPIpins);

	pflash_cs.pGPIOx=GPIOB;
	pflash_cs.GPIO_pin.GPIO_PinNumber=0;
	pflash_cs.GPIO_pin.GPIO_PinMode=OUTPUT;
	pflash_cs.GPIO_pin.GPIO_PinOPType=GPIO_OP_TYPE_PP;
	pflash_cs.GPIO_pin.GPIO_PinPUPD= GPIO_NO_PUPD;
	pflash_cs.GPIO_pin.GPIO_PinSpeed=GPIO_SPEED_HIGH;
	GPIO_init(&pflash_cs);
	GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
}
/*
 * Initializes SPI1 peripheral
 *
 * SPI Mode:
 * Master
 * Full Duplex
 * 8-bit transfers
 * CPOL = 0
 * CPHA = 0
 * speed 500Khz
 * W25Q16 supports SPI Mode 0.
 */
void flash_init()
{
	SPI1_init.pSPIx = SPI1;
	SPI1_init.SPIConfig.SPI_BusConfig = SPI_BUS_CONFIG_FD;
	SPI1_init.SPIConfig.SPI_DeviceMode = SPI_DEVICE_MODE_MASTER;
	SPI1_init.SPIConfig.SPI_SclkSpeed = SPI_SCLK_SPEED_DIV32;
	SPI1_init.SPIConfig.SPI_DFF = SPI_DFF_8BITS;
	SPI1_init.SPIConfig.SPI_CPHA = SPI_CPHA_LEADING_EDGE;
	SPI1_init.SPIConfig.SPI_CPOL = SPI_CPOL_LOW;
	SPI1_init.SPIConfig.SPI_SSM = SPI_SSM_EN;//SW slave mgmnt
	SPI_init(&(SPI1_init));
	SPI_EnDi(SPI1 , ENABLE);

}
/*
 * Sends one dummy byte (0xFF)
 * and returns the byte received
 * from the flash device.
 * waits if the RXNE flag is set if set then value is returned
 *
 */
uint8_t send_dummy_byte()
{
	uint8_t dummy_byte = DUMMY_BYTE;
	SPI_SendData(SPI1,&dummy_byte,1);
	while((SPI1->SPI_SR & 0x01) == 0)
	{

	}
	uint8_t value = 0;
	value = SPI1->SPI_DR;
	return(value);
}
/*
 * Reads JEDEC Identification
 *
 * Command:
 * 0x9F
 *
 * Response:
 * Manufacturer ID
 * Memory Type
 * Capacity
 *
 * Example:
 * EF 40 15
 */
void Flash_read_jedec()
{

    uint8_t jedec_id_command = JEDEC_ID_COMMAND;

    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
    SPI_SendData(SPI1,&jedec_id_command,1);
    while((SPI1->SPI_SR & 0x01) == 0)
    	{

    	}
    uint8_t garbage_byte = SPI1->SPI_DR;
    for(uint8_t i=0;i<3;i++)
    {
    	jedec_id [i] = send_dummy_byte();
    }
    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
}

void Flash_read_unique_id()
{
	uint8_t unique_id_command = UNIQUE_ID_COMMAND;

	    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
	    SPI_SendData(SPI1,&unique_id_command,1);
	    while((SPI1->SPI_SR & 0x01) == 0)
	    	{

	    	}
	    uint8_t garbage_byte = SPI1->SPI_DR;
	    for(uint8_t i=0;i<12;i++)
	    {
	    	unique_id[i] = send_dummy_byte();
	    }
	    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
}
/*
 * Reads Status Register-1
 *
 * Command:
 * 0x05
 *
 * Important bits of status register
 *
 * Bit0 = BUSY
 * Bit1 = WEL
 */
uint8_t Flash_Read_status_register()
{

    uint8_t read_status_register_command = READ_STATUS_REGISTER;
    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
    SPI_SendData(SPI1,&read_status_register_command,1);
    while((SPI1->SPI_SR & 0x01) == 0)
    {
    }
    uint8_t garbage_byte = SPI1->SPI_DR;
    	status_register = send_dummy_byte();

    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
    return(status_register);
}
/*
 * Enables flash write operations.
 *
 * Command:
 * 0x06
 *
 * WEL bit should become 1.
 *
 * Required before:
 * - Page Program
 * - Sector Erase
 * - Chip Erase
 */
void Flash_Write_Enable()
{

    uint8_t write_enable_command = WRITE_ENABLE_COMMAND;

    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
    SPI_SendData(SPI1,&write_enable_command,1);
   while((SPI1->SPI_SR & 0x01) == 0)
   	{
 	}
    uint8_t garbage_byte = SPI1->SPI_DR; //garbage byte RX buffer when WE command is sent
    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
    delayTicks(2);
    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
    uint8_t read_status_register_command = 0x05;
    SPI_SendData(SPI1,&read_status_register_command,1);
    while((SPI1->SPI_SR & 0x01) == 0)
        {
        }
        garbage_byte = SPI1->SPI_DR;
        	status_register = send_dummy_byte();
        	 GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);

}

void wait_rxne_flag_clear_garbage()
{
	 while((SPI1->SPI_SR & 0x01) == 0)
	 {

	 }
	 uint8_t garbage_byte = SPI1->SPI_DR;
}
/*
 * Polls BUSY bit in Status Register.
 *
 * Waits until:
 *
 * BUSY = 0
 *
 * indicating flash operation
 * has completed.
 */
void wait_busy()
{
	while(Flash_Read_status_register() & 0x01)
	{

	}
}
/*
 * Erases one 4KB sector.
 *
 * Command:
 * 0x20
 *
 * Parameter:
 * address = sector base address
 *
 * Example:
 * 0x000000
 * 0x001000
 * 0x002000
 */
void sector_erase(uint32_t address)  //address = sector base address
{
	uint8_t sector_erase_command = SECTOR_ERASE_COMMAND;

	    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
	    SPI_SendData(SPI1,&sector_erase_command,1);
	    wait_rxne_flag_clear_garbage();

	    for(int8_t shift = 16; shift >= 0; shift -= 8)
	    {
	    	uint8_t byte = (address >> shift) & 0xFF;
	    	SPI_SendData(SPI1,&byte,1);
	    	wait_rxne_flag_clear_garbage();
	    }
	    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
	    status_register = Flash_Read_status_register();
	    wait_busy();
	    status_register = Flash_Read_status_register();

}
/*
 * Reads flash contents.
 *
 * Command:
 * 0x03
 *
 * Parameters:
 * address = start address
 * buffer  = destination buffer
 * length  = bytes to read
 */
void Flash_Read(uint32_t address, uint8_t* buffer , uint32_t length)
{
	uint8_t flash_read_command = FLASH_READ_COMMAND;

		    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
		    SPI_SendData(SPI1,&flash_read_command,1);
		    wait_rxne_flag_clear_garbage();

		    for(int8_t shift = 16; shift >= 0; shift -= 8)
		    {
		    	uint8_t byte = (address >> shift) & 0xFF;
		    	SPI_SendData(SPI1,&byte,1);
		    	wait_rxne_flag_clear_garbage();
		    }
		    for(int i=0;i<length;i++)
		    {
		    	buffer[i] = send_dummy_byte();
		    }
		    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
}
/*
 * Programs up to one flash page.
 *
 * Command:
 * 0x02
 *
 * Parameters:
 * address = destination address
 * data    = source buffer
 * length  = bytes to program
 *
 * Note:
 * Length should not exceed
 * page size (256 bytes).
 */
void Flash_Page_Program(uint32_t address,uint8_t *data,uint16_t length)
{
	Flash_Write_Enable();
	uint8_t status_wel = Flash_Read_status_register();
	if(status_wel == 2) //write enabled
	{
		uint8_t flash_write_command = FLASH_WRITE_COMMAND;

	    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
		SPI_SendData(SPI1,&flash_write_command,1);
		wait_rxne_flag_clear_garbage();

		for(int8_t shift = 16; shift >= 0; shift -= 8)
		{
		 uint8_t byte = (address >> shift) & 0xFF;
		 SPI_SendData(SPI1,&byte,1);
		 wait_rxne_flag_clear_garbage();
	     }
		 for(uint8_t i=0;i<length;i++)
		  {
		  //buffer[i] = send_dummy_byte();
		 SPI_SendData(SPI1,data,1);
		 wait_rxne_flag_clear_garbage();
		 data++;
		  }
		  GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
		  wait_busy();
	}
}
/*
 * Erases entire flash memory.
 *
 * Command:
 * 0xC7
 *
 * Warning:
 * Operation may take several
 * seconds to complete.
 */
void Flash_ChipErase()
{
	Flash_Write_Enable();
	uint8_t status_wel = Flash_Read_status_register();
	if(status_wel == 2) //write enabled
		{
		uint8_t flash_erase_command = FLASH_ERASE_COMMAND;

			    GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 0);
				SPI_SendData(SPI1,&flash_erase_command,1);
				wait_rxne_flag_clear_garbage();
				GPIO_WriteOutputPin(pflash_cs.pGPIOx, pflash_cs.GPIO_pin.GPIO_PinNumber , 1);
				wait_busy();
		}



}
