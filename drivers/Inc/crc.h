/*
 * crc.h
 *
 *  Created on: 07-Jun-2026
 *      Author: vibhu
 */

#ifndef INC_CRC_H_
#define INC_CRC_H_

#include <stm32f407xx.h>

void crc_init();
void crc_reset();
void calculate_crc(uint32_t* accumulator , uint8_t word);
#endif /* INC_CRC_H_ */
