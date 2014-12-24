/**
 * PWM Stuff
 */

#ifndef _PWM_H_
#define _PWM_H_

#include "arm_cm4.h"

void pwm_init(uint16_t mod);

void pwm_set(uint8_t chan, uint16_t value);

#endif // _PWM_H_
