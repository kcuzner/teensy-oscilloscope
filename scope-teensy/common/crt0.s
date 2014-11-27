/*
 * File:	crt0.s
 * Purpose:	Lowest level routines for Kinetis.
 *
 * Notes:  This code is modified from the original from the Kinetis K20
 * package from Freescale (kinetis_50MHz_sc.zip).  Vector table entries
 * are based on the K20P64M72F1 Reference Manual by Freescale; refer to
 * the NVIC section in Chapter 3.
 *
 */

	.syntax unified
	.thumb

	.section ".interrupt_vector_table"
	.global __interrupt_vector_table
/*
 *  This interrupt vector table gets pulled in by the linker script
 *  into flash right before the startup code
 *
 *  The following set of vectors are required by all ARM devices.
 */
__interrupt_vector_table:
	.long _top_stack			/* marks the top of stack */
	.long Reset_Handler
	.long NMI_Handler
	.long HardFault_Handler
	.long MemManage_Handler
	.long BusFault_Handler
	.long UsageFault_Handler
	.long 0
	.long 0
	.long 0
	.long 0
	.long SVC_Handler
	.long DebugMon_Handler
	.long 0
	.long PendSV_Handler
	.long SysTick_Handler

/* The following set of vectors are specific to the Kinetis MK20DD256 devices */

    .long   DMA0_IRQHandler			/* DMA channel 0 transfer complete interrupt */
    .long   DMA1_IRQHandler			/* DMA channel 1 transfer complete interrupt */
    .long   DMA2_IRQHandler			/* DMA channel 2 transfer complete interrupt */
    .long   DMA3_IRQHandler			/* DMA channel 3 transfer complete interrupt */
    .long   DMA4_IRQHandler			/* DMA channel 4 transfer complete interrupt */
    .long   DMA5_IRQHandler			/* DMA channel 5 transfer complete interrupt */
    .long   DMA6_IRQHandler			/* DMA channel 6 transfer complete interrupt */
    .long   DMA7_IRQHandler			/* DMA channel 7 transfer complete interrupt */
    .long   DMA8_IRQHandler			/* DMA channel 8 transfer complete interrupt */
    .long   DMA9_IRQHandler			/* DMA channel 9 transfer complete interrupt */
    .long   DMA10_IRQHandler		/* DMA channel 10 transfer complete interrupt */
    .long   DMA11_IRQHandler		/* DMA channel 11 transfer complete interrupt */
    .long   DMA12_IRQHandler		/* DMA channel 12 transfer complete interrupt */
    .long   DMA13_IRQHandler		/* DMA channel 13 transfer complete interrupt */
    .long   DMA14_IRQHandler		/* DMA channel 14 transfer complete interrupt */
    .long   DMA15_IRQHandler		/* DMA channel 15 transfer complete interrupt */
    .long   DMA_Error_IRQHandler	/* DMA error interrupt */
    .long   Reserved33_IRQHandler	/* Reserved interrupt 33 */
    .long   FlashCmd_IRQHandler		/* Flash memory command complete interrupt */
    .long   FlashReadErr_IRQHandler	/* Flash read collision interrupt */
    .long   LVD_LVW_IRQHandler		/* Low Voltage Detect, Low Voltage Warning */
    .long   LLW_IRQHandler			/* Low Leakage Wakeup */
    .long   Watchdog_IRQHandler		/* WDOG or EVM interrupt (shared) */
    .long	Reserved39_IRQHandler	/* Reserved interrupt 39 */
    .long   I2C0_IRQHandler			/* I2C0 interrupt */
    .long   I2C1_IRQHandler			/* I2C1 interrupt */
    .long   SPI0_IRQHandler			/* SPI0 interrupt */
    .long   SPI1_IRQHandler			/* SPI1 interrupt */
    .long	Reserved44_IRQHandler	/* Reserved interrupt 44 */
	.long	CAN0Msg_IRQHandler		/* CAN0 message buffer (0-15) interrupt */
	.long	CAN0BusOff_IRQHandler	/* CAN0 Bus Off interrupt */
	.long	CAN0Error_IRQHandler	/* CAN0 Error interrupt */
	.long	CAN0Xmit_IRQHandler		/* CAN0 Transmit warning interrupt */  
	.long	CAN0Rcv_IRQHandler		/* CAN0 Recieve warning interrupt */
	.long	CAN0Wake_IRQHandler		/* CAN0 Wake Up interrupt */ 
    .long   I2S0_Tx_IRQHandler		/* I2S0 transmit interrupt */
    .long   I2S0_Rx_IRQHandler		/* I2S0 receive interrupt */
    .long	Reserved53_IRQHandler	/* Reserved interrupt 53 */
    .long	Reserved54_IRQHandler	/* Reserved interrupt 54 */
    .long	Reserved55_IRQHandler	/* Reserved interrupt 55 */
    .long	Reserved56_IRQHandler	/* Reserved interrupt 56 */
    .long	Reserved57_IRQHandler	/* Reserved interrupt 57 */
    .long	Reserved58_IRQHandler	/* Reserved interrupt 58 */
    .long	Reserved59_IRQHandler	/* Reserved interrupt 59 */
    .long   UART0_LON_IRQHandler	/* UART0 LON interrupt */
    .long   UART0_RX_TX_IRQHandler  /* UART0 receive/transmit interrupt */
    .long	UART0Error_IRQHandler	/* UART0 error interrupt */
    .long   UART1_RX_TX_IRQHandler  /* UART1 receive/transmit interrupt */
    .long   UART1Error_IRQHandler	/* UART1 error interrupt */
    .long   UART2_RX_TX_IRQHandler  /* UART2 receive/transmit interrupt */
    .long   UART2Error_IRQHandler	/* UART2 error interrupt */
    .long	Reserved67_IRQHandler	/* Reserved interrupt 67 */
    .long	Reserved68_IRQHandler	/* Reserved interrupt 68 */
    .long	Reserved69_IRQHandler	/* Reserved interrupt 69 */
    .long	Reserved70_IRQHandler	/* Reserved interrupt 70 */
    .long	Reserved71_IRQHandler	/* Reserved interrupt 71 */
    .long	Reserved72_IRQHandler	/* Reserved interrupt 72 */
    .long   ADC0_IRQHandler			/* ADC0 interrupt */
    .long   ADC1_IRQHandler			/* ADC1 interrupt */
    .long   CMP0_IRQHandler			/* CMP0 interrupt */
    .long   CMP1_IRQHandler			/* CMP1 interrupt */
    .long   CMP2_IRQHandler			/* CMP2 interrupt */
    .long   FTM0_IRQHandler			/* FTM0 fault, all sources interrupt */
    .long   FTM1_IRQHandler			/* FTM1 fault, all sources interrupt */
    .long   FTM2_IRQHandler			/* FTM2 fault, all sources interrupt */
    .long   CMT_IRQHandler			/* CMT interrupt */
    .long   RTC_IRQHandler			/* RTC alarm interrupt */
    .long   RTCSeconds_IRQHandler   /* RTC seconds interrupt */
    .long   PIT0_IRQHandler			/* PIT timer channel 0 interrupt */
    .long   PIT1_IRQHandler			/* PIT timer channel 1 interrupt */
    .long   PIT2_IRQHandler			/* PIT timer channel 2 interrupt */
    .long   PIT3_IRQHandler			/* PIT timer channel 3 interrupt */
    .long   PDB0_IRQHandler			/* PDB0 interrupt */
    .long   USBOTG_IRQHandler		/* USB OTG interrupt */
    .long   USBCharge_IRQHandler	/* USB charger detect interrupt */
    .long	Reserved91_IRQHandler	/* Reserved interrupt 91 */
    .long	Reserved92_IRQHandler	/* Reserved interrupt 92 */
    .long	Reserved93_IRQHandler	/* Reserved interrupt 93 */
    .long	Reserved94_IRQHandler	/* Reserved interrupt 94 */
    .long	Reserved95_IRQHandler	/* Reserved interrupt 95 */
    .long	Reserved96_IRQHandler	/* Reserved interrupt 96 */
    .long	DAC0_IRQHandler			/* DAC0 interrupt */
    .long	Reserved98_IRQHandler	/* Reserved interrupt 98 */
    .long   TSI_IRQHandler			/* TSI all sources interrupt */
    .long   MCG_IRQHandler			/* MCG interrupt */
    .long   LPTimer_IRQHandler		/* Low-power Timer interrupt */
    .long	Reserved102_IRQHandler	/* Reserved interrupt 102 */
    .long   PORTA_IRQHandler		/* Port A pin detect interrupt */
    .long   PORTB_IRQHandler		/* Port B pin detect interrupt */
    .long   PORTC_IRQHandler		/* Port C pin detect interrupt */
    .long   PORTD_IRQHandler		/* Port D pin detect interrupt */
    .long   PORTE_IRQHandler		/* Port E pin detect interrupt */
    .long	Reserved108_IRQHandler	/* Reserved interrupt 108 */
    .long	Reserved109_IRQHandler	/* Reserved interrupt 109 */
    .long   SWI_IRQHandler			/* Software interrupt */
    .long   DefaultISR				/* 111 */
    .long   DefaultISR				/* 112 */
    .long   DefaultISR				/* 113 */
    .long   DefaultISR				/* 114 */
    .long   DefaultISR				/* 115 */
    .long   DefaultISR				/* 116 */
    .long   DefaultISR				/* 117 */
    .long   DefaultISR				/* 118 */
    .long   DefaultISR				/* 119 */
    .long   DefaultISR				/* 120 */
    .long   DefaultISR				/* 121 */
    .long   DefaultISR				/* 122 */
    .long   DefaultISR				/* 123 */
    .long   DefaultISR				/* 124 */
    .long   DefaultISR				/* 125 */
    .long   DefaultISR				/* 126 */
    .long   DefaultISR				/* 127 */
    .long   DefaultISR				/* 128 */
    .long   DefaultISR				/* 129 */
    .long   DefaultISR				/* 130 */
    .long   DefaultISR				/* 131 */
    .long   DefaultISR				/* 132 */
    .long   DefaultISR				/* 133 */
    .long   DefaultISR				/* 134 */
    .long   DefaultISR				/* 135 */
    .long   DefaultISR				/* 136 */
    .long   DefaultISR				/* 137 */
    .long   DefaultISR				/* 138 */
    .long   DefaultISR				/* 139 */
    .long   DefaultISR				/* 140 */
    .long   DefaultISR				/* 141 */
    .long   DefaultISR				/* 142 */
    .long   DefaultISR				/* 143 */
    .long   DefaultISR				/* 144 */
    .long   DefaultISR				/* 145 */
    .long   DefaultISR				/* 146 */
    .long   DefaultISR				/* 147 */
    .long   DefaultISR				/* 148 */
    .long   DefaultISR				/* 149 */
    .long   DefaultISR				/* 150 */
    .long   DefaultISR				/* 151 */
    .long   DefaultISR				/* 152 */
    .long   DefaultISR				/* 153 */
    .long   DefaultISR				/* 154 */
    .long   DefaultISR				/* 155 */
    .long   DefaultISR				/* 156 */
    .long   DefaultISR				/* 157 */
    .long   DefaultISR				/* 158 */
    .long   DefaultISR				/* 159 */
    .long   DefaultISR				/* 160 */
    .long   DefaultISR				/* 161 */
    .long   DefaultISR				/* 162 */
    .long   DefaultISR				/* 163 */
    .long   DefaultISR				/* 164 */
    .long   DefaultISR				/* 165 */
    .long   DefaultISR				/* 166 */
    .long   DefaultISR				/* 167 */
    .long   DefaultISR				/* 168 */
    .long   DefaultISR				/* 169 */
    .long   DefaultISR				/* 170 */
    .long   DefaultISR				/* 171 */
    .long   DefaultISR				/* 172 */
    .long   DefaultISR				/* 173 */
    .long   DefaultISR				/* 174 */
    .long   DefaultISR				/* 175 */
    .long   DefaultISR				/* 176 */
    .long   DefaultISR				/* 177 */
    .long   DefaultISR				/* 178 */
    .long   DefaultISR				/* 179 */
    .long   DefaultISR				/* 180 */
    .long   DefaultISR				/* 181 */
    .long   DefaultISR				/* 182 */
    .long   DefaultISR				/* 183 */
    .long   DefaultISR				/* 184 */
    .long   DefaultISR				/* 185 */
    .long   DefaultISR				/* 186 */
    .long   DefaultISR				/* 187 */
    .long   DefaultISR				/* 188 */
    .long   DefaultISR				/* 189 */
    .long   DefaultISR				/* 190 */
    .long   DefaultISR				/* 191 */
    .long   DefaultISR				/* 192 */
    .long   DefaultISR				/* 193 */
    .long   DefaultISR				/* 194 */
    .long   DefaultISR				/* 195 */
    .long   DefaultISR				/* 196 */
    .long   DefaultISR				/* 197 */
    .long   DefaultISR				/* 198 */
    .long   DefaultISR				/* 199 */
    .long   DefaultISR				/* 200 */
    .long   DefaultISR				/* 201 */
    .long   DefaultISR				/* 202 */
    .long   DefaultISR				/* 203 */
    .long   DefaultISR				/* 204 */
    .long   DefaultISR				/* 205 */
    .long   DefaultISR				/* 206 */
    .long   DefaultISR				/* 207 */
    .long   DefaultISR				/* 208 */
    .long   DefaultISR				/* 209 */
    .long   DefaultISR				/* 210 */
    .long   DefaultISR				/* 211 */
    .long   DefaultISR				/* 212 */
    .long   DefaultISR				/* 213 */
    .long   DefaultISR				/* 214 */
    .long   DefaultISR				/* 215 */
    .long   DefaultISR				/* 216 */
    .long   DefaultISR				/* 217 */
    .long   DefaultISR				/* 218 */
    .long   DefaultISR				/* 219 */
    .long   DefaultISR				/* 220 */
    .long   DefaultISR				/* 221 */
    .long   DefaultISR				/* 222 */
    .long   DefaultISR				/* 223 */
    .long   DefaultISR				/* 224 */
    .long   DefaultISR				/* 225 */
    .long   DefaultISR				/* 226 */
    .long   DefaultISR				/* 227 */
    .long   DefaultISR				/* 228 */
    .long   DefaultISR				/* 229 */
    .long   DefaultISR				/* 230 */
    .long   DefaultISR				/* 231 */
    .long   DefaultISR				/* 232 */
    .long   DefaultISR				/* 233 */
    .long   DefaultISR				/* 234 */
    .long   DefaultISR				/* 235 */
    .long   DefaultISR				/* 236 */
    .long   DefaultISR				/* 237 */
    .long   DefaultISR				/* 238 */
    .long   DefaultISR				/* 239 */
    .long   DefaultISR				/* 240 */
    .long   DefaultISR				/* 241 */
    .long   DefaultISR				/* 242 */
    .long   DefaultISR				/* 243 */
    .long   DefaultISR				/* 244 */
    .long   DefaultISR				/* 245 */
    .long   DefaultISR				/* 246 */
    .long   DefaultISR				/* 247 */
    .long   DefaultISR				/* 248 */
    .long   DefaultISR				/* 249 */
    .long   DefaultISR				/* 250 */
    .long   DefaultISR				/* 251 */
    .long   DefaultISR				/* 252 */
    .long   DefaultISR				/* 253 */
    .long   DefaultISR				/* 254 */
    .long   DefaultISR				/* 255 */


/* Flash Configuration */

  	.long	0xFFFFFFFF
  	.long	0xFFFFFFFF
  	.long	0xFFFFFFFF
  	.long	0xFFFFFFFE

	.thumb


/*
 *  Now declare all interrupts handlers except the reset handler
 *  to be weak.  This allows you to create a replacement handler
 *  in C with the same name and install it in the vector table
 *  above at link time.
 */

	.weak	NMI_Handler
	.weak	HardFault_Handler
	.weak	MemManage_Handler
	.weak	BusFault_Handler
	.weak	UsageFault_Handler
	.weak	SVC_Handler
	.weak	DebugMon_Handler
	.weak	PendSV_Handler
	.weak	SysTick_Handler


    .weak   DMA0_IRQHandler				/* DMA channel 0 transfer complete interrupt */
    .weak   DMA1_IRQHandler				/* DMA channel 1 transfer complete interrupt */
    .weak   DMA2_IRQHandler				/* DMA channel 2 transfer complete interrupt */
    .weak   DMA3_IRQHandler				/* DMA channel 3 transfer complete interrupt */
    .weak   DMA4_IRQHandler				/* DMA channel 4 transfer complete interrupt */
    .weak   DMA5_IRQHandler				/* DMA channel 5 transfer complete interrupt */
    .weak   DMA6_IRQHandler				/* DMA channel 6 transfer complete interrupt */
    .weak   DMA7_IRQHandler				/* DMA channel 7 transfer complete interrupt */
    .weak   DMA8_IRQHandler				/* DMA channel 8 transfer complete interrupt */
    .weak   DMA9_IRQHandler				/* DMA channel 9 transfer complete interrupt */
    .weak   DMA10_IRQHandler			/* DMA channel 10 transfer complete interrupt */
    .weak   DMA11_IRQHandler			/* DMA channel 11 transfer complete interrupt */
    .weak   DMA12_IRQHandler			/* DMA channel 12 transfer complete interrupt */
    .weak   DMA13_IRQHandler			/* DMA channel 13 transfer complete interrupt */
    .weak   DMA14_IRQHandler			/* DMA channel 14 transfer complete interrupt */
    .weak   DMA15_IRQHandler			/* DMA channel 15 transfer complete interrupt */
    .weak   DMA_Error_IRQHandler		/* DMA error interrupt */
    .weak   Reserved33_IRQHandler		/* Reserved interrupt 33 */
    .weak   FlashCmd_IRQHandler			/* Flash memory command complete interrupt */
    .weak   FlashReadErr_IRQHandler		/* Flash read collision interrupt */
    .weak   LVD_LVW_IRQHandler			/* Low Voltage Detect, Low Voltage Warning */
    .weak   LLW_IRQHandler				/* Low Leakage Wakeup */
    .weak   Watchdog_IRQHandler			/* WDOG or EVM interrupt (shared) */
    .weak	Reserved39_IRQHandler		/* Reserved interrupt 39 */
    .weak   I2C0_IRQHandler				/* I2C0 interrupt */
    .weak   I2C1_IRQHandler				/* I2C1 interrupt */
    .weak   SPI0_IRQHandler				/* SPI0 interrupt */
    .weak   SPI1_IRQHandler				/* SPI1 interrupt */
    .weak	Reserved44_IRQHandler		/* Reserved interrupt 44 */
	.weak	CAN0Msg_IRQHandler			/* CAN0 message buffer (0-15) interrupt */
	.weak	CAN0BusOff_IRQHandler		/* CAN0 Bus Off interrupt */
	.weak	CAN0Error_IRQHandler		/* CAN0 Error interrupt */
	.weak	CAN0Xmit_IRQHandler			/* CAN0 Transmit warning interrupt */  
	.weak	CAN0Rcv_IRQHandler			/* CAN0 Recieve warning interrupt */
	.weak	CAN0Wake_IRQHandler			/* CAN0 Wake Up interrupt */ 
    .weak   I2S0_Tx_IRQHandler			/* I2S0 transmit interrupt */
    .weak   I2S0_Rx_IRQHandler			/* I2S0 receive interrupt */
    .weak	Reserved53_IRQHandler		/* Reserved interrupt 53 */
    .weak	Reserved54_IRQHandler		/* Reserved interrupt 54 */
    .weak	Reserved55_IRQHandler		/* Reserved interrupt 55 */
    .weak	Reserved56_IRQHandler		/* Reserved interrupt 56 */
    .weak	Reserved57_IRQHandler		/* Reserved interrupt 57 */
    .weak	Reserved58_IRQHandler		/* Reserved interrupt 58 */
    .weak	Reserved59_IRQHandler		/* Reserved interrupt 59 */
    .weak   UART0_LON_IRQHandler		/* UART0 LON interrupt */
    .weak   UART0_RX_TX_IRQHandler		/* UART0 receive/transmit interrupt */
    .weak	UART0Error_IRQHandler		/* UART0 error interrupt */
    .weak   UART1_RX_TX_IRQHandler		/* UART1 receive/transmit interrupt */
    .weak   UART1Error_IRQHandler		/* UART1 error interrupt */
    .weak   UART2_RX_TX_IRQHandler		/* UART2 receive/transmit interrupt */
    .weak   UART2Error_IRQHandler		/* UART2 error interrupt */
    .weak	Reserved67_IRQHandler		/* Reserved interrupt 67 */
    .weak	Reserved68_IRQHandler		/* Reserved interrupt 68 */
    .weak	Reserved69_IRQHandler		/* Reserved interrupt 69 */
    .weak	Reserved70_IRQHandler		/* Reserved interrupt 70 */
    .weak	Reserved71_IRQHandler		/* Reserved interrupt 71 */
    .weak	Reserved72_IRQHandler		/* Reserved interrupt 72 */
    .weak   ADC0_IRQHandler				/* ADC0 interrupt */
    .weak   ADC1_IRQHandler				/* ADC1 interrupt */
    .weak   CMP0_IRQHandler				/* CMP0 interrupt */
    .weak   CMP1_IRQHandler				/* CMP1 interrupt */
    .weak   CMP2_IRQHandler				/* CMP2 interrupt */
    .weak   FTM0_IRQHandler				/* FTM0 fault, all sources interrupt */
    .weak   FTM1_IRQHandler				/* FTM1 fault, all sources interrupt */
    .weak   FTM2_IRQHandler				/* FTM2 fault, all sources interrupt */
    .weak   CMT_IRQHandler				/* CMT interrupt */
    .weak   RTC_IRQHandler				/* RTC alarm interrupt */
    .weak   RTCSeconds_IRQHandler		/* RTC seconds interrupt */
    .weak   PIT0_IRQHandler				/* PIT timer channel 0 interrupt */
    .weak   PIT1_IRQHandler				/* PIT timer channel 1 interrupt */
    .weak   PIT2_IRQHandler				/* PIT timer channel 2 interrupt */
    .weak   PIT3_IRQHandler				/* PIT timer channel 3 interrupt */
    .weak   PDB0_IRQHandler				/* PDB0 interrupt */
    .weak   USBOTG_IRQHandler			/* USB OTG interrupt */
    .weak   USBCharge_IRQHandler		/* USB charger detect interrupt */
    .weak	Reserved91_IRQHandler		/* Reserved interrupt 91 */
    .weak	Reserved92_IRQHandler		/* Reserved interrupt 92 */
    .weak	Reserved93_IRQHandler		/* Reserved interrupt 93 */
    .weak	Reserved94_IRQHandler		/* Reserved interrupt 94 */
    .weak	Reserved95_IRQHandler		/* Reserved interrupt 95 */
    .weak	Reserved96_IRQHandler		/* Reserved interrupt 96 */
    .weak	DAC0_IRQHandler				/* DAC0 interrupt */
    .weak	Reserved98_IRQHandler		/* Reserved interrupt 98 */
    .weak   TSI_IRQHandler				/* TSI all sources interrupt */
    .weak   MCG_IRQHandler				/* MCG interrupt */
    .weak   LPTimer_IRQHandler			/* Low-power Timer interrupt */
    .weak	Reserved102_IRQHandler		/* Reserved interrupt 102 */
    .weak   PORTA_IRQHandler			/* Port A pin detect interrupt */
    .weak   PORTB_IRQHandler			/* Port B pin detect interrupt */
    .weak   PORTC_IRQHandler			/* Port C pin detect interrupt */
    .weak   PORTD_IRQHandler			/* Port D pin detect interrupt */
    .weak   PORTE_IRQHandler			/* Port E pin detect interrupt */
    .weak	Reserved108_IRQHandler		/* Reserved interrupt 108 */
    .weak	Reserved109_IRQHandler		/* Reserved interrupt 109 */
    .weak   SWI_IRQHandler				/* Software interrupt */
    .weak   DefaultISR
    


/*
 *  Actual code.
 */
	.section ".startup","x",%progbits
	.thumb_func
	.global _startup
	.global Reset_Handler


Reset_Handler:
_startup:
    mov     r0,#0                   /* Initialize the GPRs */
	mov     r1,#0
	mov     r2,#0
	mov     r3,#0
	mov     r4,#0
	mov     r5,#0
	mov     r6,#0
	mov     r7,#0
	mov     r8,#0
	mov     r9,#0
	mov     r10,#0
	mov     r11,#0
	mov     r12,#0

/*
 *  The CodeWarrior model wants to do system init in C rather than
 *  assembly.  But one of the system init functions is disabling the
 *  watchdog, which must happen within the first 256 clocks.  This
 *  conflicts with my desire to do all system init in assembler
 *  before jumping to main().
 *
 *  As a compromise, do an assembler jump to the wdog_disable() routine,
 *  then have that routine return here to finish low-level setup.
 */
	ldr r0, =wdog_disable
	blx r0

/*
 *  With the watchdog disabled, it is now safe to initialize areas
 *  of RAM without fear of watchdog timeout.
 *
 *  Clear the BSS section
 */ 
	mov r0, #0
	ldr r1, = _start_bss
	ldr r2, = _end_bss
	cmp	r1, r2
	beq	_done_clear

	sub r2, #1
_clear:
	cmp r1, r2
	str r0, [r1, #0]
	add r1, #4
	blo _clear
_done_clear:


/* 
 *  Copy data from flash initialization area to RAM
 *
 *  The three values seen here are supplied by the linker script
 */
    ldr   r0, =_start_data_flash	/* initial values, found in flash */
    ldr   r1, =_start_data			/* target locations in RAM to write */
    ldr   r2, =_data_size			/* number of bytes to write */

/*
 *  Perform the copy.
 *  Handle the special case where _data_size == 0
 */
    cmp   r2, #0
    beq   done_copy
copy:
    ldrb   r4, [r0], #1
    strb   r4, [r1], #1
    subs   r2, r2, #1
    bne    copy
done_copy:

/*
 *  Configure vector table offset register
 */
  ldr r0, =0xE000ED08
  ldr r1, =__interrupt_vector_table
  str r1, [r0]

/*
 *  Low-level init is done.  Time to hand off to the code that does
 *  mid-level init, such as setting up the PLL and any memory/gpio
 *  accesses.
 *
 *  Traditionally, this is done by invoking main() and letting main()
 *  do the setup.  The Freescale code (Code Warrior?) wants to use
 *  a start routine for mid-level setup; that routine then calls
 *  main().  OK, whatever...
 */
	ldr r0, =start
	blx r0
	b	.					/* just in case control ever leaves main()! */
	
/*
 *  The following stub routines serve as default handlers for the
 *  above vectors (except for the reset handler, of course).
 *
 *  You can recode this to have all dummy handlers use the same
 *  function, if you choose.  However, if you have a debugging
 *  tool, you should use a separate function for each default handler
 *  to make it easier to determine what caused any stray interrupt.
 */

/*
 *  Stub routines for the main vectors.
 */
NMI_Handler:
	b	.
	    
HardFault_Handler:
	b	.
	    
MemManage_Handler:
	b	.
	    
BusFault_Handler:
	b	.
	    
UsageFault_Handler:
	b	.
	    
SVC_Handler:
	b	.
	    
DebugMon_Handler:
	b	.
	    
PendSV_Handler:
	b	.
	    
SysTick_Handler:
	b	.


/*
 *  Declare stub ISR handlers for the external interrupts.
 *
 *  If you have access to a debugger and need to isolate each of the
 *  handlers, simply follow each of the following entry points with
 *  a branch to self ("  b  .").
 */
DMA0_IRQHandler:
DMA1_IRQHandler:
DMA2_IRQHandler:
DMA3_IRQHandler:
DMA4_IRQHandler:
DMA5_IRQHandler:
DMA6_IRQHandler:
DMA7_IRQHandler:
DMA8_IRQHandler:
DMA9_IRQHandler:
DMA10_IRQHandler:
DMA11_IRQHandler:
DMA12_IRQHandler:
DMA13_IRQHandler:
DMA14_IRQHandler:
DMA15_IRQHandler:
DMA_Error_IRQHandler:
Reserved33_IRQHandler:
FlashCmd_IRQHandler:
FlashReadErr_IRQHandler:
LVD_LVW_IRQHandler:
LLW_IRQHandler:
Watchdog_IRQHandler:
Reserved39_IRQHandler:
I2C0_IRQHandler:
I2C1_IRQHandler:
SPI0_IRQHandler:
SPI1_IRQHandler:
Reserved44_IRQHandler:
FlashCmd_IRQHandler:
FlashReadErr_IRQHandler:
LVD_LVW_IRQHandler:
LLW_IRQHandler:
Watchdog_IRQHandler:
Reserved39_IRQHandler:
I2C0_IRQHandler:
I2C1_IRQHandler:
SPI0_IRQHandler:
SPI1_IRQHandler:
Reserved44_IRQHandler:
CAN0Msg_IRQHandler:
CAN0BusOff_IRQHandler:
CAN0Error_IRQHandler:
CAN0Xmit_IRQHandler:
CAN0Rcv_IRQHandler:
CAN0Wake_IRQHandler:
I2S0_Tx_IRQHandler:
I2S0_Rx_IRQHandler:
Reserved53_IRQHandler:
Reserved54_IRQHandler:
Reserved55_IRQHandler:
Reserved56_IRQHandler:
Reserved57_IRQHandler:
Reserved58_IRQHandler:
Reserved59_IRQHandler:
UART0_LON_IRQHandler:
UART0_RX_TX_IRQHandler:
UART0Error_IRQHandler:
UART1_RX_TX_IRQHandler:
UART1Error_IRQHandler:
UART2_RX_TX_IRQHandler:
UART2Error_IRQHandler:
Reserved67_IRQHandler:
Reserved68_IRQHandler:
Reserved69_IRQHandler:
Reserved70_IRQHandler:
Reserved71_IRQHandler:
Reserved72_IRQHandler:
ADC0_IRQHandler:
ADC1_IRQHandler:
CMP0_IRQHandler:
CMP1_IRQHandler:
CMP2_IRQHandler:
FTM0_IRQHandler:
FTM1_IRQHandler:
FTM2_IRQHandler:
CMT_IRQHandler:
RTC_IRQHandler:
RTCSeconds_IRQHandler:
PIT0_IRQHandler:
PIT1_IRQHandler:
PIT2_IRQHandler:
PIT3_IRQHandler:
PDB0_IRQHandler:
USBOTG_IRQHandler:
USBCharge_IRQHandler:
Reserved91_IRQHandler:
Reserved92_IRQHandler:
Reserved93_IRQHandler:
Reserved94_IRQHandler:
Reserved95_IRQHandler:
Reserved96_IRQHandler:
DAC0_IRQHandler:
Reserved98_IRQHandler:
TSI_IRQHandler:
MCG_IRQHandler:
LPTimer_IRQHandler:
Reserved102_IRQHandler:
PORTA_IRQHandler:
PORTB_IRQHandler:
PORTC_IRQHandler:
PORTD_IRQHandler:
PORTE_IRQHandler:
Reserved108_IRQHandler:
Reserved109_IRQHandler:
SWI_IRQHandler:
DefaultISR:
	b	.

 
	.end
