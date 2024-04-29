/*
 * ADC.h
 *
 *  Created on: Mar 16, 2021
 *      Author: ianjohn, bwg
 */

#ifndef ADC_H_
#define ADC_H_

#include <stdio.h>
#include <stdint.h>
#include <inc/tm4c123gh6pm.h>

void adc_init(void);
int adc_read(void);

#endif /* ADC_H_ */
