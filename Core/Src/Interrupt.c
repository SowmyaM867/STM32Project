/*
 * Interrupt.c
 *
 *  Created on: Nov 7, 2026
 *      Author: user
 */
#include "stm32f4xx.h"
#include<stdint.h>

volatile uint8_t button_flag = 0;

void INTERRUPT_Init(void)
{
// =========  EXTI (PB0 Interrupt) Configuration ==========   //
	/*Enable AHB1 clock for GPIO Port C */
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN);

/**** PC13 push button to detect interrupt ****/
	/*Configure PC13 as Input Mode.
	 * Set the corresponding bits in GPIOC->MODER to 00.
	 */
	GPIOC->MODER &= ~(3U << (2*13));

	/*Configure type (OTYPER)*/
	GPIOC->OTYPER &= ~(1u << 13);

	/* Set speed (OSPEEDR) */
	GPIOA->OSPEEDR &= ~(3u << (13*2));

	/*Configure pull-up/down (PUPDR)*/
	GPIOC->PUPDR &= ~(3u << (13 * 2));   // PC13 no pull
/**** END OF PC13 push button configuration ****/

	/*Enable APB2 clock for SYSCFG. SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN)*/
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);

	/*Map GPIO Port c pin 13 to EXTI Line 13.
	 * SYSCFG->EXTICR[3]. For PC13, set EXTI13 = 0010.*/
	SYSCFG->EXTICR[3] &= ~(0xFu << 4);
	SYSCFG->EXTICR[3] |=  (2u   << 4);      // 0010 = port C

	/*Configure EXTI Trigger Edge. EXTI->RTSR / EXTI->FTSR */
	EXTI->FTSR |=  (1U << 13);    // falling edge: ON
	EXTI->RTSR &= ~(1U << 13);    // rising edge: OFF

	/*Enable EXTI13 Interrupt */
	EXTI->IMR |= (1U << 13);     // enable (unmask) line 13

	/*Clear any Previous EXTI13 Pending Flag. EXTI->PR.
	 * Write 1 to PR13 to clear the pending condition.
	 */
	EXTI->PR = (1U << 13);       // clears bit 13 only

// ========= NVIC Configuration ==========   //
	/* 1. IRQ number: EXTI15_10_IRQn = 40 */

	/* 2. Priority = 5 (upper 4 bits of the 8-bit IP field) */
	NVIC->IP[40] = (5U << 4);

	/* 3. Enable IRQ 40: ISER[1], bit 8 (40 - 32 = 8) */
	NVIC->ISER[1] = (1U << 8);
}

void EXTI15_10_IRQHandler(void)
{
    if (EXTI->PR & (1U << 13))        // did line 13 trigger?
    {
        EXTI->PR = (1U << 13);        // clear the pending flag (write 1)
        button_flag = 1;              // tell main() a press happened
    }
}
