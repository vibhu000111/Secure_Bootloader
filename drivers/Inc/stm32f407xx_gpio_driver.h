/*
 * stm32f407xx_gpio_driver.h
 *
 *  Created on: 23-Nov-2024
 *      Author: vibhu
 */

#ifndef INC_STM32F407XX_GPIO_DRIVER_H_
#define INC_STM32F407XX_GPIO_DRIVER_H_

#include <stm32f407xx.h> //the driver header file should contain the MCU specific header file
/*
	 * 00: Input (reset state)
       01: General purpose output mode
       10: Alternate function mode
       11: Analog mode
       Assign Macro in the driver header file for this
	 */
#define INPUT 0
#define OUTPUT 1
#define ALTFN 2
#define ANALOG 3
#define GPIO_MODE_FT 4
#define GPIO_MODE_RT 5
#define GPIO_MODE_RTFT 6

/* gpio output type
 * 0: Output push-pull (reset state)
   1: Output open-drain
 */

#define GPIO_OP_TYPE_PP 0
#define GPIO_OP_TYPE_OD 1

/* gpio speed register
 * 00: Low speed
   01: Medium speed
   10: High speed
   11: Very high speed
 */

#define GPIO_SPEED_LOW 0
#define GPIO_SPEED_MEDIUM 1
#define GPIO_SPEED_HIGH 2
#define GPIO_SPEED_VERY_HIGH 3

/* gpio PUPDR
 * 00: No pull-up, pull-down
   01: Pull-up
   10: Pull-down
   11: Reserved
 */
#define GPIO_NO_PUPD 0
#define GPIO_PU 1
#define GPIO_PD 2

//GPIO possible pin numbers
#define GPIO_PIN0 0
#define GPIO_PIN1 1
#define GPIO_PIN2 2
#define GPIO_PIN3 3
#define GPIO_PIN4 4
#define GPIO_PIN5 5
#define GPIO_PIN6 6
#define GPIO_PIN7 7
#define GPIO_PIN8 8
#define GPIO_PIN9 9
#define GPIO_PIN10 10
#define GPIO_PIN11 11
#define GPIO_PIN12 12
#define GPIO_PIN13 13
#define GPIO_PIN14 14
#define GPIO_PIN15 15
//Configuration structure for GPIO pin


typedef struct  //this register is not a peripheral memory mapped register this will occupy memory in stack or heap
{
	uint8_t GPIO_PinNumber; // uint8_t because pin number ranges from 0 to 15 which is in the range of 0 to 255
	uint8_t GPIO_PinMode; //this value will be stored in the moder reg
	uint8_t GPIO_PinSpeed; // check the register definition of GPIO
	uint8_t GPIO_PinPUPD;
	uint8_t GPIO_PinOPType;
	uint8_t GPIO_PinAltFunMode;
}GPIO_PinConfig_t;  //this register holds the configuration details of the PIN of the GPIO port being used in Sample application and will be initialized by user application


//Handle structure for GPIO pin

typedef struct
{
	GPIO_RegDef_t* pGPIOx;  //this pointer variable will be intialized by the user application to define which GPIO port is being used
	//now after definiton the port to be used the user application will have to configure the pin to be used
	//for that we will create a separate structure for pin configuration which will hold the values which have to put in the specific registers these
	//registers cann be accessed by pGPIOx pointer to a structure check the structure definition in the MCU specific header

	GPIO_PinConfig_t GPIO_pin;  // use . operator to access the structure variables like GPIO_pin.
}GPIO_Handle_t;

void GPIO_init(GPIO_Handle_t *pGPIOHandle); //user application should create a variable of GPIO_Handle_t type initialize it and send the pointer
//to GPIO_intit function to initialize the GPIO port and Pin

void GPIO_deinit(GPIO_RegDef_t* pGPIOx);
//used to deinitialize the registers of the given GPIO peripheral meaning put all the register specific to that GPIO back to its reset state
//can be done in one shot by using  a register in the RCC peripheral using the RCC peripheral reset register refer the reference manual

void GPIO_Peripehral_clk_control(GPIO_RegDef_t *pGPIOx,uint8_t EnDi); //to enable or disable the clock I need to know the base adddress of the
//GPIO which can then be mapped to which bit to set or reset in the RCC AHB1 ENR register


uint8_t GPIO_ReadInputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber); //to read the IDR register this function need the base address of the GPIO port and the pin number to specify which bit to read from IDR
uint16_t GPIO_ReadInputPort(GPIO_RegDef_t *pGPIOx);
void GPIO_WriteOutputPort(GPIO_RegDef_t *pGPIOx,uint16_t value);
void GPIO_WriteOutputPin(GPIO_RegDef_t *pGPIOx, uint8_t PinNumber , uint8_t value);
void GPIO_ToggleOutputPin(GPIO_RegDef_t *pGPIOx ,uint8_t PinNumber);
void GPIO_altfn(void);
uint32_t GPIO_baseaddr_to_code(GPIO_RegDef_t* m);


//IRQ_configuration = configuring NVIC registers like IPR for priority , ISER for enabling interrupts from specific IRQ and IRQHandling for handling the ISR

void GPIO_IRQConfig(uint8_t IRQNumber,uint8_t EnDi);
void GPIO_IRQ_PriorityConfig(uint8_t IRQNumber , uint8_t IRQPriority);
void GPIO_IRQHandling(uint8_t IRQNumber);



#endif /* INC_STM32F407XX_GPIO_DRIVER_H_ */
