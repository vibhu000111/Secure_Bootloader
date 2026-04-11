/*
 * stm32f4xx_I2C_driver.h
 *
 *  Created on: 17-Jan-2025
 *      Author: vibhu
 */

#ifndef INC_STM32F4XX_I2C_DRIVER_H_
#define INC_STM32F4XX_I2C_DRIVER_H_
#include <stm32f407xx.h>


typedef struct
{
	uint32_t I2C_SCLSpeed;
	uint8_t DeviceAddress;
	uint8_t I2C_ACKControl;
	uint16_t I2C_FMDutyCycle;
}I2C_config_t;

typedef struct
{
	I2C_RegDef_t* pI2Cx; // initialized with pointer to a structure which points to base address of I2Cx peripheral
	I2C_config_t I2C_config_params;
	uint8_t* pTxBuffer;
	uint8_t* pRxBuffer;
	uint32_t RxLen;
	uint32_t TxLen;
	uint8_t Slaveaddr;
	uint8_t TxRx_status;
	uint8_t Sr;
	uint8_t RxSize ;
}I2C_handle_t;

//I2C APPLICATION STATE

#define I2C_READY 0
#define I2C_BUSY_IN_TX 2
#define I2C_BUSY_IN_RX 1

//I2C application events
#define I2C_EV_TX_CMPLT 0
#define EVENT_STOP 2
#define I2C_EV_RX_CMPLT  1
//i2c scl speed

#define I2C_SCL_SPEED_SM 100000 //100KHZ
#define I2C_SCL_SPEED_FM 400000 //400KHZ


//OPTINS FOR ACK CONTROL

#define I2C_ACK_EN  1
#define I2C_ACK_DI  0

#define I2C_FM_DUTY_2    0
#define I2C_FM_DUTY_16_9 1

// I2C status flag

#define I2C_FLAG_SB (1 << I2C_SB)
#define I2C_FLAG_ADDR (1 << I2C_ADDR)
#define I2C_FLAG_BTF (1 << I2C_BTF)
#define I2C_FLAG_ADD10 (1 << I2C_ADD10)
#define I2C_FLAG_STOPF (1 << I2C_STOPF)
#define I2C_FLAG_RXNE (1 << I2C_RXNE)
#define I2C_FLAG_TXE (1 << I2C_TXE)
#define I2C_FLAG_BERR (1 << I2C_BERR)
#define I2C_FLAG_ARLO (1 << I2C_ARLO)
#define I2C_FLAG_AF (1 << I2C_AF)
#define I2C_FLAG_OVR (1 << I2C_OVR)
#define I2C_FLAG_PECERR (1 << I2C_PECERR)
#define I2C_FLAG_TIMEOUT (1 << I2C_TIMEOUT)
#define I2C_FLAG_SMBALERT (1 << I2C_SMBALERT)

#define REP_START_DISABLE 0
#define REP_START_ENABLE 1
void I2C_Peripehral_clk_control(I2C_RegDef_t *I2Cpx,uint8_t EnDi);

//Init or deinit the peripheral

void I2C_init(I2C_handle_t *pI2Chandle);
void I2C_deinit(I2C_handle_t *pI2Chandle);

//Data send or receive
// in communication peripherals
// 3 types of data transmission methods
// blocking , interrupt , DMA based


void I2C_IRQConfig(uint8_t IRQNumber,uint8_t EnDi);
void I2C_IRQ_PriorityConfig(uint8_t IRQNumber , uint8_t IRQPriority);
void I2C_EV_IRQHandling(I2C_handle_t *pI2Chandle);
void I2C_ER_IRQHandling(I2C_handle_t *pI2Chandle);

uint8_t I2C_MasterReceiveDataIT(I2C_handle_t *pI2Chandle, uint8_t *pTxBuffer ,uint32_t Len, uint8_t SlaveAddr , uint8_t rep_start);
void I2C_MasterSendData(I2C_handle_t *pI2Chandle, uint8_t *pTxBuffer ,uint32_t Len, uint8_t SlaveAddr , uint8_t rep_start);
void I2C_MasterReceiveData(I2C_handle_t *pI2Chandle, uint8_t *pRxBuffer ,uint32_t Len, uint8_t SlaveAddr , uint8_t rep_start);
uint8_t I2C_GetFlagStatus(I2C_RegDef_t* pI2Cx,uint32_t FlagName);
uint8_t I2C_MasterSendDataIT(I2C_handle_t *pI2Chandle, uint8_t *pTxBuffer ,uint32_t Len, uint8_t SlaveAddr, uint8_t rep_start); //returns the state to the application
void I2C_ApplicationEventCallback(I2C_handle_t *pI2Chandle, uint8_t AppEv);
void I2C_EnDi(I2C_RegDef_t *pI2Cx , uint8_t EnDi);

#endif /* INC_STM32F4XX_I2C_DRIVER_H_ */
