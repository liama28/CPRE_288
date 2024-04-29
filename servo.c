/*
 * servo.c
 *
 *  Created on: Mar 30, 2021
 *      Author: ianjohn, bwg
 */
#include "lcd.h"
#include "Timer.h"
#include "servo.h"

void servo_init(void){
    SYSCTL_RCGCGPIO_R |= 0x02;    // activate clock for PortB
    while((SYSCTL_PRGPIO_R & 0x02) != 0x02){};   //wait until ready

    GPIO_PORTB_AFSEL_R |= 0x20;   // enable alternate function on Pin 5
    GPIO_PORTB_DIR_R |= 0x20;     // set Pin 5 as output
    GPIO_PORTB_DEN_R |= 0x20;     // enable Pin 5
    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & ~0xF00000) | 0x700000; // change encoding on Pin 5 for later

    SYSCTL_RCGCTIMER_R |= 0x02;   // enable Timer 1
    while((SYSCTL_PRTIMER_R & 0x02) != 0x02){};   //wait until ready

    TIMER1_CTL_R &= ~0x0100; // disable Timer 1 B
    TIMER1_CFG_R = 0x4;      // chooses 16 bit timer mode
    TIMER1_TBMR_R |= 0x8;    // sets TIMER 1B to PWM mode
    TIMER1_TBMR_R &= ~0x4;   // sets TIMER 1B to edge count
    TIMER1_TBMR_R |= 0x2;    // sets periodic timer mode

    TIMER1_TBILR_R = (0x4E200 & 0xFFFF); // initialize timer with max cycles 160000
    TIMER1_TBPR_R = (0x4E200 >> 16) & 0xFF;   // activates the Pre-scale, adds 8 bits to the timer

    TIMER1_TBMATCHR_R = ((0x4E200 - 20375) & 0xFFFF); // value timer needs to match
    TIMER1_TBPMR_R = ((0x4E200 - 20375) >> 16) & 0xFF; // set Pre-scale match

    TIMER1_CTL_R |= 0x0100;    // enable Timer 1 B

}


int servo_move(float degrees){

    if(degrees <= 0){
        degrees = 0;
    }else if(degrees >= 180){
       degrees = 180;
    }

    //determine PWM period
    int period = 0x4E200;

    int matchVal = period - ((126.388889 * degrees) + 9000);

    TIMER1_TBMATCHR_R = matchVal & 0xFFFF; // value timer needs to match
    TIMER1_TBPMR_R = (matchVal >> 16) & 0xFF;  // set Pre-scale match to 0

    return matchVal;
}

