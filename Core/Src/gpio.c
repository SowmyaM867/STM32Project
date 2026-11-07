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

/**** PA5 configuration for LED  ****/
	/* Set mode (MODER) */
	GPIOA->MODER |= (1u << (5*2));

	/*Configure type (OTYPER)*/
	GPIOA->OTYPER &= ~(1u << 5);

	/* Set speed (OSPEEDR) */
	GPIOA->OSPEEDR &= ~(3u << (5*2));

	/*Configure pull-up/down (PUPDR)*/
	GPIOA->PUPDR &= ~(3u << (5*2));
/**** End of PA5 Configurations ****/
}

void GPIO_LED_ON_OFF(uint8_t val)
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

void Buzzer_Init(void)
{
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN);
    (void)RCC->AHB1ENR;

    GPIOB->MODER &= ~(3U << (0 * 2));
    GPIOB->MODER |=  (1U << (0 * 2));      // PB0 output
    GPIOB->OTYPER &= ~(1U << 0);           // push-pull
    GPIOB->PUPDR  &= ~(3U << (0 * 2));     // no pull

    GPIOB->BSRR = (1U << (0 + 16));        // start with buzzer off
}

void Buzzer_On(void)  { GPIOB->ODR |=  (1U << 0); }
void Buzzer_Off(void) { GPIOB->ODR &=  ~(1U << 0); }
