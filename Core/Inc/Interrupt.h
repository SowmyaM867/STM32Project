/*
 * Interrupt.h
 *
 *  Created on: Nov 7, 2026
 *      Author: user
 */

#ifndef INC_INTERRUPT_H_
#define INC_INTERRUPT_H_
void INTERRUPT_Init(void);
extern volatile uint8_t button_flag;    // declaration only, no memory allocated
void EXTI15_10_IRQHandler(void);
#endif /* INC_INTERRUPT_H_ */
