/*
 * adc.c
 *
 *  Created on: Nov 6, 2026
 *      Author: user
 */
#include "stm32f4xx.h"
#include<stdint.h>
#include"adc.h"
#define ADC_MAX_VALUE      4095.0f

void ADC_Init(void)
{
	/*Enable AHB1 clock for GPIO Port A. For GPIO Port A*/
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);

	/*Enable APB2 clock for ADC1*/
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN);

	/*Configure GPIO as Analog Mode.
	 * For the ADC input pin PA0, PA1 set the respective bits in GPIOA->MODER to 11 */
	GPIOA->MODER &= ~((3U << (2*0)) | ((3U << (2*1))));
	GPIOA->MODER |= ((3U << (2*0)) | ((3U << (2*1))));

	/* Disable GPIO Pull-up / Pull-down. GPIOA->PUPDR */
	GPIOA->PUPDR &= ~((3U << (2*0)) | ((3U << (2*1))));

	/*Configure ADC Clock Prescaler. ADC->CCR */
	ADC->CCR &= ~(3U << 16);

	/*Configure ADC Resolution. ADC1->CR1.
	 * Set RES = 00 for 12-bit resolution.
	 */
	ADC1->CR1 &= ~(3U << 24);

	/*Configure Conversion Sequence Length.
	 * ADC1->SQR1. Set the L field to define the number of regular conversions.
	 * Set L = 0001 for 2 conversions.
	 */
	ADC1->SQR1 &= ~(0x0F << 20);
	ADC1->SQR1 |= (1U << 20);

#if 0
	/*Select ADC Channel Sequence. ADC1->SQR3. For PA0: SQ1 = 0*/
	ADC1->SQR3 &= ~(0x1F << 0);

	/*Select ADC Channel Sequence. ADC1->SQR3. For PA1: SQ1 = 1*/
	ADC1->SQR3 &= ~(0x1F << 5);
	ADC1->SQR3 |= (1U << 5);
#endif
	/*Configure ADC Sampling Time. ADC1->SMPR2.
	 * For Channel 0: SMP0 = 111 → 480 cycles.
	 */
	 ADC1->SMPR2 &= ~((7U << (3*0)) | (7U << (3*1)));
	 ADC1->SMPR2 |= ((7U << (3*0)) | (7U << (3*1)));

	 /*Configure Continuous Conversion Mode. ADC1->CR2.
	  * Clear CONT for single conversion.
	  */
	 ADC1->CR2 &= ~(1U << 1);

	 /*Configure External Trigger/Software Trigger.
	  * ADC1->CR2. Disable external trigger using EXTEN = 00 so that conversion
	  * can be started using the software trigger.
	  */
	 ADC1->CR2 &= ~(3U << 28);

	 /*Configure Data Alignment. ADC1->CR2.
	  * Set ALIGN = 0 for right-aligned ADC data.
	  */
	 ADC1->CR2 &= ~(1U << 11);

	 /*Configure End Of Conversion (EOC). ADC1->CR2.
	  * Configure EOCS according to whether EOC is required after each conversion or
	  * at the end of a regular sequence.
	  */
	 ADC1->CR2 |= (1U << 10);

	 /*Clear ADC Status Flags. ADC1->SR = 0*/
	 ADC1->SR = 0;

	 /*Enable ADC Peripheral. SET_BIT(ADC1->CR2, ADC_CR2_ADON) */
	 SET_BIT(ADC1->CR2, ADC_CR2_ADON);
}

uint16_t ADC_Read(uint8_t channel)
{
    uint16_t adc_value;


    /* Only channels 0-7 are supported by this GPIO configuration */
    if (channel > 7U)
    {
        return 0U;
    }


    /*
     * IMPORTANT:
     *
     * This generic driver currently assumes that the required analog GPIO
     * has already been configured correctly.
     *
     * In your current project:
     *
     * PA0 = ADC1_IN0 = LM35D
     * PA1 = ADC1_IN1 = LDR
     */


    /* Select channel */
    ADC1->SQR3 &= ~(0x1FU << 0);
    ADC1->SQR3 |= ((uint32_t)channel << 0);


    /* Clear previous flags */
    ADC1->SR = 0U;


    /* Start conversion */
    ADC1->CR2 |= ADC_CR2_SWSTART;


    /* Wait for conversion complete */
    while ((ADC1->SR & ADC_SR_EOC) == 0U)
    {
        /* Wait */
    }


    /* Read ADC result */
    adc_value = (uint16_t)ADC1->DR;


    return adc_value;
}


/* ==========================================================================
 * ADC_ReadVoltage
 * ========================================================================== */

float ADC_ReadVoltage(uint8_t channel, float vref)
{
    uint16_t adc_value;
    float voltage;


    adc_value = ADC_Read(channel);

    voltage = ((float)adc_value * vref) / ADC_MAX_VALUE;


    return voltage;
}

uint8_t ADC_ReadLDRValue(uint8_t channel)
{
	uint16_t ldr;
	uint8_t light_percent;

	ldr = ADC_Read(channel);
	light_percent = (ldr * 100u) / 4095u;   // 0 = dark, 100 = brightest

	return light_percent;
}
