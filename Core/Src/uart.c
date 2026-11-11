/*
 * uart.c
 *
 *  Created on: Nov 11, 2026
 *      Author: user
 */
#include "stm32f4xx.h"
#include<stdint.h>

void UART2_Init(void)
{
	/*Enable the APB clock. For USART 2 */
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_USART2EN);

	/*Enable the AHB clock for GPIO. */
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

	/*Enable Alternative function for GPIO PA2, PA3 */
	GPIOA->MODER |= ((2U << (2*2)) | (2U << 2*3));

	/*Select AF7 (USART2) using GPIOA->AFRL*/
	GPIOA->AFR[0] &= ~((0x0F << (2*4)) | (0x0F << (3*4)));
	GPIOA->AFR[0] |= ((7U << (2*4)) | (7U << (3*4)));

	/*Set the Baud Rate using USART2->BRR.*/
	USART2->BRR = 0x0683;//((0x68 << 4) | 3U);

	/*Set the TE bit in USART_CR1 to send an idle frame
	 * as the first transmission.
	 */
	USART2->CR1 |= (1<<3);

	/*Set the RE bit in USART_CR1.*/
	USART2->CR1 |= (1<<2);

	/*Enable the USART by writing the UE bit in USART_CR1 register to 1*/
	USART2->CR1 |= (1<<13);
}

void UART2_Tx_Byte(uint8_t byte)
{
	while(!((USART2->SR >> 7) & 0x01));
	USART2->DR = byte;
}

uint8_t UART2_Rx_Byte(void)
{
	uint8_t RxByte;
	while(!((USART2->SR >> 5) & 0x01));
	RxByte = USART2->DR;
	return RxByte;
}

void UART2_Tx_Buffer(const char *buffer)
{
    while (*buffer)
    {
        UART2_Tx_Byte((uint8_t)*buffer);
        buffer++;
    }
}
