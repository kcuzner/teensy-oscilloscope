#include  "common.h"

#define  LED_ON        GPIOC_PSOR=(1<<5)
#define  LED_OFF    GPIOC_PCOR=(1<<5)


int  main(void)
{
    volatile uint32_t            n;
    uint32_t                    v;
    uint8_t                        mask;

    PORTC_PCR5 = PORT_PCR_MUX(0x1); // LED is on PC5 (pin 13), config as GPIO (alt = 1)
    GPIOC_PDDR = (1<<5);            // make this an output pin
    LED_OFF;                        // start with LED off

    v = (uint32_t)mcg_clk_hz;
    v = v / 1000000;

    if (v == 0x48)
        LED_ON;

    while(1);

    while (1)
    {
        for (n=0; n<10000000; n++)  ;    // dumb delay
        mask = 0x80;
        while (mask != 0)
        {
            LED_ON;
            for (n=0; n<1000000; n++)  ;        // base delay
            if ((v & mask) == 0)  LED_OFF;    // for 0 bit, all done
            for (n=0; n<2000000; n++)  ;        // (for 1 bit, LED is still on)
            LED_OFF;
            for (n=0; n<1000000; n++)  ;
            mask = mask >> 1;
        }
    }

    return  0;                        // should never get here!
}
