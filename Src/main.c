#include "stm32f407xx.h"
#include "stm32f407xx_gpio_driver.h"
#include "stm32f407xx_USART_driver.h"
#include "string.h"
#include "queue.h"
#include "timebase.h"
#include <stdarg.h>
#include <stdio.h>   // for vsprintf
#include <string.h>  // for strlen
#include <main.h>
#include <crc.h>
#include <flash_driver.h>
#define APP_ADDR 0x08008000
#define CHIP_ID_ADDR (uint32_t*) 0xE0042000
uint8_t buffer_rx[256] = {0};
uint8_t data_w = 0x69;
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
	uint8_t payload[256];
	uint32_t crc;
};
struct dataoveruart parsed={
	.command = 0,
	.length =0,
	.payload={0},
	.crc=0
};
char buf[] = "Test string";
UART_Queue_t q={
		.buffer={0},
		.count=0,
		.head=0,
		.tail=0
};
volatile int i=0;
uint8_t ack = BL_ACK;
uint8_t nack = BL_NACK;
uint8_t curr_address = 0;
/*
uint8_t bootloader_verify_crc(uint8_t command,uint8_t len,uint8_t* payload,uint32_t crc_host)
{
   uint32_t accumulated_crc = 0xffffffff;
   calculate_crc(&accumulated_crc, accumulated_crc);
   calculate_crc(&accumulated_crc, command);
   calculate_crc(&accumulated_crc, len);
   for(uint8_t i=0;i<len;i++)
   {
	   calculate_crc(&accumulated_crc, payload[i]);
   }
   if(accumulated_crc == crc_host)
   {
	   return(1);
   }
   else
   {
	   return(0);
   }

}*/

uint32_t crc32_stm32_style(uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFF;
    uint32_t poly = 0x04C11DB7;

    for(uint32_t i = 0; i < length; i++)
    {
        crc ^= ((uint32_t)data[i] << 24);

        for(uint8_t bit = 0; bit < 8; bit++)
        {
            if(crc & 0x80000000)
            {
                crc = (crc << 1) ^ poly;
            }
            else
            {
                crc <<= 1;
            }
        }
    }

    return crc;
}
uint8_t bootloader_verify_crc(uint8_t command,
                              uint8_t len,
                              uint8_t *payload,
                              uint32_t crc_host)
{
    uint8_t buffer[258];

    buffer[0] = command;
    buffer[1] = len;

    for(uint8_t i = 0; i < len; i++)
    {
        buffer[2 + i] = payload[i];
    }

    uint32_t crc =
        crc32_stm32_style(buffer, len + 2);

    return (crc == crc_host);
}
////  Implementation of BL command handle functions ////
void send_ack();
void send_nack()
{
	USART_SendData(&usart_init, &nack , 1);
}
void send_ack()
{

	USART_SendData(&usart_init, &ack , 1);
	printmsg("ACK SENT\n");
}
void printchipid()
{
	send_ack();
	uint32_t chipid  = *CHIP_ID_ADDR & 0x7ff;
	printmsg("Chip ID = %x\n",chipid);
}
void print_supported_commands()
{
	send_ack();
	printmsg("BL_GET_VER\n");
	printmsg("BL_GET_HELP\n");
	printmsg("BL_GET_CID\n");

}
void erase_external_flash()

{
	uint32_t start = getTicks();
	Flash_ChipErase();
	uint32_t end =getTicks();
	Flash_Read(0x001000,&buffer_rx,16);
	uint8_t flag =1;
	for(int i=0;i<16;i++)
	{
		if(buffer_rx[i]==0xff)
		{
			flag=1;
		}
		else
		{
			flag=0;// fail
			send_nack();
		}
	}
	if(flag == 1)
	{
		send_ack();
		printmsg("Chip Erase successful\n");
	}
	else
	{
		printmsg("Chip erase failed\n");
	}
}
void print_BL_ver()
{
	uint8_t bl_version;
	// verify crc
    if(bootloader_verify_crc(parsed.command,parsed.length,parsed.payload,parsed.crc))
    {
      printmsg("Checksum matched\n");
      send_ack();
    }
    else
    {
    	printmsg("checksum failed");
    	send_nack();
    }

}
void write_external_flash()
{
	Flash_Page_Program(curr_address,parsed.payload,parsed.length);
	curr_address=curr_address+parsed.length;
	Flash_Read(0x000000,&buffer_rx, 256);
}


////   ***********************************************/////
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
		for(uint8_t i=0;i<4;i++)
		{
			ret_code = Queue_Dequeue(&q, &data);
			x->crc = (((x->crc) << 8) | data);
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
	crc_init();
	Queue_Init(&q);
	USART_GPIOInits();
	USART_Inits();
	USART_PeripheralControl(USART3,ENABLE);
	USART_PeripheralControl(USART2,ENABLE);
	SPI1_GPIOInits();
	flash_init();
	Flash_read_jedec();
	Flash_read_unique_id();

	Flash_Read(0x000000,&buffer_rx, 256);
	Flash_Write_Enable();
	sector_erase(0x000000);
	Flash_Read(0x000000,&buffer_rx, 256);
	//Flash_Page_Program(0x001000,&data_w,256);
	//Flash_Read(0x001000,&buffer_rx,16);


//	GPIO_IRQConfig(6,ENABLE);







	while(1)
	{
//		printmsg("In bootloader mode\n");
//		delayTicks(1000);// delay of 200 ms
		//if(GPIO_ReadInputPin(GPIOE,4) == 0)
		//{
			printmsg("Continuing into bootloader mode.. timeout set to 1 second\n");


			while(1)
			{
				parser(&parsed);
				switch(parsed.command)
				{
				case 0x51:
					print_BL_ver();
					//USART_SendData(&usart_init, &ack , 1);
					//printmsg("Bootloader Version 1.0\n");
					break;
				case 0x52:
					print_supported_commands();
					break;
				case 0x53:
					printchipid();
					break;
				 case BL_FLASH_ERASE:
                    erase_external_flash();
					break;
				 case BL_MEM_WRITE:
					 write_external_flash();
				default:
				break;
				}
				delayTicks(1000);

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
