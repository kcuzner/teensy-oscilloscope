/**
 * PWM Stuff
 */

#include "pwm.h"

#include "arm_cm4.h"

#define PWM_CnSC (FTM_CnSC_MSB_MASK | FTM_CnSC_ELSB_MASK)

void pwm_init(uint16_t mod)
{
    //enable clock
    SIM_SCGC6 |= SIM_SCGC6_FTM0_MASK;

    //quaden = 0
    FTM0_QDCTRL = 0;
    FTM0_SC = 0;

    //every n cycles, the timer will go off
    FTM0_CNTIN = 0;
    FTM0_CNT = 0;
    FTM0_MOD = mod;

    //no interrupts, clock source is system clock, prescaler is 32
    FTM0_SC = FTM_SC_CLKS(1) | FTM_SC_PS(2);
}

void pwm_set(uint8_t chan, uint16_t value)
{
    PORTC_PCR(chan + 1) = PORT_PCR_MUX(4);
    FTM0_CnSC(chan) = PWM_CnSC;
    FTM0_CnV(chan) = value;
}
