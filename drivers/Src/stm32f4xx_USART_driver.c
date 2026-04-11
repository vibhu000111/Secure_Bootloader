/*
 * stm32f4xx_USART_driver.c
 *
 *  Created on: 23-Jan-2025
 *      Author: vibhu
 */

#include <stm32f407xx_USART_driver.h>
#include <stm32f407xx.h>
uint32_t getPCLK2Frequ();
uint32_t getPCLK1Frequ();
uint16_t factor_AHBu[] ={2,4,8,16,64,128,256,512};
uint8_t factor_APB1u[] ={2,4,8,16};
uint32_t getPCLK1Frequ() // pg 154 of reference manual
{

	//clk sources can be HSE,HSI OR PLLCLK
	//whatever clock is selected is prescaled by AHB prescaler and then APB prescaler

	//clk source
	uint32_t SystemClk;

	uint8_t clksource = ((RCC->RCC_CFGR >> 2) & 0x03); // gives value of SWS0 AND SWS1

	if(clksource ==0)
	{
		SystemClk = 16000000;
	}
	else if(clksource == 1)
	{
		SystemClk = 8000000;
	}
	//No support for PLL

	//Now get the value of AHB prescaler

	uint8_t AHBprescalefactor =((RCC->RCC_CFGR >> 4) & 0x0f);

	if(AHBprescalefactor < 8)
	{
		SystemClk = SystemClk/1;
	}
	else if(AHBprescalefactor >=8)
	{
		SystemClk = SystemClk/(factor_AHBu[8 - AHBprescalefactor]);
	}
	//As all I2C peripherals are connected to apb1 BUS
	uint8_t APB1_prescaler = (RCC->RCC_CFGR >> 10) & 0x07; //extract the the bits for prescaler factor

	if(APB1_prescaler<4)
	{
		SystemClk = SystemClk/1;
	}
	else if(APB1_prescaler>=4)
	{
		SystemClk = SystemClk/(factor_APB1u[APB1_prescaler - 4]);
	}




	return(SystemClk);
}

uint32_t getPCLK2Frequ() // pg 154 of reference manual
{

	//clk sources can be HSE,HSI OR PLLCLK
	//whatever clock is selected is prescaled by AHB prescaler and then APB prescaler

	//clk source
	uint32_t SystemClk;

	uint8_t clksource = ((RCC->RCC_CFGR >> 2) & 0x03); // gives value of SWS0 AND SWS1

	if(clksource ==0)
	{
		SystemClk = 16000000;
	}
	else if(clksource == 1)
	{
		SystemClk = 8000000;
	}
	//No support for PLL

	//Now get the value of AHB prescaler

	uint8_t AHBprescalefactor =((RCC->RCC_CFGR >> 4) & 0x0f);

	if(AHBprescalefactor < 8)
	{
		SystemClk = SystemClk/1;
	}
	else if(AHBprescalefactor >=8)
	{
		SystemClk = SystemClk/(factor_AHBu[8 - AHBprescalefactor]);
	}
	//As all I2C peripherals are connected to apb1 BUS
	uint8_t APB2_prescaler = (RCC->RCC_CFGR >> 13) & 0x07; //extract the the bits for prescaler factor

	if(APB2_prescaler<4)
	{
		SystemClk = SystemClk/1;
	}
	else if(APB2_prescaler>=4)
	{
		SystemClk = SystemClk/(factor_APB1u[APB2_prescaler - 4]);
	}




	return(SystemClk);
}

void USART_PeriClockControl(USART_RegDef_t *pUSARTx, uint8_t EnorDi)
{
	//Enable or Disable the clock confgured by rcc register
if(EnorDi == ENABLE)
{
	if(pUSARTx == USART1)
	{
		USART1_PCLK_EN();
	}
	if(pUSARTx == USART2)
	{
		USART2_PCLK_EN();	;
	}
	if(pUSARTx == USART3)
	{
		USART3_PCLK_EN();
	}
	if(pUSARTx == UART4)
	{
		UART4_PCLK_EN();
	}
	if(pUSARTx == UART5)
	{
		UART5_PCLK_EN();
	}
	if(pUSARTx == USART6)
	{
		USART6_PCLK_EN();
	}
}
if(EnorDi == DISABLE)
{
	if(pUSARTx == USART1)
	{
		USART1_PCLK_DI();
	}
	if(pUSARTx == USART2)
	{
		USART2_PCLK_DI();
	}
	if(pUSARTx == USART3)
	{
		USART3_PCLK_DI();
	}
	if(pUSARTx == UART4)
	{
		UART4_PCLK_DI();
	}
	if(pUSARTx == UART5)
	{
		UART5_PCLK_DI();
	}
	if(pUSARTx == USART6)
	{
		USART6_PCLK_DI();
	}
}
}
void USART_PeripheralControl(USART_RegDef_t *pUSARTx, uint8_t EnOrDi)
{
	if(EnOrDi == ENABLE)
	{
		pUSARTx->USART_CR1 |= (1 << USART_UE); //Peripheral enabled
	}
	else if(EnOrDi == DISABLE)
	{
		pUSARTx->USART_CR1 &= ~(1 << USART_UE); //Peripheral enabled
	}
}
uint8_t USART_GetFlagStatus(USART_RegDef_t *pUSARTx , uint32_t FlagName)
{
   uint8_t flag = (pUSARTx->USART_SR & FlagName);
   return(flag);
}

void USART_Init(USART_Handle_t *pUSARTHandle)
{
	//Temporary variable
	uint32_t tempreg=0;

/******************************** Configuration of CR1******************************************/

	//Implement the code to enable the Clock for given USART peripheral
	USART_PeriClockControl(pUSARTHandle->pUSARTx , ENABLE);
	//Enable USART Tx and Rx engines according to the USART_Mode configuration item
	if ( pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_RX)
	{
		//Implement the code to enable the Receiver bit field
		tempreg|= (1 << USART_RE);
	}else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_ONLY_TX)
	{
		//Implement the code to enable the Transmitter bit field
		tempreg |= ( 1 << USART_TE );

	}else if (pUSARTHandle->USART_Config.USART_Mode == USART_MODE_TXRX)
	{
		//Implement the code to enable the both Transmitter and Receiver bit fields
		tempreg |= ( ( 1 << USART_RE) | ( 1 << USART_TE) );
	}

    //Implement the code to configure the Word length configuration item
	tempreg |= pUSARTHandle->USART_Config.USART_WordLength << USART_M  ;


    //Configuration of parity control bit fields
	if ( pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_EVEN )
	{
		//Implement the code to enable the parity control
		tempreg |= ( 1 << USART_M);

		//Implement the code to enable EVEN parity
		//Not required because by default EVEN parity will be selected once you enable the parity control

	}else if (pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_EN_ODD )
	{
		//Implement the code to enable the parity control
	    tempreg |= ( 1 << USART_PCE);

	    //Implement the code to enable ODD parity
	    tempreg |= ( 1 << USART_PS);

	}
	//Check if receive interrupt from UART to core needs to be neabled
	if(pUSARTHandle->USART_Config.USART_RXInterruptEnl == 1)
	{
		tempreg |= (1 << USART_RXNEIE);
	}

   //Program the CR1 register
	pUSARTHandle->pUSARTx->USART_CR1 = tempreg;

/******************************** Configuration of CR2******************************************/

	tempreg=0;

	//Implement the code to configure the number of stop bits inserted during USART frame transmission
	tempreg |= (pUSARTHandle->USART_Config.USART_NoOfStopBits) << USART_STOP;

	//Program the CR2 register
	pUSARTHandle->pUSARTx->USART_CR2 = tempreg;

/******************************** Configuration of CR3******************************************/

	tempreg=0;

	//Configuration of USART hardware flow control
	if ( pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS)
	{
		//Implement the code to enable CTS flow control
		tempreg |= ( 1 << USART_CTSE);


	}else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_RTS)
	{
		//Implement the code to enable RTS flow control
		tempreg |= USART_RTSE;

	}else if (pUSARTHandle->USART_Config.USART_HWFlowControl == USART_HW_FLOW_CTRL_CTS_RTS)
	{
		//Implement the code to enable both CTS and RTS Flow control
		tempreg |= (1 << USART_CTSE) |(1 << USART_RTSE);
	}


	pUSARTHandle->pUSARTx->USART_CR3 = tempreg;

/******************************** Configuration of BRR(Baudrate register)******************************************/

	//Implement the code to configure the baud rate
	//We will cover this in the lecture. No action required here
	USART_SetBaudRate(pUSARTHandle->pUSARTx, pUSARTHandle->USART_Config.USART_Baud);
}

void USART_SetBaudRate(USART_RegDef_t *pUSARTx, uint32_t BaudRate)
{

	//Variable to hold the APB clock
	uint32_t PCLKx;

	uint32_t usartdiv;

	//variables to hold Mantissa and Fraction values
	uint32_t M_part,F_part;

  uint32_t tempreg=0;

  //Get the value of APB bus clock in to the variable PCLKx
  if(pUSARTx == USART1 || pUSARTx == USART6)
  {
	   //USART1 and USART6 are hanging on APB2 bus
	   PCLKx = getPCLK2Frequ();
  }else
  {
	   PCLKx = getPCLK1Frequ();
  }

  //Check for OVER8 configuration bit
  if(pUSARTx->USART_CR1 & (1 << USART_OVER8))
  {
	   //OVER8 = 1 , over sampling by 8
	   usartdiv = ((25 * PCLKx) / (2 *BaudRate));
  }else
  {
	   //over sampling by 16
	   usartdiv = ((25 * PCLKx) / (4 *BaudRate));
  }

  //Calculate the Mantissa part
  M_part = usartdiv/100;

  //Place the Mantissa part in appropriate bit position . refer USART_BRR
  tempreg |= M_part << 4;

  //Extract the fraction part
  F_part = (usartdiv - (M_part * 100));

  //Calculate the final fractional
  if(pUSARTx->USART_CR1 & ( 1 << USART_OVER8))
   {
	  //OVER8 = 1 , over sampling by 8
	  F_part = ((( F_part * 8)+ 50) / 100)& ((uint8_t)0x07);

   }else
   {
	   //over sampling by 16
	   F_part = ((( F_part * 16)+ 50) / 100) & ((uint8_t)0x0F);

   }

  //Place the fractional part in appropriate bit position . refer USART_BRR
  tempreg |= F_part;

  //copy the value of tempreg in to BRR register
  pUSARTx->USART_BRR = tempreg;
}

void USART_SendData(USART_Handle_t *pUSARTHandle, uint8_t *pTxBuffer, uint32_t Len)
{

	uint16_t *pdata;
   //Loop over until "Len" number of bytes are transferred
	for(uint32_t i = 0 ; i < Len; i++)
	{
		//Implement the code to wait until TXE flag is set in the SR
		while(! USART_GetFlagStatus(pUSARTHandle->pUSARTx,USART_FLAG_TXE));

         //Check the USART_WordLength item for 9BIT or 8BIT in a frame
		if(pUSARTHandle->USART_Config.USART_WordLength == USART_WORDLEN_9BITS)
		{
			//if 9BIT, load the DR with 2bytes masking the bits other than first 9 bits
			pdata = (uint16_t*) pTxBuffer;
			pUSARTHandle->pUSARTx->USART_DR = (*pdata & (uint16_t)0x01FF);

			//check for USART_ParityControl
			if(pUSARTHandle->USART_Config.USART_ParityControl == USART_PARITY_DISABLE)
			{
				//No parity is used in this transfer. so, 9bits of user data will be sent
				//Implement the code to increment pTxBuffer twice
				pTxBuffer++;
				pTxBuffer++;
			}
			else
			{
				//Parity bit is used in this transfer . so , 8bits of user data will be sent
				//The 9th bit will be replaced by parity bit by the hardware
				pTxBuffer++;
			}
		}
		else
		{
			//This is 8bit data transfer
			pUSARTHandle->pUSARTx->USART_DR = (*pTxBuffer  & (uint8_t)0xFF);

			//Implement the code to increment the buffer address
			pTxBuffer++;
		}
	}

	//Implement the code to wait till TC flag is set in the SR
	while( ! USART_GetFlagStatus(pUSARTHandle->pUSARTx,USART_FLAG_TC));
}

void USART_IRQConfig(uint8_t IRQNumber,uint8_t EnDi)
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
