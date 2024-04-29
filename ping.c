/**
 * Driver for ping sensor
 * @file ping.c
 * @author ianjohn, bwg
 */

#include "ping.h"
#include "Timer.h"

volatile unsigned long START_TIME = 0;
volatile unsigned long END_TIME = 0;
volatile enum{LOW, HIGH, DONE} STATE = LOW; // State of ping echo pulse

void ping_init (void){

    // YOUR CODE HERE
    SYSCTL_RCGCGPIO_R |= 0x02;  // activate clock for PortB
    while((SYSCTL_PRGPIO_R & 0x02) != 0x02){};   //wait until ready
    SYSCTL_RCGCTIMER_R |= 0x08; // enable Timer 3
    while((SYSCTL_PRTIMER_R & 0x08) != 0x08){};   //wait until ready

    GPIO_PORTB_DIR_R |= 0x08;  // set Pin 3 as output
    GPIO_PORTB_DEN_R |= 0x08;  // enable Pin 3 as output
    GPIO_PORTB_AFSEL_R &= ~0x08; // disable alternate function on Pin 3
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0xF000) | 0x7000; // change encoding on Pin 3 for later

    TIMER3_CTL_R &= ~0x0100; // disable Timer 3 B
    TIMER3_CFG_R = 0x4;     // chooses 16 bit timer mode
    TIMER3_TBMR_R |= 0x3;    // sets TIMER 3B to edge time, capture mode
    TIMER3_TBMR_R |= 0x4;    // sets TIMER 3B to edge time
    TIMER3_TBMR_R &= ~0x0010;// sets count direction to down
    TIMER3_CTL_R |= 0x0C00;  // sets Timer 3 B to both edge capture &= ~0x0C00; sets positive edge
    TIMER3_TBILR_R |= 0xFFFF;// initialize timer with FFFF
    TIMER3_TBPR_R |= 0xFF;   // activates the Pre-scale, adds 8 bits to the timer
    TIMER3_ICR_R |= 0x0400;  // clears interrupt Capture Mode Event Interrupt
	TIMER3_MIS_R |= 0x0400;  //`Timer B Capture Mode Event Masked Interrupt
    TIMER3_IMR_R |= 0x0400;  // enables Timer Capture Mode Event Interrupt Mask


    NVIC_EN1_R |= 0x10;      //enable interrupts bit 36 for Timer 3B 
	NVIC_PRI9_R &= ~0x1;     //priority 1
	NVIC_PRI9_R |= 0x1;      //priority 1

    IntRegister(INT_TIMER3B, TIMER3B_Handler);

    IntMasterEnable();

    // Configure and enable the timer
    TIMER3_CTL_R |= 0x0100; // enable Timer 3 B

}

void ping_trigger (void){
    STATE = LOW;
    // Disable timer and disable timer interrupt
    TIMER3_CTL_R &= ~0x0100;  // disable Timer 3 B
    TIMER3_IMR_R &= ~0x0400;  // disables Timer Capture Mode Event Interrupt Mask

    // Disable alternate function (disconnect timer from port pin)
    //GPIO_PORTB_PCTL_R &= ~0x7000; // change encoding on Pin 3
    GPIO_PORTB_AFSEL_R &= ~0x08; // disable alternate function on Pin 3

    // YOUR CODE HERE FOR PING TRIGGER/START PULSE

    GPIO_PORTB_DIR_R |= 0x08;  // set Pin 3 as output
    GPIO_PORTB_DATA_R &= ~0x08; // pulls PB3 low
    GPIO_PORTB_DATA_R |= 0x08; // pulls PB3 high
    timer_waitMicros(5); // wait 5 Micro seconds
    GPIO_PORTB_DATA_R &= ~0x08; // pulls PB3 low
    GPIO_PORTB_DIR_R &= ~0x08;  // set Pin 3 as input

    // Clear an interrupt that may have been erroneously triggered
    TIMER3_ICR_R |= 0x0400;  // clears interrupt Capture Mode Event Interrupt

    // Re-enable alternate function, timer interrupt, and timer
    GPIO_PORTB_AFSEL_R |= 0x08; // enable alternate function on Pin 3
    //GPIO_PORTB_PCTL_R |= 0x7000; // change encoding on Pin 3

    TIMER3_IMR_R |= 0x0400;  // enables Timer Capture Mode Event Interrupt Mask
    TIMER3_CTL_R |= 0x0100;  // enable Timer 3 B
}

void TIMER3B_Handler(void){

  // YOUR CODE HERE

    
    if(STATE == LOW){
        STATE = HIGH;
        START_TIME = TIMER3_TBR_R; //sets start time
    }else if(STATE == HIGH){
        STATE = DONE;
        END_TIME = TIMER3_TBR_R; //sets end time
    }
	TIMER3_ICR_R |= 0x0400;  // clears interrupt Capture Mode Event Interrupt
}

float ping_getDistance (void){

    // YOUR CODE HERE

    ping_trigger ();
    while(STATE != DONE){};
    //.0000625
    //return (float) ((((START_TIME - END_TIME) *.0000625) *.1 * (343)) / 2);
    float time = (START_TIME - END_TIME);
        if(time < 0){
            time = (START_TIME +(0xFFFFFF - END_TIME));
        }
    return (((time *.0000625) *.1 * (343)) / 2);
    //return (float) (START_TIME - END_TIME);
}
