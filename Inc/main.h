/*
 * main.h
 *
 *  Created on: 06-Jun-2026
 *      Author: vibhu
 */

#ifndef MAIN_H_
#define MAIN_H_

#define BL_ACK   0xA5
#define BL_NACK  0x7f
// boot version
#define BL_VER 0x51
//suppoerted cmd
#define BL_HELP 0X52
//CHIP ID
#define BL_CID 0x53
//read protection level
#define BL_RDP 0x54
// jump BL to specified address
#define BL_GO_TO_ADDR 0x55
//mass erase or secotr erase user flash
#define BL_FLASH_ERASE 0x56
//WRITE DATA into diff memories of MCU
#define BL_MEM_WRITE 0x57
// enable disable read/write protect on different sectors of MCU flash
#define BL_ENDIS_RW_PROTECT 0x58
//read data from diff memories of MCU
#define BL_MEM_READ 0x59
//read asll sector protection status
#define BL_READ_SECTOR_STATUS 0x5A
//read OTP content
#define BL_OTP_READ 0x5B



#endif /* MAIN_H_ */
