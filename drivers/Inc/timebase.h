/*
 * timebase.h
 *
 *  Created on: Apr 11, 2026
 *      Author: vibhu
 */

#ifndef INC_TIMEBASE_H_
#define INC_TIMEBASE_H_
#include <stm32f407xx.h>
#define TICK_HZ 1U
#define HSI_CLK 16000000U
#define SYSTICK_TIM_CLK HSI_CLK

void init_systick_timer(uint32_t tick_hz);
void init_systick_timer(uint32_t tick_hz);
uint32_t getTicks(void);
void delayTicks(uint32_t delay_ms);


#endif /* INC_TIMEBASE_H_ */
