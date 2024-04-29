/*
 * uart.c
 *
 *  Created on: Apr 20, 2021
 *      Author: liama28
 */
#include "uart.h"
#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "driverlib/interrupt.h"
#include <stdio.h>
#include "objectDetection.h"
#include <ctype.h>

volatile char uartChar;

// Initialize UART for port B pin 0 and 1
void init_uart() {

    // Run Clock Gating Control set to UART module 1
    SYSCTL_RCGCUART_R |= SYSCTL_RCGC1_UART1;

    // Enable GPIO port B clock
    SYSCTL_RCGCGPIO_R |= 0b00000010;

    // Wait for ready bit
    while ((SYSCTL_PRGPIO_R & SYSCTL_PRGPIO_R1) == 0) {}; // SYSCTL_PRGPIO_R1 = 0x2
    while ((SYSCTL_PRUART_R & SYSCTL_PRUART_R1) == 0) {}; //SYSCTL_PRUART_R1 = 0x2

    // Digital Enable for pin 0 and 1
    GPIO_PORTB_DEN_R |= 0x3; // 0b 0011

    // Alternate Function Select for pin 0 and 1, (set for UART)
    GPIO_PORTB_AFSEL_R |= 0x3; // 0b 0011

    // Port Control / Enable UART1 Rx and Tx on port B pins
    GPIO_PORTB_PCTL_R &= 0xFFFFFF00;     // Force 0's in the desired locations
    GPIO_PORTB_PCTL_R |= 0x00000011;     // Force 1's in the desired locations

    // Turn off UARTEN bit in the UART Control register
    UART1_CTL_R &= ~UART_CTL_UARTEN;

    //calculate baud rate
    uint16_t iBRD = 0x8; //use equations
    uint16_t fBRD = 0x2C; //use equations

    // Integer portion of the baud-rate divisor (BRD) (8)
    UART1_IBRD_R = iBRD;
    // fractional portion of the baud-rate divisor (BRD) (0.6875 * 64 = 44)
    UART1_FBRD_R = fBRD;

    // set frame, 8 data bits, 1 stop bit, no parity, no FIFO
    // note: this write to LCRH must be after the BRD assignments
    UART1_LCRH_R = 0x60;

    // UART clock source
    // use system clock as source
    // note from the datasheet UARTCCC register description:
    // field is 0 (system clock) by default on reset
    UART1_CC_R |= 0x00;

    //======================INTERRUPTS=============================

    //first clear RX interrupt flag (clear by writing 1 to ICR)
    UART1_ICR_R |= 0b00010000;

    //enable RX raw interrupts in interrupt mask register
    UART1_IM_R |= 0b00010000;

    //NVIC setup: set priority of UART1 interrupt to 2 in bits 21-23
    NVIC_PRI1_R = (NVIC_PRI1_R & 0xFF1FFFFF) | 0x00400000;

    //NVIC setup: enable interrupt for UART1, IRQ #6, set bit 6
    NVIC_EN0_R |= 0b1000000;

    //tell CPU to use ISR handler for UART1 (see interrupt.h file)
    //from system header file: #define INT_UART1 22
    IntRegister(INT_UART1, UART1_Handler);

    //globally allow CPU to service interrupts (see interrupt.h file)
    IntMasterEnable();

    // re-enable UART1 and also enable RX, TX (three bits)
    // note from the datasheet UARTCTL register description:
    // RX and TX are enabled by default on reset
    // Good to be explicit in your code
    // Be careful to not clear RX and TX enable bits
    // (either preserve if already set or set them)
    UART1_CTL_R |= 0x301; //0b11 0000 0001;
}

// Function is called when UART receives a char
void UART1_Handler(void)
{
    //check if handler called due to RX event
    if (UART1_MIS_R & 0b10000) {
        //byte was received in the UART data register
        //clear the RX trigger flag (clear by writing 1 to ICR)
        UART1_ICR_R |= 0b00010000;

        //read the byte received from UART1_DR_R and echo it back to PuTTY
        //ignore the error bits in UART1_DR_R
        char byte_received = uart_receive_nonblocking();
        uart_sendChar(byte_received);
        uartChar = byte_received;
    }
}

// Sends a string through UART to putty
void uart_sendStr(const char *data)
{
    int i;
    for(i = 0; i < strlen(data); i++) {
        if(data[i] == '\n') {
        uart_sendChar('\n');
        uart_sendChar('\r');
        break;
        }
     uart_sendChar(data[i]);
    }
}

// Sends GUI Command help to putty through UART
void uart_GUI() {
  char buffer[60];
  sprintf(buffer, "%7s %9s\t%12s\n", "Number", "Command", "Function");
  uart_sendStr("\n");
  uart_sendStr("\n");
  uart_sendStr("==========================================\n");
  uart_sendStr("/////////// CyBot Command Help ///////////\n");
  uart_sendStr("==========================================\n");
  uart_sendStr("\n");
  uart_sendStr(buffer);
  uart_sendStr("------------------------------------------\n");
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 1, "g", "Go / Move");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 2, "f", "Scan");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 3, "q", "Quick Scan");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 4, "t", "Turn");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 5, "?", "Command Help");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 6, "a", "Turn Left 45");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 7, "d", "Turn Right 45");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 8, "x", "Turn Right 180");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 9, "s", "Move Back 10");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  sprintf(buffer, "%7d %9s\t%12s\n", 10, "w", "Move Forward 10");
  uart_sendStr(buffer);
  uart_sendStr("\n");
  uart_sendStr("========================================\n");
}

// Sends single char through UART to putty
void uart_sendChar(char data)
{
    while(!(UART1_FR_R & 0b10000000)) {}
    UART1_DR_R = data;
}

// Busy wait to receives a char
char uart_receive(void)
{
    while((UART1_FR_R & 0b10000))  {}
    char c = UART1_DR_R & 0b11111111;
    if(c == '\r') {
        uart_sendChar('\n');
        uart_sendChar('\r');
        return c;
        }
    uart_sendChar(c);
    return c;
    //DO NOT USE this busy-wait function if using RX interrupt
}

// Returns char in UART register
char uart_receive_nonblocking(void)
{
    return (UART1_DR_R & 0b11111111);

}

void toggleInterrupt(void) {
    if((UART1_IM_R & 0b00010000) == 0) {
        UART1_IM_R |= 0b00010000;
    }
    else {
        UART1_IM_R &= ~0b00010000;
    }
}
