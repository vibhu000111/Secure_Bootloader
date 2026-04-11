#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_USART_driver.h"
#include "string.h"
#include "queue.h"
#include "timebase.h"
/*
 * SCL PB6
 * SDA PB9
 */
// IRQ number for USART3 is 39
//
uint8_t a[QUEUE_SIZE];
USART_Handle_t usart_init;
uint8_t data;
char buf[] = "Test string";
UART_Queue_t q={
		.buffer={0},
		.count=0,
		.head=0,
		.tail=0
};
volatile int i=0;
void delay()
{
	for(int i=0;i<=80000;i++)
	{

	}
}

void EXTI0_IRQHandler()
{
	delay();
		//char user_data[] = "Hi Alok\r\nEnter your Account number\r\n";
	char y[]="Overflown\r\n";
		uint8_t len = strlen(y);
		if(q.count>=256)
		USART_SendData(&usart_init, (uint8_t*)y, len);

		i=0;
		GPIO_IRQHandling(0);
		uint32_t* pNVIC_ICPR0=(uint32_t*)(0XE000E280);
				*pNVIC_ICPR0|=(1<<6);


}
void USART3_IRQHandler()
{

	data = USART3->USART_DR;
	if(Queue_Enqueue(&q,data)==-1)
	{
		i=1;
	}

}

void USART_GPIOInits(void)
{
	GPIO_Handle_t USARTpins;

	USARTpins.pGPIOx = GPIOC;
	USARTpins.GPIO_pin.GPIO_PinMode = ALTFN;
	USARTpins.GPIO_pin.GPIO_PinAltFunMode = 7 ;
	USARTpins.GPIO_pin.GPIO_PinOPType = GPIO_OP_TYPE_PP;
	USARTpins.GPIO_pin.GPIO_PinPUPD = GPIO_PU;
	USARTpins.GPIO_pin.GPIO_PinSpeed = GPIO_SPEED_HIGH;

	//TX
	USARTpins.GPIO_pin.GPIO_PinNumber=10;
	GPIO_init(&USARTpins);

	//RX
	USARTpins.GPIO_pin.GPIO_PinNumber=11;
	GPIO_init(&USARTpins);

	GPIO_Handle_t pGPIOA ;
	pGPIOA.pGPIOx=GPIOA;
	pGPIOA.GPIO_pin.GPIO_PinNumber=0;
	pGPIOA.GPIO_pin.GPIO_PinMode=GPIO_MODE_RT;
	pGPIOA.GPIO_pin.GPIO_PinOPType=0;
	pGPIOA.GPIO_pin.GPIO_PinPUPD= GPIO_NO_PUPD;
	pGPIOA.GPIO_pin.GPIO_PinSpeed=GPIO_SPEED_HIGH;
	GPIO_init(&pGPIOA);


}

void USART_Inits()
{


	usart_init.pUSARTx = USART3;
	usart_init.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usart_init.USART_Config.USART_Mode = USART_MODE_TXRX;
	usart_init.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1 ;
	usart_init.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usart_init.USART_Config.USART_HWFlowControl =  USART_HW_FLOW_CTRL_NONE;
	usart_init.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;
	usart_init.USART_Config.USART_RXInterruptEnl = ENABLE;



	USART_Init(&usart_init);
	USART_IRQConfig(39,ENABLE);

}
int main()
{
	init_systick_timer(1000);
	Queue_Init(&q);
	USART_GPIOInits();
	USART_Inits();
	USART_PeripheralControl(USART3,ENABLE);
	GPIO_IRQConfig(6,ENABLE);
	//USART_PeripheralControl(USART2,ENABLE);







	while(1)
	{
		USART_SendData(&usart_init,(uint8_t*)buf, strlen(buf));
		delayTicks(1000);

	}



}
