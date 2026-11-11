/*
 * uart.h
 *
 *  Created on: Nov 11, 2026
 *      Author: user
 */

#ifndef INC_UART_H_
#define INC_UART_H_
void UART2_Init(void);
void UART2_Tx_Byte(uint8_t byte);
uint8_t UART2_Rx_Byte(void);
void UART2_Tx_Buffer(char *buffer);
#endif /* INC_UART_H_ */
