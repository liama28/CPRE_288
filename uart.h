/*
 * uart.h
 *
 *  Created on: Apr 20, 2021
 *      Author: liama28
 */

#ifndef UART_H_
#define UART_H_

#include <inc/tm4c123gh6pm.h>
#include <stdint.h>

extern volatile char uartChar;

// Initialize UART for port B pin 0 and 1
void init_uart();

// Function is called when UART receives a char
void UART1_Handler(void);

// Sends a string through UART to putty
void uart_sendStr(const char *data);

// Sends GUI Command help to putty through UART
void uart_GUI();

// Sends single char through UART to putty
void uart_sendChar(char data);

// Busy wait to receives a char
char uart_receive(void);

// Returns char in UART register
char uart_receive_nonblocking(void);

void toggleInterrupt(void);


#endif /* UART_H_ */
