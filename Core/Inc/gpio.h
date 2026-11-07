/*
 * gpio.h
 *
 *  Created on: Nov 6, 2026
 *      Author: user
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_

void GPIO_Init(void);
void GPIO_LED_Blinking(uint8_t val);
void Buzzer_Init(void);
void Buzzer_On(void);
void Buzzer_Off(void);
#endif /* INC_GPIO_H_ */
