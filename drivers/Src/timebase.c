/*
 * timebase.c
 *
 *  Created on: Apr 11, 2026
 *      Author: vibhu
 */
#include <timebase.h>

uint32_t gTicks=0;

void init_systick_timer(uint32_t tick_hz)
{
	uint32_t* SYST_RVR = (uint32_t*)0xE000E014;
		uint32_t count = (SYSTICK_TIM_CLK / tick_hz)-1;

		//clear value of rvr // 31:24 is reserved 23:0 is where data will be stored

		*SYST_RVR&= ~(0x00ffffff); //clears the 23:0
		*SYST_RVR|=count;

		//configure systick and control register refer manual
		/*
		 * [2] CLKSOURCE Indicates the clock source:
	0 = external clock
	1 = processor clock.
	[1] TICKINT Enables SysTick exception request:
	0 = counting down to zero does not assert the SysTick exception request
	1 = counting down to zero asserts the SysTick exception request.
	Software can use COUNTFLAG to determine if SysTick has ever counted to zero.
	[0] ENABLE Enables the counter:
	0 = counter disabled
	1 = counter enabled
		 */

		uint32_t* SYST_csr =(uint32_t*) 0xE000E010;
		*SYST_csr|=(7 << 0); //clears the last 3 bits

}
void SysTick_Handler()
{
	gTicks++;
}
uint32_t getTicks()
{
	return(gTicks);
}
void delayTicks(uint32_t delay_ms)
{
    uint32_t CurrTick = getTicks();

    while ((getTicks() - CurrTick) < delay_ms)
    {
        /* Wait */
    }
}

