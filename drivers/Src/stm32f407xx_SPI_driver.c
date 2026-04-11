


#include <stm32f407xx_SPI_driver.h>

static void spi_txe_handle(SPI_Handle_t *pSPIHandle);
static void spi_rxe_handle(SPI_Handle_t *pSPIHandle);
//static void spi_ovf_handle(SPI_Handle_t *pSPIHandle);

void SPI_Peripehral_clk_control(SPI_RegDef_t *pSPIx,uint8_t EnDi)
{
	if (EnDi == ENABLE)
	{
		if(pSPIx == SPI1)
		{
			SPI1_PCLK_EN();
		}
		else if(pSPIx == SPI2)
		{
			SPI2_PCLK_EN();

		}
		else if(pSPIx == SPI3)
		{
			SPI3_PCLK_EN();

		}

	}
	else
	{
		if(EnDi == DISABLE)
		{
			if(pSPIx == SPI1)
					{
						SPI1_PCLK_DI();
					}
					else if(pSPIx == SPI2)
					{
						SPI2_PCLK_DI();

					}
					else if(pSPIx == SPI3)
					{
						SPI3_PCLK_DI();

					}
		}
	}
}

//Init or deinit the peripheral

void SPI_init(SPI_Handle_t *pSPIHandle)
{
	SPI_Peripehral_clk_control(pSPIHandle->pSPIx,ENABLE);
	uint32_t tempreg=0;

	//1. configure the device mode
	tempreg|=(pSPIHandle->SPIConfig.SPI_DeviceMode<<2);
	//2. Configure the busconfig mode
	if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_FD)
	{
		//2-line unidirectional data mode has to be selected
		tempreg&=~(1<<15);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_HD)
	{
		//1-line bidirectional data mode has to be selected
		tempreg|=(1<<15);
	}
	else if(pSPIHandle->SPIConfig.SPI_BusConfig == SPI_BUS_CONFIG_SIMPLEX_RXONLY)
	{
		//2-line unidirectional mode is selected
		tempreg&=~(1<<15);
		//RXONLY bit must be set
		tempreg|=(1<<10);
	}

	//3. CONFIGURE CLOCK SPEED
	tempreg|=(pSPIHandle->SPIConfig.SPI_SclkSpeed << 3);
	// 4. Configure Data frame format
	tempreg|=(pSPIHandle->SPIConfig.SPI_DFF << 11);
	// 5. Configure clock polarity
	tempreg|=(pSPIHandle->SPIConfig.SPI_CPOL << 1);
	// configure clock phase
	tempreg|=(pSPIHandle->SPIConfig.SPI_CPHA << 0);
	// configure SSM
	tempreg|=(pSPIHandle->SPIConfig.SPI_SSM << 9);

	if(pSPIHandle->SPIConfig.SPI_SSM == 1) // is SSM is selected SSI bit has to be set to 1 otherwise MODF fault occurs
	{
		tempreg|=(1<<8);
	}

	pSPIHandle->pSPIx->SPI_CR1 = tempreg; // pSPIX->SPI_CR1 means derefrencing the base address of SPI + offset to access specific register
    if(pSPIHandle->SPIConfig.SPI_SSM == 0)
    {
    	pSPIHandle->pSPIx->SPI_CR2|=(1<<2);

    }

}
void SPI_deinit(SPI_Handle_t *pSPIHandle);

//Data send or receive
// in communication peripherals
// 3 types of data transmission methods
// blocking , interrupt , DMA based

void SPI_SendData(SPI_RegDef_t* pSPIx , uint8_t *pTxBuffer , uint32_t length) // length dhould always be uint32 , this code is polling based
{
	uint32_t temp = length;
	uint16_t *pTx_Buffer;
	uint32_t DFF =( (pSPIx->SPI_CR1) & 1<<11);
	if (DFF==1)
	{
		pTx_Buffer = (uint16_t*)pTxBuffer;

	}
	while (temp>0)
	{
		//uint32_t txe_status= (pSPIx->SPI_SR & (1<<1));
		while((pSPIx->SPI_SR & (1<<1))==0)
		{
		}
		if(DFF == 1)
		{
			pSPIx->SPI_DR = *(pTx_Buffer);// accesses two locations at a time
			temp--;
			temp--;
			pTx_Buffer++; // points to location ahead as 16 bit pointer
		}
		else
		{
			pSPIx->SPI_DR = *(pTxBuffer);
			temp--;
			pTxBuffer++;
		}
		}
	}
void SPI_ReceiveData(SPI_RegDef_t* pSPIx,uint8_t *pRxBuffer , uint32_t length)
{
	uint32_t temp= length;
	uint16_t *pRx_Buffer = (uint16_t*)pRxBuffer;
	uint32_t DFF =( (pSPIx->SPI_CR1) & 1<<11);
	while(temp>0)
	{

		while( (pSPIx->SPI_SR & (1<<0)) == 0);

		if(DFF == 1)
		{
			*pRx_Buffer = pSPIx->SPI_DR; // read data register
			temp--;
			temp--;
			pRx_Buffer++;
		}
		else
		{
			*pRxBuffer = pSPIx->SPI_DR;
			temp--;
			pRxBuffer++;
		}


	}
}

void SPI_EnDi(SPI_RegDef_t *pSPIx , uint8_t EnDi)
{
	if(EnDi == ENABLE)
	{
		SPI2_PCLK_EN();
  pSPIx->SPI_CR1|=(1<<6);
	}
	else if(EnDi ==DISABLE)
	{
		pSPIx->SPI_CR1&=~(1<<6);
		SPI2_PCLK_DI();
	}

}
uint8_t SPI_GetStatusFLAG(SPI_RegDef_t* pSPIx)
{
	return((pSPIx->SPI_SR & (1<<7)));
}
void SPI_IRQConfig(uint8_t IRQNumber,uint8_t EnDi)
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
void SPI_IRQ_PriorityConfig(uint8_t IRQNumber , uint8_t IRQPriority)
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
void SPI_IRQHandling(SPI_Handle_t *pHandle)
{
	uint8_t temp1 ,temp2;
	//check reason for interrupt
	temp1 = pHandle->pSPIx->SPI_SR & (1<<1);
	temp2 = pHandle->pSPIx->SPI_CR2 & (1<<7);

	if (temp1 && temp2)
	{
		//handle TXE
		spi_txe_handle(pHandle);

	}
	temp1 = pHandle->pSPIx->SPI_SR & (1<<0);
	temp2 = pHandle->pSPIx->SPI_CR2 & (1<<6);

	if(temp1 && temp2)
	{
		//handle RXE
		spi_rxe_handle(pHandle);
	}
	//Can be improved for CRC error
	//cehck for OVR flag

}

uint8_t SPI_SendDataIT(SPI_Handle_t* pSPIHandle , uint8_t *pTxBuffer , uint32_t length)
{
	uint8_t state = pSPIHandle->TxState;
	if(state != SPI_BUSY_IN_TX)
	{


	//1. Save the Tx buffer address and Len info in some global variable
	pSPIHandle->pTxBuffer = pTxBuffer;
	pSPIHandle->TxLen = length;

	//2. Mark the SPI state as busy in transmission so that no other code can take over same SPI peripheral uintil transmission is over

	pSPIHandle->TxState = SPI_BUSY_IN_TX;



	//3.Enable the TXIE control bit to get interrupt whenever TXE flag is set in SR
	pSPIHandle->pSPIx->SPI_CR2|=(1<<7); //unmasks the TX interrupt from processor

	}
	 return state;


	//4. Data transmission will be handled by the ISR code
}
uint8_t SPI_ReceiveDataIT(SPI_Handle_t* pSPIHandle, uint8_t *pRxBuffer , uint32_t length)
{
	uint8_t state = pSPIHandle->RxState;
		if(state != SPI_BUSY_IN_RX)
		{


		//1. Save the Tx buffer address and Len info in some global variable
		pSPIHandle->pRxBuffer = pRxBuffer;
		pSPIHandle->RxLen = length;

		//2. Mark the SPI state as busy in transmission so that no other code can take over same SPI peripheral uintil transmission is over

		pSPIHandle->RxState = SPI_BUSY_IN_RX;



		//3.Enable the RXNIE control bit to get interrupt whenever RXE flag is set in SR
		pSPIHandle->pSPIx->SPI_CR2|=(1<<6); //unmasks the RX interrupt from processor

		}
		 return state;
}
static void spi_txe_handle(SPI_Handle_t *pSPIHandle) //helper function implementation
{
	if((pSPIHandle->pSPIx->SPI_CR1 & (1<<11)) != 0)
	{
		pSPIHandle->pSPIx->SPI_DR = *((uint16_t*)(pSPIHandle->pTxBuffer));
		pSPIHandle->pTxBuffer++;
		pSPIHandle->pTxBuffer++;;
		pSPIHandle->TxLen--;
		pSPIHandle->TxLen--;


	}
	else
	{
		pSPIHandle->pSPIx->SPI_DR = *(pSPIHandle->pTxBuffer);
		pSPIHandle->pTxBuffer++;
		pSPIHandle->TxLen--;
	}

	if(pSPIHandle->TxLen == 0)
	{
		pSPIHandle->pSPIx->SPI_CR2 &= ~(1 << 7);
		pSPIHandle->pTxBuffer = NULL;
		pSPIHandle->TxState = SPI_READY;
		pSPIHandle->TxLen=0;
		SPI_ApplicationEventCallback(pSPIHandle , SPI_EVENT_TX_CMPLT);
	}

}
static void spi_rxe_handle(SPI_Handle_t *pSPIHandle)
{
	if((pSPIHandle->pSPIx->SPI_CR1 & (1<<11)) != 0)
	{
		*((uint16_t*)(pSPIHandle->pRxBuffer)) = pSPIHandle->pSPIx->SPI_DR;
		pSPIHandle->pRxBuffer++;
		pSPIHandle->RxLen--;
		pSPIHandle->RxLen--;


	}
	else
	{
		*(pSPIHandle->pRxBuffer) = pSPIHandle->pSPIx->SPI_DR ;
		pSPIHandle->pRxBuffer++;
		pSPIHandle->RxLen--;
	}

	if(pSPIHandle->RxLen == 0)
	{
		pSPIHandle->pSPIx->SPI_CR2 &= ~(1 << 6);
		pSPIHandle->pRxBuffer = NULL;
		pSPIHandle->RxState = SPI_READY;
		pSPIHandle->RxLen=0;
		SPI_ApplicationEventCallback(pSPIHandle , SPI_EVENT_RX_CMPLT);
	}
}
__attribute__((weak)) void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv)
{
	//weak implementation
}
