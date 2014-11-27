/*
 *  term_io.h      terminal I/O functions for exchanging chars with the active UART
 *
 *  These functions interface the Teensy 3.x UARTs with user programs, providing
 *  common char I/O functions such as string print and reading chars from a UART
 *  console.
 *
 *  This routines are based on printf(), getc(), and others, but specifically
 *  coded to use the UART primitives in Karl Lunt's uart.c file.  Refer to the
 *  corresponding uart.h and uart.c files for details.
 */

#ifndef  TERM_IO_H
#define  TERM_IO_H


int32_t			xatoi (char **str, double *res);
void			xitoa (double val, int32_t radix, int32_t len);
void			xputc (char c);
uint8_t			xgetc(void);
int32_t			xavail(void);
void			xputs (const char* str);
void			xprintf (const char* str, ...);
void			put_dump (const uint8_t *buff, uint32_t ofs, int32_t cnt);
void			get_line (char *buff, int32_t len);
int32_t			get_line_r (char *buff, int32_t len, int32_t *idx);

#endif
