/*
 * timer.c
 *
 *  Created on: Nov 6, 2026
 *      Author: user
 */
#include "stm32f4xx.h"
#include<stdint.h>

void TIMER2_Init(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
	(void)RCC->APB1ENR;

	TIM2->PSC = 16 - 1;        // 16 MHz / 16 = 1 MHz -> 1 tick = 1 us
	TIM2->ARR = 1000 -1;    // TIM2 is 32-bit
	TIM2->EGR = TIM_EGR_UG;    // load prescaler
	TIM2->CR1 |= TIM_CR1_CEN;

}

void TIMER2_Delay_ms(uint32_t delay)
{
	if (delay == 0) return;

	TIM2->CR1 |= (1u << 0);        // bit 0 = CEN, start counter
	/*Each tick =1us, ARR is loaded with 1000
	 * So each in each iteration it generates 1ms delay
	 * when the count reaches 1000 it updates status register (SR), UIF will be set
	 * we have to clear and start SR and wait for the next overflow */
	for(int i=0;i<delay;i++){
		while (!(TIM2->SR & (1u << 0))) { }   // bit 0 = UIF, wait for overflow
		TIM2->SR = 0;                  // clear UIF
	}
}

void PWM_Init(void)
{
	/*Enable the APB clock. For Timer 3*/
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM3EN);

	/*Enable AHB clock. For GPIO Port A*/
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

	/*Configure GPIO PA6 as Alternate Function (GPIOA->MODER)*/
	GPIOA->MODER &= ~(3U << (6*2));
	GPIOA->MODER |= (2U << (6*2));

	/*Set the Alternate functionality to Timer 3. (GPIOA->AFR)*/
	GPIOA->AFR[0] &= ~(0xFu << (6 * 4));  // clear bits 27:24
	GPIOA->AFR[0] |=  (2u   << (6 * 4));  // AF2 = TIM3

	/* Configure Timer */
	TIM3->PSC = 16 - 1;      // 16 MHz / 16 = 1 MHz
	TIM3->ARR = 1000 - 1;    // 1 MHz / 1000 = 1 kHz PWM

	/* Set Duty Cycle */
	TIM3->CCR1 = 500;        // 50%

	/* Configure PWM Mode 1 */
	TIM3->CCMR1 &= ~(7 << 4);
	TIM3->CCMR1 |=  (6 << 4);

	/* Enable preload */
	TIM3->CCMR1 |= (1 << 3);

	/* Enable Channel Output */
	TIM3->CCER |= TIM_CCER_CC1E;

	/* Enable Auto-Reload Preload */
	TIM3->CR1 |= TIM_CR1_ARPE;
	TIM3->EGR = TIM_EGR_UG;
	/*Start Timer */
	TIM3->CR1 |= TIM_CR1_CEN;


}
void PWM_Motor_Speed_Control(uint32_t speed_percent)   // 0 to 100
{
	TIM3->CCR1 = speed_percent;
//    if (speed_percent > 100) speed_percent = 100;
//
//    TIM3->CCR1 = (speed_percent * 1000u) / 100u;      // = speed_percent * 10
}
