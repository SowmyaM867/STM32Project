/*
 * adc.h
 *
 *  Created on: Nov 6, 2026
 *      Author: user
 */

#ifndef INC_ADC_H_
#define INC_ADC_H_

void ADC_Init(void);
uint16_t ADC_Read(uint8_t channel);
float ADC_ReadVoltage(uint8_t channel, float vref);
uint8_t ADC_ReadLDRValue(uint8_t channel);
#endif /* INC_ADC_H_ */
