/*
 *  File:        sysinit.c
 *  Purpose:     Kinetis Configuration
 *               Initializes processor to a default state
 *
 *  Notes:
 *  This version of sysinit.c contains a modified version of the
 *  original pll_init() function, originally found in the mgc.c
 *  source file.  I moved that PLL startup code here and recoded
 *  it to assume use of a Teensy 3.x board.  8 Apr 14   KEL
 *
 */

#include "common.h"
#include "sysinit.h"
#include "uart.h"

/*
 *  Actual system clock frequencies, as determined by PLL following lock
 *
 *  Declare these variables as extern in other modules if you need access
 *  to the actual system clock frequencies.
 */
int32_t			mcg_clk_hz;
int32_t			mcg_clk_khz;
int32_t			core_clk_khz;
int32_t			periph_clk_khz;



/*
 *  start()      initial entry point from the C run-time routine (crt0.s)
 *
 *  This is the entry point following reset and low-level initialzation.
 *  This routine is responsible for system initialization and for
 *  invoking main().
 *
 *  In the original Freescale Code Warrior example code, this routine
 *  lived in start.c.  I've moved it here so that a custom Teensy 3.1
 *  project can consist of just three files; crt0.s, the main project
 *  file, and this file.   8 Apr 14  KEL
 */
void start(void)
{
/*
 * Enable all of the port clocks. These have to be enabled to configure
 * pin muxing options, so most code will need all of these on anyway.
 */
	sysinit();			// Perform processor initialization
	main();				// run the main program

	while (1)   ;		// control should never get here!
}



/********************************************************************/
void sysinit (void)
{
/*
 * Enable all of the port clocks. These have to be enabled to configure
 * pin muxing options, so most code will need all of these on anyway.
 */
	SIM_SCGC5 |= (SIM_SCGC5_PORTA_MASK
              | SIM_SCGC5_PORTB_MASK
              | SIM_SCGC5_PORTC_MASK
              | SIM_SCGC5_PORTD_MASK
              | SIM_SCGC5_PORTE_MASK );

/* Ramp up the system clock
 * Set the system dividers
 * NOTE: The PLL init will not configure the system clock dividers,
 * so they must be configured appropriately before calling the PLL
 * init function to ensure that clocks remain in valid ranges.
 */  
    SIM_CLKDIV1 = ( 0
                    | SIM_CLKDIV1_OUTDIV1(0)
                    | SIM_CLKDIV1_OUTDIV2(0)
                    | SIM_CLKDIV1_OUTDIV4(1) );

/* releases hold with ACKISO:  Only has an effect if recovering from VLLS1, VLLS2, or VLLS3
 * if ACKISO is set you must clear ackiso before calling pll_init 
 * or pll init hangs waiting for OSC to initialize.
 * if osc enabled in low power modes - enable it first before ack.
 * if I/O needs to be maintained without glitches enable outputs and modules first before ack.
 */
    if (PMC_REGSC &  PMC_REGSC_ACKISO_MASK)
        PMC_REGSC |= PMC_REGSC_ACKISO_MASK;

/* Initialize PLL
 * PLL will be the source for MCG CLKOUT so the core, system, and flash clocks
 * are derived from it.
 */
	mcg_clk_hz = pll_init(PRDIV_VAL, VDIV_VAL);	// Use the output from this PLL as the MCGOUT

/* Check the value returned from pll_init() to make sure there wasn't an error.
 */
	if (mcg_clk_hz < 0x100)
	{
		while(1);
	}

/*
 * Use the value obtained from the pll_init function to define variables
 * for the core clock in kHz and also the peripheral clock. These
 * variables can be used by other functions that need awareness of the
 * system frequency.
 */
	mcg_clk_khz = mcg_clk_hz / 1000;
	core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
  	periph_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV2_MASK) >> 24)+ 1);
        
/*
 *  For debugging purposes, enable the trace clock and/or FB_CLK so that
 *  we'll be able to monitor clocks and know the PLL is at the frequency
 *  that we expect.
 */
	//fb_clk_init();
	//trace_clk_init();
}


#if 0
/********************************************************************/
void trace_clk_init(void)
{
	/* Set the trace clock to the core clock frequency */
	SIM_SOPT2 |= SIM_SOPT2_TRACECLKSEL_MASK;

	/* Enable the TRACE_CLKOUT pin function on PTA6 (alt7 function) */
	PORTA_PCR6 = ( PORT_PCR_MUX(0x7));
}
/********************************************************************/
void fb_clk_init(void)
{
 	/* Enable the FB_CLKOUT function on PTC3 (alt5 function) */
    SIM_SOPT2 &= ~SIM_SOPT2_CLKOUTSEL_MASK; // clear clkoout field
    SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(2);    // select flash clock
	PORTC_PCR3 = ( PORT_PCR_MUX(0x5) | PORT_PCR_DSE_MASK );
}
/********************************************************************/
#endif


/*********************************************************************************************/
/* Functon name : pll_init
 *
 *  NOTE:  This code was heavily modified from the original supplied in the Freescale
 *  K20 example set (kinetis_50MHz_sc) as mgc.c.  This code is intended to run solely
 *  on the Teensy 3.x board, so the PLL initialization code makes assumptions about
 *  the hardware.  Specifically, the Teensy 3.x boards use a 16 MHz crystal, so there
 *  is no need for tests of the external oscillator value, originally passed in argument
 *  crystal_val.  8 Apr 14  KEL
 *
 *  ------------------------------------------------------------------------------------
 *
 * This function initializess either PLL0 or PLL1. Either OSC0 or OSC1 can be selected for the
 * reference clock source. The oscillators can be configured to use a crystal or take in an
 * external square wave clock.
 * NOTE : This driver does not presently (as of Sept 9 2011) support the use of OSC1 as the
 * reference clock for the MCGOUT clock used for the system clocks.
 * The PLL outputs a PLLCLK and PLLCLK2X. PLLCLK2X is the actual PLL frequency and PLLCLK is
 * half this frequency. PLLCLK is used for MCGOUT and is also typically used by the
 * peripherals that can select the PLL as a clock source. So the PLL frequency generated will
 * be twice the desired frequency.
 * Using the function parameter names the PLL frequency is calculated as follows:
 * PLL freq = ((crystal_val / prdiv_val) * vdiv_val)
 * Refer to the readme file in the mcg driver directory for examples of pll_init configurations.
 * All parameters must be provided, for example crystal_val must be provided even if the
 * oscillator associated with that parameter is already initialized.
 * The various passed parameters are checked to ensure they are within the allowed range. If any
 * of these checks fail the driver will exit and return a fail/error code. An error code will
 * also be returned if any error occurs during the PLL initialization sequence. Refer to the
 * readme file in the mcg driver directory for a list of all these codes.
 *
 * Parameters: prdiv_val   - value to divide the external clock source by to create the desired
 *                           PLL reference clock frequency
 *             vdiv_val    - value to multiply the PLL reference clock frequency by
 *
 * Return value : PLL frequency (Hz) divided by 2 or error code
 */

int32_t  pll_init(int8_t  prdiv_val, int8_t  vdiv_val)
{
	uint8_t		frdiv_val;
	uint8_t		temp_reg;
	uint8_t		prdiv;
	uint8_t		vdiv;
	int16_t		i;
	int32_t		ref_freq;
	int32_t		pll_freq;
	uint32_t	crystal_val;
	uint8_t		hgo_val;
	uint8_t		erefs_val;

	// check if in FEI mode
	if (!((((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x0) && // check CLKS mux has selcted FLL output
      (MCG_S & MCG_S_IREFST_MASK) &&                          // check FLL ref is internal ref clk
      (!(MCG_S & MCG_S_PLLST_MASK))))                         // check PLLS mux has selected FLL
	{
		return 0x1;                                           // return error code
	}

/*
 *  Removed original checks on crystal frequency; Teensy 3.x always uses 16 MHz
 *  crystal as external source (crystal_val = 16000000).
 */
	crystal_val = 16000000;
/*
 *  Removed check of high-gain flag; Teensy 3.x always uses low-power (HGO = 0).
 */
	hgo_val = 0;
/*
 *  Removed check of external select; Teensy 3.x always uses external crystal oscillator
 *  (erefs_val = 1).
 */
	erefs_val = 1;

	// Check PLL divider settings are within spec.
	if ((prdiv_val < 1) || (prdiv_val > 25)) {return 0x41;}
	if ((vdiv_val < 24) || (vdiv_val > 55)) {return 0x42;}

	// Check PLL reference clock frequency is within spec.
	ref_freq = crystal_val / prdiv_val;
	if ((ref_freq < 2000000) || (ref_freq > 4000000)) {return 0x43;}

	// Check PLL output frequency is within spec.
	pll_freq = (crystal_val / prdiv_val) * vdiv_val;
	if ((pll_freq < 48000000) || (pll_freq > 100000000)) {return 0x45;}

	// configure the MCG_C2 register
	// the RANGE value is determined by the external frequency. Since the RANGE parameter affects the FRDIV divide value
	// it still needs to be set correctly even if the oscillator is not being used
      
	temp_reg = MCG_C2;
	temp_reg &= ~(MCG_C2_RANGE0_MASK | MCG_C2_HGO0_MASK | MCG_C2_EREFS0_MASK); // clear fields before writing new values
	temp_reg |= (MCG_C2_RANGE0(2) | (hgo_val << MCG_C2_HGO0_SHIFT) | (erefs_val << MCG_C2_EREFS0_SHIFT));
	MCG_C2 = temp_reg;
  
/*
 *  Removed tests around frdiv_val.  The frdiv_val is fixed at 4 because the Teensy
 *  always uses a 16 MHz crystal.
 */
	frdiv_val = 4;

/*
 *  Select external oscillator and Reference Divider and clear IREFS to start ext osc
 *  If IRCLK is required it must be enabled outside of this driver, existing state
 *  will be maintained.
 *  CLKS=2, FRDIV=frdiv_val, IREFS=0, IRCLKEN=0, IREFSTEN=0
 */
	temp_reg = MCG_C1;
	temp_reg &= ~(MCG_C1_CLKS_MASK | MCG_C1_FRDIV_MASK | MCG_C1_IREFS_MASK); // Clear values in these fields
	temp_reg = MCG_C1_CLKS(2) | MCG_C1_FRDIV(frdiv_val); // Set the required CLKS and FRDIV values
	MCG_C1 = temp_reg;

/*
 *  if the external oscillator is used need to wait for OSCINIT to set
 */
	for (i = 0 ; i < 10000 ; i++)
	{
		if (MCG_S & MCG_S_OSCINIT0_MASK) break; // jump out early if OSCINIT sets before loop finishes
	}
	if (!(MCG_S & MCG_S_OSCINIT0_MASK)) return 0x23; // check bit is really set and return with error if not set

/*
 *  Wait for clock status bits to show clock source is ext ref clk
 */
	for (i = 0 ; i < 2000 ; i++)
	{
		if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x2) break; // jump out early if CLKST shows EXT CLK slected before loop finishes
	}
	if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2) return 0x1A; // check EXT CLK is really selected and return with error if not

/*
 *  Now in FBE
 *  It is recommended that the clock monitor is enabled when using an external clock
 *  as the clock source/reference.
 *  It is enabled here but can be removed if this is not required.
 */
	MCG_C6 |= MCG_C6_CME0_MASK;
  
/*
 *  Configure PLL
 *  Configure MCG_C5
 *  If the PLL is to run in STOP mode then the PLLSTEN bit needs to be OR'ed
 *  in here or in user code.
 */
	temp_reg = MCG_C5;
	temp_reg &= ~MCG_C5_PRDIV0_MASK;
	temp_reg |= MCG_C5_PRDIV0(prdiv_val - 1);    //set PLL ref divider
	MCG_C5 = temp_reg;

/*
 *  Configure MCG_C6
 *  The PLLS bit is set to enable the PLL, MCGOUT still sourced from ext ref clk
 *  The loss of lock interrupt can be enabled by seperately OR'ing in the LOLIE bit in MCG_C6
 */
	temp_reg = MCG_C6;					// store present C6 value
	temp_reg &= ~MCG_C6_VDIV0_MASK;		// clear VDIV settings
	temp_reg |= MCG_C6_PLLS_MASK | MCG_C6_VDIV0(vdiv_val - 24); // write new VDIV and enable PLL
	MCG_C6 = temp_reg;					// update MCG_C6

/*
 *  wait for PLLST status bit to set
 */
	for (i = 0 ; i < 2000 ; i++)
	{
		if (MCG_S & MCG_S_PLLST_MASK) break; // jump out early if PLLST sets before loop finishes
	}
	if (!(MCG_S & MCG_S_PLLST_MASK)) return 0x16; // check bit is really set and return with error if not set

/*
 *  Wait for LOCK bit to set
 */
	for (i = 0 ; i < 2000 ; i++)
	{
		if (MCG_S & MCG_S_LOCK0_MASK) break; // jump out early if LOCK sets before loop finishes
	}
	if (!(MCG_S & MCG_S_LOCK0_MASK)) return 0x44; // check bit is really set and return with error if not set

/*
 *  Use actual PLL settings to calculate PLL frequency
 */
	prdiv = ((MCG_C5 & MCG_C5_PRDIV0_MASK) + 1);
	vdiv = ((MCG_C6 & MCG_C6_VDIV0_MASK) + 24);

/*
 *  now in PBE
 */
	MCG_C1 &= ~MCG_C1_CLKS_MASK; // clear CLKS to switch CLKS mux to select PLL as MCG_OUT

/*
 *  Wait for clock status bits to update
 */
	for (i = 0 ; i < 2000 ; i++)
	{
	if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) == 0x3) break; // jump out early if CLKST = 3 before loop finishes
	}
	if (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3) return 0x1B; // check CLKST is set correctly and return with error if not

/*
 *  Now in PEE
 */
	return ((crystal_val / prdiv) * vdiv); //MCGOUT equals PLL output frequency
} // pll_init



/********************************************************************
 *
 *  Watchdog timer disable routine
 *
 *  This routine was modified from the original in a Freescale Code
 *  Warrior example set.  The original code was contained in a source
 *  file named wdog.c.  That code disabled interrupts prior to unlocking
 *  the watchdog.  Unfortunately, that code also blindly reenabled
 *  interrupts without regard to their state prior to entry.
 *
 *  This code assumes that the calling routine will disable interrupts
 *  prior to the call, if necessary.  8 Apr 14   KEL
 *
 * Parameters:  none
 *
 */
void wdog_disable(void)
{
	WDOG_UNLOCK = 0xC520;			// Write 0xC520 to the unlock register
	WDOG_UNLOCK = 0xD928;			// Followed by 0xD928 to complete the unlock
	WDOG_STCTRLH &= ~WDOG_STCTRLH_WDOGEN_MASK;	// Clear the WDOGEN bit to disable the watchdog
}
