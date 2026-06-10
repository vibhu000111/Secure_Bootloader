/*
 * crc.c
 *
 *  Created on: 07-Jun-2026
 *      Author: vibhu
 */

#include <crc.h>
CRC_RegDef_t* crc = CRC;

void crc_init()
{
	CRC_PCLK_EN() ;
	crc_reset();
}
void crc_reset()
{
	crc->CRC_CR|=(1<<0); // setting the reset bit in CRC CR register
}
void calculate_crc(uint32_t* accumulator , uint8_t word)
{
		crc->CRC_DR = word; // putting 32bit word in it
		*accumulator =  crc->CRC_DR; //reading out computed CRC
}
