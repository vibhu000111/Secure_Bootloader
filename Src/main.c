#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_USART_driver.h"
#include "string.h"
#include "queue.h"
#include "timebase.h"
#include <stdarg.h>
#include <stdio.h>   // for vsprintf
#include <string.h>  // for strlen
#define APP_ADDR 0x08008000
#define CHIP_ID_ADDR (uint32_t*) 0xE0042000
/*
 * SCL PB6
 * SDA PB9
 */
// IRQ number for USART3 is 39
//
uint8_t a[QUEUE_SIZE];
USART_Handle_t usart_init;
USART_Handle_t usartdebug;
uint8_t data;
struct dataoveruart {
	uint8_t command;
	uint8_t length;
	uint8_t payload[8];
};
struct dataoveruart parsed={
	.command = 0,
	.length =0,
	.payload={0}
};
char buf[] = "Test string";
UART_Queue_t q={
		.buffer={0},
		.count=0,
		.head=0,
		.tail=0
};
volatile int i=0;
void printchipid()
{
	uint32_t chipid  = *CHIP_ID_ADDR & 0x7ff;
	printmsg("Chip ID = %x\n",chipid);
}
void print_supported_commands()
{
	printmsg("BL_GET_VER\n");
	printmsg("BL_GET_HELP\n");
	printmsg("BL_GET_CID\n");

}
void printmsg(char *format,...)
{
	char str [80];
	va_list args;
	va_start(args,format);
	vsprintf(str,format,args);
	USART_SendData(&usartdebug,(uint8_t*)str, strlen(str));
	va_end(args);

}
void parser(struct dataoveruart* x)
{
	uint8_t data;
	int ret_code = Queue_Dequeue(&q,&data);
	if(ret_code == 0 && data == 0xAA)
	{
		ret_code = Queue_Dequeue(&q,&data);
		x->command = data;
		ret_code = Queue_Dequeue(&q,&data);
		x->length = data;
		for(uint8_t i = 0 ; i<x->length;i++)
		{
		    ret_code = Queue_Dequeue(&q, &data);
		    x->payload[i] = data;
		}
		//printmsg("Command = %d  Length = %d payload = %d ",x->command,x->length,x->payload);

	}
	else if(ret_code == -1)
	{
		//printmsg("Queue is empty \n");
	}
}
void jump_to_app()
{
	uint32_t MSP_value = *(uint32_t*)(APP_ADDR); //  MSP value the first address stores the MSP value

	void (*app_reset_handler) (void);
	app_reset_handler = (void*) *(uint32_t*)(APP_ADDR+4);
	printmsg ("MSP value = %x\n   ",MSP_value);
	//__set_MSP(MSP_value);
	/* Disable USARTs */
	USART2->USART_CR1 = 0;
	USART3->USART_CR1 = 0;

	/* Stop SysTick */
	//SysTick->CTRL = 0;
//	SysTick->LOAD = 0;
//	SysTick->VAL = 0;

	/* Clear NVIC */
//	for(int i=0;i<8;i++)
//	{
//	    NVIC->ICER[i] = 0xFFFFFFFF;
//	    NVIC->ICPR[i] = 0xFFFFFFFF;
//	}
	   __asm volatile ("cpsid i");
	uint32_t* VTOR = (uint32_t*) 0xE000ED08;
	*VTOR = APP_ADDR;

	__asm volatile ("MSR MSP, %0" : : "r" (MSP_value)); //
//	__asm volatile("LDR R0,=0x08008000");
//	__asm volatile("LDR R1,[R0]");
//	__asm volatile("MSR MSP,R1");

	//app_reset_handler();
	__asm volatile ("BX %0" : : "r" (app_reset_handler));


}
void delay()
{
	for(int i=0;i<=80000;i++)
	{

	}
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

	USARTpins.pGPIOx = GPIOA;
	USARTpins.GPIO_pin.GPIO_PinNumber=2;
	//USART2 RX PA3  ; TX PA2
	GPIO_init(&USARTpins);
	USARTpins.GPIO_pin.GPIO_PinNumber=3;
	GPIO_init(&USARTpins);

	GPIO_Handle_t pushbutton;
	pushbutton.pGPIOx=GPIOE;
	pushbutton.GPIO_pin.GPIO_PinNumber=4;
	pushbutton.GPIO_pin.GPIO_PinMode=INPUT;
	pushbutton.GPIO_pin.GPIO_PinOPType=0;
	pushbutton.GPIO_pin.GPIO_PinPUPD = GPIO_PU;
	pushbutton.GPIO_pin.GPIO_PinSpeed=GPIO_SPEED_HIGH;
	GPIO_init(&pushbutton);


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

	usartdebug.pUSARTx = USART2;
	usartdebug.USART_Config.USART_Baud = USART_STD_BAUD_115200;
	usartdebug.USART_Config.USART_Mode = USART_MODE_ONLY_TX;
	usartdebug.USART_Config.USART_NoOfStopBits = USART_STOPBITS_1 ;
	usartdebug.USART_Config.USART_ParityControl = USART_PARITY_DISABLE;
	usartdebug.USART_Config.USART_HWFlowControl =  USART_HW_FLOW_CTRL_NONE;
	usartdebug.USART_Config.USART_WordLength = USART_WORDLEN_8BITS;

	USART_Init(&usartdebug);

}
int main()
{
	init_systick_timer(1000);
	Queue_Init(&q);
	USART_GPIOInits();
	USART_Inits();
	USART_PeripheralControl(USART3,ENABLE);
	USART_PeripheralControl(USART2,ENABLE);
//	GPIO_IRQConfig(6,ENABLE);







	while(1)
	{
//		printmsg("In bootloader mode\n");
//		delayTicks(1000);// delay of 200 ms
		//if(GPIO_ReadInputPin(GPIOE,4) == 0)
		//{
			printmsg("Continuing into bootloader mode.. timeout set to 1 second\n");

			uint8_t flag = 1;
			while(flag == 1)
			{

				parser(&parsed);
				switch(parsed.command)
				{
				case 0x51:
					printmsg("Bootloader Version 1.0\n");
				case 0x52:
					print_supported_commands();
				case 0x53:
					printchipid();
				}

				delayTicks(100);
				memset(&parsed,0,sizeof(parsed));
			}


		//}
//		else
//		{
//			printmsg("Jumping to application code\n in 1 seconds\n");
//			delayTicks(1000);
//			jump_to_app();
//
//		}

	}



}
