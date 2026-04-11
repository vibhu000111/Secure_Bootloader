/*
 * queue.h
 *
 *  Created on: 03-Apr-2026
 *      Author: vibhu
 */

#ifndef INC_QUEUE_H_
#define INC_QUEUE_H_

#include <stm32f407xx.h>
#define QUEUE_SIZE 256

typedef struct {
    uint8_t buffer[QUEUE_SIZE];
    volatile uint32_t head;    // Read index
    volatile uint32_t tail;    // Write index
    volatile uint32_t count;   // Track number of bytes for easy checks
} UART_Queue_t;

/**
 * @brief Initializes the queue pointers and count.
 * @param q Pointer to the queue structure.
 */
void Queue_Init(UART_Queue_t *q);

/**
 * @brief Adds a byte to the tail of the queue.
 * @note  Call this inside your UART_IRQHandler.
 * @return true if successful, false if queue is full (overflow).
 */
volatile uint8_t Queue_Enqueue(UART_Queue_t *q, uint8_t data);

/**
 * @brief Removes and returns the byte at the head of the queue.
 * @note  Call this in your main loop or bootloader logic.
 * @param data Pointer to store the dequeued byte.
 * @return true if successful, false if queue is empty.
 */
uint8_t Queue_Dequeue(UART_Queue_t *q, uint8_t *data);

/**
 * @brief Returns the byte at the head without removing it.
 * @note  Useful for checking headers/start bytes in bootloader protocols.
 */
uint8_t Queue_Peek(UART_Queue_t *q, uint8_t *data);

/**
 * @brief Checks if the queue has no data.
 */
uint8_t Queue_IsEmpty(UART_Queue_t *q);

/**
 * @brief Checks if the queue is at max capacity.
 */
uint8_t Queue_IsFull(UART_Queue_t *q);

/**
 * @brief Resets the queue (flushes all data).
 */
void Queue_Flush(UART_Queue_t *q);
#endif /* INC_QUEUE_H_ */
