/*
 * queue.c
 *
 *  Created on: 03-Apr-2026
 *      Author: vibhu
 */
#include <queue.h>

void Queue_Init(UART_Queue_t *q)
{
	q->count=0;
	q->head=0;
	q->tail=0;
}

volatile uint8_t Queue_Enqueue(UART_Queue_t *q, uint8_t data)
{
	uint8_t *a = q->buffer;
	if(q->count >= QUEUE_SIZE) // means queue has overflown
	{

		return(-1);
	}
	else
	{
		a[q->tail % QUEUE_SIZE] = data;
		q->tail=(q->tail+1)%QUEUE_SIZE;
		q->count++;
		return(1);
	}
}
int Queue_Dequeue(UART_Queue_t *q, uint8_t *data)
{
	uint8_t *a = q->buffer;
	if(q->count == 0)
	{
		return(-1);
	}
	else
	{
		*data =  a[q->head % QUEUE_SIZE];
        q->head = (q->head + 1) % QUEUE_SIZE;
        q->count--;
        return(0);
	}


}
uint8_t Queue_IsEmpty(UART_Queue_t *q)
{
	if (q->count == 0)
	{
		return(-1);
	}
	else
	{
		return(1);
	}
}

