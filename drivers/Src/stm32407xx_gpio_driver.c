/*
 * stm32407xx_gpio_driver.c
 *
 *  Created on: 23-Nov-2024
 *      Author: vibhu
 */
// FOR EVERY FUNCTION THERE SHOULD E THE DOCUMENTATION OF THE API
//IT SHOULD EXPLAIN THE PARAMETERS OF THE FUNCTION AND THE RETURN TYPES
#include <stm32f407xx_gpio_driver.h> //the driver source file of the peripheral should contain the driver header file





/*
 * Function name  - GPIO_init
 * Description of function -
 * parameter -
 * return -
 *
 *
 */

void GPIO_init(GPIO_Handle_t *pGPIOHandle) //user application should create a variable of GPIO_Handle_t type initialize it and send the pointer
//to GPIO_intit function to initialize the GPIO port and Pin
{
	GPIO_Peripehral_clk_control(pGPIOHandle->pGPIOx,ENABLE);
	GPIO_RegDef_t* GPIO_temp = pGPIOHandle->pGPIOx; // initializing the a temporary pointer to structure variable that holds the base address of the GPIO Port

	/*
	 * 00: Input (reset state)
       01: General purpose output mode
       10: Alternate function mode
       11: Analog mode
       Assign Macro in the driver header file for this
	 */

	// configure MODER register
	uint8_t MODE = pGPIOHandle->GPIO_pin.GPIO_PinMode;
	GPIO_temp->MODER&=~(3<<((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2)); // clears the MODER bit to be set before setting the value
	if(MODE<=3) //check the  code more optimized
	{
	if( MODE == INPUT)
	{
		GPIO_temp->MODER&=~(3<<((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2));
	}
	else if (MODE == OUTPUT)
	{
		GPIO_temp->MODER|=(1<<((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2));
	}
	else if(MODE == ALTFN)
	{
		GPIO_temp->MODER|=(2<<((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2));
	}
	else if(MODE == ANALOG)
	{
		GPIO_temp->MODER|=(3<<((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2));
	}
	}
	else
	{
		GPIO_temp->MODER&=~(3<<((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2)); // pin should be in input mode
		//interrupt mode
		if(pGPIOHandle->GPIO_pin.GPIO_PinMode == GPIO_MODE_FT)
		{
			EXTI->EXTI_FTSR|=(1 << pGPIOHandle->GPIO_pin.GPIO_PinNumber); // configuring edge detector in EXTI to generate interrupt at falling edge
			EXTI->EXTI_RTSR&=~(1<< pGPIOHandle->GPIO_pin.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_pin.GPIO_PinMode == GPIO_MODE_RT)
		{
			EXTI->EXTI_RTSR|=(1<< pGPIOHandle->GPIO_pin.GPIO_PinNumber);
			EXTI->EXTI_FTSR&=~(1<< pGPIOHandle->GPIO_pin.GPIO_PinNumber);
		}
		else if(pGPIOHandle->GPIO_pin.GPIO_PinMode == GPIO_MODE_RTFT)
		{
			EXTI->EXTI_FTSR|=(1 << pGPIOHandle->GPIO_pin.GPIO_PinNumber);
			EXTI->EXTI_RTSR|=(1<< pGPIOHandle->GPIO_pin.GPIO_PinNumber);
		}
		uint32_t k;
		//Configure SYSCFG register
		// have a look at the register lay out
		k = GPIO_baseaddr_to_code(pGPIOHandle->pGPIOx);
		/*
		if( k== GPIOA)
			k=PA;
		else if(pGPIOHandle->pGPIOx == GPIOB)
			k=PB;
		else if(pGPIOHandle->pGPIOx == GPIOC)
			k=PC;
		else if(pGPIOHandle->pGPIOx == GPIOD)
			k=PD;
		else if(pGPIOHandle->pGPIOx == GPIOE)
			k=PE;
		else if(pGPIOHandle->pGPIOx == GPIOF)
			k=PF;
		else if(pGPIOHandle->pGPIOx == GPIOG)
			k=PG;
		else if(pGPIOHandle->pGPIOx == GPIOH)
			k=PH;
		else if(pGPIOHandle->pGPIOx == GPIOI)
			k=PI;
			*/
		SYSCFG_PCLK_EN();
		SYSCFG->SYSCFG_EXTICR[(pGPIOHandle->GPIO_pin.GPIO_PinNumber)/4]|=(k << (pGPIOHandle->GPIO_pin.GPIO_PinNumber)*4);
		// Unmask the interrupt to NVIC engine by unmasking bit in the IMR register
		EXTI->EXTI_IMR|=(1<<pGPIOHandle->GPIO_pin.GPIO_PinNumber);

	}

	//configure speed
	GPIO_temp->OSPEEDR&=~(3 << ((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2)); //clearing the bits before setting
	GPIO_temp->OSPEEDR|=(pGPIOHandle->GPIO_pin.GPIO_PinSpeed << ((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2));
	//configure PUPD
	GPIO_temp->PUPDR&=~(3 << ((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2));// clearing the bits before setting
	GPIO_temp->PUPDR|=pGPIOHandle->GPIO_pin.GPIO_PinPUPD << ((pGPIOHandle->GPIO_pin.GPIO_PinNumber)*2);
	//configure opttype
	GPIO_temp->OTYPER&=~( 1 <<((pGPIOHandle->GPIO_pin.GPIO_PinNumber))); // clear the OUTPUTTYPE BIT for the specific pin before setting it
	GPIO_temp->OTYPER|=pGPIOHandle->GPIO_pin.GPIO_PinOPType <<((pGPIOHandle->GPIO_pin.GPIO_PinNumber));

	/*
	 * 0: Output push-pull (reset state)
       1: Output open-drain

       For INPUT MODE and ANALOG MODE no need to configure
	 */

	/*
	 * Alternate functionality
	 */
	/*Alternate functionality needs to be configures if in the output mode Altfn is selected
	 * There are two AFR registers AFRH and AFRL
	 * now the AFRL register for a port is used to map alternate function to the pin from pin 0 to pin 7
	 * AFRH is used to map alternate function to the pin from pin 8 to pin 15
	 */
	if(MODE == ALTFN)
	{
		GPIO_RegDef_t* temp = pGPIOHandle->pGPIOx;

		uint8_t reg_h_l = (pGPIOHandle->GPIO_pin.GPIO_PinNumber)/8; // if quotient is 0 means AFRL if 1 AFRH
		uint8_t reg_bit = ((pGPIOHandle->GPIO_pin.GPIO_PinNumber)%8)*4; // check AFRH and AFRL register definition each pin is taking 4 bits

		if(reg_h_l == 1) //select the AFRH register
		{
			temp->AFRH&=~(15 << reg_bit);
			temp->AFRH|=(pGPIOHandle->GPIO_pin.GPIO_PinAltFunMode << reg_bit);
		}
		else
		{
			temp->AFRL&=~(15 << reg_bit);
			temp->AFRL|=(pGPIOHandle->GPIO_pin.GPIO_PinAltFunMode << reg_bit);
		}

	}



}
void GPIO_deinit(GPIO_RegDef_t* pGPIOx)
//used to deinitialize the registers of the given GPIO peripheral meaning put all the register specific to that GPIO back to its reset state
//can be done in one shot by using  a register in the RCC peripheral using the RCC peripheral reset register refer the reference manual
{
	if(pGPIOx == GPIOA)
		{
			RCC->RCC_AHB1RSTR|=(1<<0);
			RCC->RCC_AHB1RSTR&=~(1<<0);
		}
		else if(pGPIOx == GPIOB)
		{
			RCC->RCC_AHB1RSTR|=(1<<1);
			RCC->RCC_AHB1RSTR&=~(1<<1);
		}
		else if(pGPIOx == GPIOC)
		{
			RCC->RCC_AHB1RSTR|=(1<<2);
			RCC->RCC_AHB1RSTR&=~(1<<2);
		}
		else if(pGPIOx == GPIOD)
		{
			RCC->RCC_AHB1RSTR|=(1<<3);
			RCC->RCC_AHB1RSTR&=~(1<<3);
		}
		else if(pGPIOx == GPIOE)
		{
			RCC->RCC_AHB1RSTR|=(1<<4);
			RCC->RCC_AHB1RSTR&=~(1<<4);
		}
		else if(pGPIOx == GPIOF)
		{
			RCC->RCC_AHB1RSTR|=(1<<5);
			RCC->RCC_AHB1RSTR&=~(1<<5);
		}
		else if(pGPIOx == GPIOG)
		{
			RCC->RCC_AHB1RSTR|=(1<<6);
			RCC->RCC_AHB1RSTR&=~(1<<6);
		}
		else if(pGPIOx == GPIOH)
		{
			RCC->RCC_AHB1RSTR|=(1<<7);
			RCC->RCC_AHB1RSTR&=~(1<<7);
		}
		else if(pGPIOx == GPIOI)
		{
			RCC->RCC_AHB1RSTR|=(1<<8);
			RCC->RCC_AHB1RSTR&=~(1<<8);
		}
}

void GPIO_Peripehral_clk_control(GPIO_RegDef_t *pGPIOx,uint8_t EnDi) //to enable or disable the clock I need to know the base adddress of the
//GPIO which can then be mapped to which bit to set or reset in the RCC AHB1 ENR register
{
	if(EnDi == ENABLE)
	{
		if(pGPIOx == GPIOA)
		{
			GPIOA_PCLK_EN();
		}
		else if(pGPIOx == GPIOB)
		{
			GPIOB_PCLK_EN();
		}
		else if(pGPIOx == GPIOC)
		{
			GPIOC_PCLK_EN();
		}
		else if(pGPIOx == GPIOD)
		{
			GPIOD_PCLK_EN();
		}
		else if(pGPIOx == GPIOE)
		{
			GPIOE_PCLK_EN();
		}
		else if(pGPIOx == GPIOF)
		{
			GPIOF_PCLK_EN();
		}
		else if(pGPIOx == GPIOG)
		{
			GPIOG_PCLK_EN();
		}
		else if(pGPIOx == GPIOH)
		{
			GPIOH_PCLK_EN();
		}
		else if(pGPIOx == GPIOI)
		{
			GPIOI_PCLK_EN();
		}
	}
	else
	{
		if(pGPIOx == GPIOA)
				{
					GPIOA_PCLK_DI();
				}
				else if(pGPIOx == GPIOB)
				{
					GPIOB_PCLK_DI();
				}
				else if(pGPIOx == GPIOC)
				{
					GPIOC_PCLK_DI();
				}
				else if(pGPIOx == GPIOD)
				{
					GPIOD_PCLK_DI();
				}
				else if(pGPIOx == GPIOE)
				{
					GPIOE_PCLK_DI();
				}
				else if(pGPIOx == GPIOF)
				{
					GPIOF_PCLK_DI();
				}
				else if(pGPIOx == GPIOG)
				{
					GPIOG_PCLK_DI();
				}
				else if(pGPIOx == GPIOH)
				{
					GPIOH_PCLK_DI();
				}

	}
}

uint8_t GPIO_ReadInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber) //to read the IDR register this function need the base address of the GPIO port and the pin number to specify which bit to read from IDR
{
	uint32_t value;
	value = pGPIOx->IDR;
	//as the value now holds the value in IDR and now right shift it by pin number and do bitwise and
	uint8_t final = (value>>PinNumber)&0x00000001; //this holds the value of the pin state in the final variable
	return(final);

}
uint16_t GPIO_ReadInputPort(GPIO_RegDef_t *pGPIOx)
{
 uint16_t value = pGPIOx->IDR;
 //here the pGPIOx->IDR fetches 32 bit value stored at an offset of IDR location from the base address and this value is then truncated when being
 //stored in 16 bit variable and the lower 2 bytes that is 15:0 is stored in variable
 //as arm is little endian meaning least significant byte is stored at lowest address

  return(value);
}
void GPIO_WriteOutputPort(GPIO_RegDef_t *pGPIOx,uint16_t value)
{
     pGPIOx->ODR|=value;
}
void GPIO_WriteOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber , uint8_t value)
{
	if (value == 0)
	{
		pGPIOx->ODR&=~(1<<PinNumber);
	}
	else
	{
		pGPIOx->ODR|=(1<<PinNumber);
	}

}
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx ,uint8_t PinNumber)
{
	/*
	 * XOR Truth Table:

| A   | B   | A XOR B |
| --- | --- | ------- |
|  0  |  0  |    0    |
|  0  |  1  |    1    |
|  1  |  0  |    1    |
|  1  |  1  |    0    |
	 */
    pGPIOx->ODR^=(1<<PinNumber); // when xored with 1 the bit toggles if it is xored with a bit with value 0 then it remains the same
}
void GPIO_altfn(void)
{

}

uint32_t GPIO_baseaddr_to_code(GPIO_RegDef_t* m)
{
	uint32_t k;

	if( m== GPIOA)
				k=PA;
			else if(m == GPIOB)
				k=PB;
			else if(m == GPIOC)
				k=PC;
			else if(m == GPIOD)
				k=PD;
			else if(m == GPIOE)
				k=PE;
			else if(m == GPIOF)
				k=PF;
			else if(m == GPIOG)
				k=PG;
			else if(m == GPIOH)
				k=PH;
			else if(m == GPIOI)
				k=PI;
	return(k);
}
//IRQ_configuration = configuring NVIC registers like IPR for priority , ISER for enabling interrupts from specific IRQ and IRQHandling for handling the ISR
// this API is processor specific cortex M4 setup the NVIC registers for the processor
void GPIO_IRQConfig(uint8_t IRQNumber,uint8_t EnDi)
{
	// identify IRQ number based on the pin number for PA0 it will be EXTI0 identify on which IRQ line it issues interrupt
	//first set the priority of the interrupt then enable the interrupt

	volatile uint32_t* ISER = (uint32_t*) NVIC_ISER_BASEADDR;

	volatile uint32_t* ICER = (uint32_t*)NVIC_ICER_BASEADDR;

	uint32_t ISER_reg_offset;
	uint32_t ISER_reg_bit_pos;

	uint32_t ICER_reg_offset;
	uint32_t ICER_reg_bit_pos;

	ISER_reg_offset = IRQNumber/32;
	ISER_reg_bit_pos = IRQNumber%32;

	ICER_reg_offset = ISER_reg_offset;
	ICER_reg_bit_pos = ISER_reg_bit_pos;

	if (EnDi == ENABLE)
	{
		ISER = ISER + ISER_reg_offset;
	    *ISER|=(1<<ISER_reg_bit_pos); // enable the interrupt
	}
	else if (EnDi == DISABLE)
	{
			ICER = ICER + ICER_reg_offset;
			*ICER|=(1 << ICER_reg_bit_pos);

	}


}
void GPIO_IRQ_PriorityConfig(uint8_t IRQNumber , uint8_t IRQPriority)
{
	volatile uint32_t* IPR = (uint32_t*)NVIC_IPR_BASEADDR;
	uint32_t IPR_reg_offset;
	uint32_t IPR_reg_bit_pos;
	IPR_reg_offset = IRQNumber/4;
	IPR_reg_bit_pos = IRQNumber%4;
	IPR = IPR + IPR_reg_offset;
	uint8_t* irq_byte = (uint8_t*)IPR;
	irq_byte = irq_byte + (IRQNumber%4);
	*irq_byte &= 0x0f;
	*irq_byte |= (IRQPriority << 4);
}


void GPIO_IRQHandling(uint8_t PinNumber)
{
	//CLEAR THE EXTI PR REGISTER CORRESPONDING TO PIN NUMBER

	if(EXTI->EXTI_PR & (1<<PinNumber))
			{
		EXTI->EXTI_PR|=(1<<PinNumber); // clears the interrupt on peripheral side

			}
}

