#ifndef INC_STM32F407XX_SPI_DRIVER_H_
#define INC_STM32F407XX_SPI_DRIVER_H_

#include <stm32f407xx.h> //the driver header file should contain the MCU specific header file

#define RXNE 0
#define TXE 1
#define CHSIDE 2
#define UDR 3
#define CRCERR 4
#define MODF 5
#define OVR 6
#define BSY 7
#define FRE 8




#define  SPI_DEVICE_MODE_MASTER 1
#define  SPI_DEVICE_MODE_SLAVE 0

#define SPI_BUS_CONFIG_FD 1
#define SPI_BUS_CONFIG_HD 2
#define SPI_BUS_CONFIG_SIMPLEX_RXONLY 3

#define SPI_SCLK_SPEED_DIV2 0
#define SPI_SCLK_SPEED_DIV4 1
#define SPI_SCLK_SPEED_DIV8 2
#define SPI_SCLK_SPEED_DIV16 3
#define SPI_SCLK_SPEED_DIV32 4
#define SPI_SCLK_SPEED_DIV64 5
#define SPI_SCLK_SPEED_DIV128 6
#define SPI_SCLK_SPEED_DIV256 7

#define SPI_DFF_8BITS 0
#define SPI_DFF_16BITS 1

#define SPI_CPOL_HIGH 1
#define SPI_CPOL_LOW 0

#define SPI_CPHA_TRAILING_EDGE 1
#define SPI_CPHA_LEADING_EDGE 0

#define SPI_SSM_EN 1
#define SPI_SSM_DI 0

#define SPI_READY 0
#define SPI_BUSY_IN_RX 1
#define SPI_BUSY_IN_TX 2

#define SPI_EVENT_TX_CMPLT 0
#define SPI_EVENT_RX_CMPLT 1
#define SPI_EVENT_OVR_ERR 2


typedef struct
{
	uint8_t SPI_DeviceMode; // SPI master or Slave
	uint8_t SPI_BusConfig; // half duplex , simplex or full duplex
	uint8_t SPI_SclkSpeed;
	uint8_t SPI_DFF; // 8 bit or 16 bit of shift register
	uint8_t SPI_CPOL;
	uint8_t SPI_CPHA;
	uint8_t SPI_SSM;
}SPI_Config_t;


typedef struct
{
	SPI_RegDef_t *pSPIx; // this will hold the base address of SPIx(x = 1,2,3) peripheral
	SPI_Config_t SPIConfig; // structure holding all the configurable items for SPI
	uint8_t *pTxBuffer; // to store application Tx Buffer address
	uint8_t *pRxBuffer; // to store the application Rx buffer address
	uint32_t TxLen; // to store Tx len
	uint32_t RxLen; // to store Rx len
	uint8_t TxState; // to store Tx state
	uint8_t RxState; // To store Rx state

}SPI_Handle_t;




// peripheral clock setup enable or disable peripehral clock

void SPI_Peripehral_clk_control(SPI_RegDef_t *pSPIx,uint8_t EnDi);

//Init or deinit the peripheral

void SPI_init(SPI_Handle_t *pSPIHandle);
void SPI_deinit(SPI_Handle_t *pSPIHandle);

//Data send or receive
// in communication peripherals
// 3 types of data transmission methods
// blocking , interrupt , DMA based

void SPI_SendData(SPI_RegDef_t* pSPIx , uint8_t *pTxBuffer , uint32_t length); // length dhould always be uint32
void SPI_ReceiveData(SPI_RegDef_t* pSPIx,uint8_t *pRxBuffer , uint32_t length);

uint8_t SPI_SendDataIT(SPI_Handle_t* pSPIHandle , uint8_t *pTxBuffer , uint32_t length);
uint8_t SPI_ReceiveDataIT(SPI_Handle_t* pSPIHandle,uint8_t *pRxBuffer , uint32_t length);
uint8_t SPI_GetStatusFLAG(SPI_RegDef_t* pSPIx);
void SPI_IRQConfig(uint8_t IRQNumber,uint8_t EnDi);
void SPI_IRQ_PriorityConfig(uint8_t IRQNumber , uint8_t IRQPriority);
void SPI_IRQHandling(SPI_Handle_t *pHandle);

void SPI_ApplicationEventCallback(SPI_Handle_t *pSPIHandle, uint8_t AppEv);
void SPI_EnDi(SPI_RegDef_t *pSPIx , uint8_t EnDi);













#endif /* INC_STM32F407XX_SPI_DRIVER_H_ */
