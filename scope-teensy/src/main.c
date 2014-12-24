#include "common.h"
#include "arm_cm4.h"
#include "adc.h"
#include "usb.h"
#include "pwm.h"

#define LED_ON  GPIOC_PSOR=(1<<5)
#define LED_OFF GPIOC_PCOR=(1<<5)
#define LED2_ON  GPIOC_PSOR=(1<<7)
#define LED2_OFF GPIOC_PCOR=(1<<7)


int main(void)
{
    volatile uint32_t n;
    uint32_t          v;
    uint32_t s;
    uint8_t           mask;

    PORTC_PCR5 = PORT_PCR_MUX(0x1); // LED is on PC5 (pin 13), config as GPIO (alt = 1)
    PORTC_PCR7 = PORT_PCR_MUX(0x1); // LED2 is on PC7 (pin 12), config as GPIO (alt = 1)
    GPIOC_PDDR = (1<<5) | (1<<7);            // make this an output pin
    LED_OFF;                        // start with LED off
    LED2_OFF;

    v = (uint32_t)mcg_clk_hz;
    v = v / 1000000;

    s = (uint32_t)mcg_clk_hz / 16;

    //enable the PIT clock
    SIM_SCGC3 |= SIM_SCGC3_ADC1_MASK;
    SIM_SCGC6 |= SIM_SCGC6_PIT_MASK | SIM_SCGC6_ADC0_MASK;

    // turn on PIT
    PIT_MCR = 0x00;
    // Timer 1
    PIT_LDVAL1 = 0x0003E7FF; // setup timer 1 for 256000 cycles
    PIT_TCTRL1 = PIT_TCTRL_TIE_MASK; // enable Timer 1 interrupts
    PIT_TCTRL1 |= PIT_TCTRL_TEN_MASK; // start Timer 1

    adc_init(8);
    usb_init();
    pwm_init(1000);
    pwm_set(0, 500);

    enable_irq(IRQ(INT_PIT1));
    EnableInterrupts

    while(1)
    {
        LED2_ON;
        for (n = 0; n < s; n++);
        LED2_OFF;
        for (n = 0; n < s; n++);
    }

    return  0;                        // should never get here!
}

//void PIT1_IRQHandler() __attribute__ ((interrupt ("IRQ")));
void PIT1_IRQHandler()
{
    static uint8_t count = 0;
    static uint8_t stat = 0;
    if (count == 30)
    {
        /*if (stat)
            LED_ON;
        if (!stat)
            LED_OFF;*/
        stat ^= 0x01;
        count = 0;
    }
    else
    {
        count++;
    }
    //reset the interrupt flag
    PIT_TFLG1 |= PIT_TFLG_TIF_MASK;
}
