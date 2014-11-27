/*
 * File:		uart.h
 * Purpose:     Provide common ColdFire UART routines for polled serial IO
 *
 * Notes:
 * Modified to hide getchar, putchar, and char_avail routines; those now exist
 * as statics inside the uart.c code.  Modified uart_init() to take a UART
 * number (0-2), rather than a UART base pointer, and to remove the clock
 * argument; baud calcs are now based on the selected UART.
 */

#ifndef __UART_H__
#define __UART_H__


/*
 *  These routines support access to all UARTs on the Teensy 3.x (K20).
 *  To use a UART, first call UARTInit() with the UART number (0-2) and
 *  a desired baud rate; this call marks the selected UART as active.
 *  You can then call UARTWrite() to send chars to the active UART.  Use
 *  UARTAvail() to check for received chars.  Use UARTRead() to read
 *  chars (with blocking) from the active UART.
 *
 *  To use a different UART as the active UART, call UARTAssignActiveUART().
 */
void			UARTInit(uint32_t  uartnum, int32_t  baudrate);
uint32_t		UARTAssignActiveUART(uint32_t  uartnum);
int32_t			UARTWrite(const char *ptr, int32_t len);
int32_t			UARTAvail(void);
int32_t			UARTRead(char *ptr, int32_t len);


#endif /* __UART_H__ */
