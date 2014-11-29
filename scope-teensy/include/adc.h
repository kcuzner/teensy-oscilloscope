/**
 * Analog to digital converter stuff
 */

#ifndef _ADC_H_
#define _ADC_H_

#include "common.h"

/**
 * Initializes and calibrates the analog to digital module
 */
void adc_init(uint8_t bits);

void adc_set_chan0(uint8_t chan);

void adc_set_chan1(uint8_t chan);

#endif // _ADC_H_
