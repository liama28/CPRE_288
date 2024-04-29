/*
 * ACD.c
 *
 *  Created on: Mar 16, 2021
 *      Author: ianjohn, bwg
 */
#include "ADC.h"

void adc_init(void){
    SYSCTL_RCGCADC_R |= 0x0001; // activates ADC0
    while((SYSCTL_PRADC_R & 0x0001) != 0x001){}; // wait until ADC0 is ready
    SYSCTL_RCGCGPIO_R |= 0x02;  // activate clock for PortB
    while((SYSCTL_PRGPIO_R & 0x02) != 0x02){};   //wait until ready
    GPIO_PORTB_DIR_R &= ~0x10;  // set Pin 4 as input
    GPIO_PORTB_AFSEL_R |= 0x10; // enable alternate function on Pin 4
    GPIO_PORTB_DEN_R &= ~0x10;  // disable digital I/O pin 4
    GPIO_PORTB_AMSEL_R |= 0x10; // enable analog mode Pin 4
    //?
    ADC0_PC_R &= ~0xF;          // sets sample rate
    ADC0_PC_R |= 0x1;           //sets sample rate to 125k samples / second
    ADC0_ACTSS_R &= ~0x0008;    // disables sample sequencer 3
    ADC0_EMUX_R &= ~0xF000;     // seq3 is software trigger
    ADC0_SSMUX3_R &= ~0x000F;
    ADC0_SSMUX3_R += 10;        // sets channel
    ADC0_SSCTL3_R = 0x006;      // sets IE0 and END0
    ADC0_IM_R &= ~0x0008;       // disables SS3 Interrupts
    ADC0_ACTSS_R |= 0x0008;     // enables sample sequencer 3
}

int adc_read(void){
    ADC0_PSSI_R = 0x0008;       // initiates SS3
    while((ADC0_RIS_R & 0x08) == 0){};     // waits until the conversion to be done
    int result = (ADC0_SSFIFO3_R & 0xFFF); // sets the result to the 12 bits of ADC
    ADC0_ISC_R = 0x0008;        // ADC interrupt status clear, reset flag
    return(result);
}
