/*
 * gpio.c
 *
 *  Created on: Nov 6, 2026
 *      Author: user
 */
#include "stm32f4xx.h"
#include<stdint.h>
#include"gpio.h"
void GPIO_Init(void)
{
	/* Enable GPIO clock (RCC) */
	SET_BIT(RCC->AHB1ENR,RCC_AHB1ENR_GPIOAEN);

	/* Set mode (MODER) */
	GPIOA->MODER |= (1u << (5*2));

	/*Configure type (OTYPER)*/
	GPIOA->OTYPER &= ~(1u << 5);

	/* Set speed (OSPEEDR) */
	GPIOA->OSPEEDR &= ~(3u << (5*2));

	/*Configure pull-up/down (PUPDR)*/
	GPIOA->PUPDR &= ~(3u << (5*2));
}

void GPIO_LED_Blinking(uint8_t val)
{
	if(val == 0)
	{
		GPIOA->ODR &=  ~(1U << 5);
	}
	else if(val == 1)
	{
		GPIOA->ODR |=  (1U << 5);
	}
}
