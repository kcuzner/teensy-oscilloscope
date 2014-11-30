/*
 * File:        common.h
 * Purpose:     File to be included by all project files
 *
 * Notes:
 *  This is a common header file for Teensy 3.1 projects.  It should
 *  be included in all Teensy 3.1 projects.
 *
 *  If you choose, you can move the PRDIV_VAL and VDIV_VAL settings
 *  to a project-specific file.  This would let you build projects
 *  with different system clock frequencies.
 *
 *  This file is derived from the original found in the Freescale
 *  CodeWarrior source files.  There have been several variations
 *  of these on the Internet; one such is kinetis_50MHz_sc, though
 *  I can't swear that is where this file came from originally.
 *
 *  12 Apr 14  KEL
 */

#ifndef _COMMON_H_
#define _COMMON_H_


#include  <stdint.h>
#include  "MK20D7.h"

/*
 *  Define characteristics of the target platform.
 *
 *  Choose PRDIV_VAL and VDIV_VAL based on your project hardware and system
 *  needs.
 *
 *  Note that PRDIV_VAL is *not* the value written to MCG_C5!  PRDIV_VAL is
 *  an integer divisor for prescaling the external clock for use by the PLL.
 *  PRDIV_VAL must be selected so that:
 *    XTAL_FREQ_HZ / PRDIV_VAL is between 2 MHz and 4 Mhz.
 *
 *  Note that VDIV_VAL is *not* the value written to MCG_C6!  VDIV_VAL is
 *  an integer multiplier for creating the final PLL frequency.
 *  VDIV_VAL must be selected so that:
 *    (XTAL_FREQ_HZ / PRDIV_VAL) * VDIV_VAL is between 48 MHz and 100 MHz.
 *
 * The final clock frequency is determined by PRDIV_VAL and VDIV_VAL.  Here are
 * some sample values for a Teensy 3.1:
 *    For system clock of	PRDIV_VAL	VDIV_VAL
 *    -------------------	---------	--------
 *         48 MHz			    8		   24
 *		   64 MHz				8		   32
 *		   72 MHz			    8		   36
 *
 */
#define PRDIV_VAL			4				/* PLL prescaler */
#define VDIV_VAL			24				/* PLL multiplier */

/*
 *  Optionally define the system console (one of the UARTs) for serial I/O.
 *  The value for TERM_PORT must be a UART base pointer, such as
 *  UART0_BASE_PTR.
 *  The value for TERMINAL_BAUD must be the baud rate, such as
 *  115200.
 *
 *  If your project does not include any of the UART drivers, these
 *  defines are meaningless and can be set to anything.
 */
#define TERM_PORT           UART0_BASE_PTR
#define TERMINAL_BAUD       115200


extern  int32_t				mcg_clk_hz;		// following PLL init, holds actual MCG clock in Hz
extern  int32_t				mcg_clk_khz;	// following PLL init, holds actual MCG clock in kHz
extern  int32_t				core_clk_khz;	// following PLL init, holds actual core clock in kHz
extern  int32_t				periph_clk_khz;	// following PLL init, holds actual peripheral clock in kHz

/********************************************************************/

#endif /* _COMMON_H_ */
