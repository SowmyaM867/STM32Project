/*
 * timer.h
 *
 *  Created on: Nov 6, 2026
 *      Author: user
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

void TIMER2_Init(void);
void TIMER2_Delay_ms(uint32_t);
void PWM_Init(void);
void PWM_Motor_Speed_Control(uint32_t);
#endif /* INC_TIMER_H_ */
