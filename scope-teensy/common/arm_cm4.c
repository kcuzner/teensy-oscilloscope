/*
 * File:		arm_cm4.c
 * Purpose:		Generic high-level routines for ARM Cortex M4 processors
 *
 * Notes:
 * Modified 28 Apr 14  KEL
 * Changed to handle the K20 MCU, which support 94 IRQs (110 vectors).
 * Removed #ifdefs for the Keil toolchain; this assumes GCC.
 * Removed use of printf() for error output; where possible, errors
 * are silently ignored.
 */

#include "common.h"

/***********************************************************************/
/*
 * Configures the ARM system control register for STOP (deep sleep) mode
 * and then executes the WFI instruction to enter the mode.
 *
 * Parameters:
 * none
 *
 * Note: Might want to change this later to allow for passing in a parameter
 *       to optionally set the sleep on exit bit.
 */

void stop (void)
{
	/* Set the SLEEPDEEP bit to enable deep sleep mode (STOP) */
	SCB_SCR |= SCB_SCR_SLEEPDEEP_MASK;	

	/* WFI instruction will start entry into STOP mode */
	asm("WFI");
}
/***********************************************************************/
/*
 * Configures the ARM system control register for WAIT (sleep) mode
 * and then executes the WFI instruction to enter the mode.
 *
 * Parameters:
 * none
 *
 * Note: Might want to change this later to allow for passing in a parameter
 *       to optionally set the sleep on exit bit.
 */

void wait (void)
{
	/* Clear the SLEEPDEEP bit to make sure we go into WAIT (sleep) mode instead
	 * of deep sleep.
	 */
	SCB_SCR &= ~SCB_SCR_SLEEPDEEP_MASK;	

	/* WFI instruction will start entry into WAIT mode */
	asm("WFI");
}
/***********************************************************************/
/*
 * Change the value of the vector table offset register to the specified value.
 *
 * Parameters:
 * vtor     new value to write to the VTOR
 */

void write_vtor (int vtor)
{
        /* Write the VTOR with the new value */
        SCB_VTOR = vtor;	
}
/***********************************************************************/
/*
 * Initialize the NVIC to enable the specified IRQ.
 * 
 * NOTE: The function only initializes the NVIC to enable a single IRQ. 
 * Interrupts will also need to be enabled in the ARM core. This can be 
 * done using the EnableInterrupts macro.
 *
 * Parameters:
 * irq    irq number to be enabled (the irq number NOT the vector number)
 */

void enable_irq (int irq)
{
    int div;
    
    /* Make sure that the IRQ is an allowable number. Right now up to 110 is 
     * used.
     */
    if (irq > 110)				// if requested IRQ is out of range...
		return;
    
    /* Determine which of the NVICISERs corresponds to the irq */
    div = irq/32;
    
    switch (div)
    {
    	case 0x0:
              NVICICPR0 = 1 << (irq%32);
              NVICISER0 = 1 << (irq%32);
              break;
    	case 0x1:
              NVICICPR1 = 1 << (irq%32);
              NVICISER1 = 1 << (irq%32);
              break;
    	case 0x2:
              NVICICPR2 = 1 << (irq%32);
              NVICISER2 = 1 << (irq%32);
              break;
    	case 0x3:
              NVICICPR3 = 1 << (irq%32);
              NVICISER3 = 1 << (irq%32);
              break;
    }              
}
/***********************************************************************/
/*
 * Initialize the NVIC to disable the specified IRQ.
 * 
 * NOTE: The function only initializes the NVIC to disable a single IRQ. 
 * If you want to disable all interrupts, then use the DisableInterrupts
 * macro instead. 
 *
 * Parameters:
 * irq    irq number to be disabled (the irq number NOT the vector number)
 */

void disable_irq (int irq)
{
    int div;
    
    /* Make sure that the IRQ is an allowable number. Right now up to 110 is 
     * used.
     */
    if (irq > 110)						// if IRQ is outside legal range...
		return;

    /* Determine which of the NVICICERs corresponds to the irq */
    div = irq/32;
    
    switch (div)
    {
    	case 0x0:
               NVICICER0 = 1 << (irq%32);
              break;
    	case 0x1:
              NVICICER1 = 1 << (irq%32);
              break;
    	case 0x2:
              NVICICER2 = 1 << (irq%32);
			  break;
    	case 0x3:
              NVICICER3 = 1 << (irq%32);
              break;
    }              
}

#if 0
/***********************************************************************/
/*
 * Initialize the NVIC to set specified IRQ priority.
 * 
 * NOTE: The function only initializes the NVIC to set a single IRQ priority. 
 * Interrupts will also need to be enabled in the ARM core. This can be 
 * done using the EnableInterrupts macro.
 *
 * Parameters:
 * irq    irq number to be enabled (the irq number NOT the vector number)
 * prio   irq priority. 0-15 levels. 0 max priority
 */

void set_irq_priority (int irq, int prio)
{
    /*irq priority pointer*/
    uint8_t				*prio_reg;
    
    /* Make sure that the IRQ is an allowable number. Right now up to 110 is 
     * used.
     */
    if (irq > 110)					// if IRQ is outside legal range...
		return;

    if (prio > 15)
        return;

/*
 *  NOTE: The following code is not correct for K20.  Mod before use!
 */
    /* Determine which of the NVICIPx corresponds to the irq */
    prio_reg = (uint8_t *)(((uint32_t)&NVICIP0) + irq);
    /* Assign priority to IRQ */
    *prio_reg = ( (prio&0xF) << (8 - ARM_INTERRUPT_LEVEL_BITS) );             
}
/***********************************************************************/
#endif


