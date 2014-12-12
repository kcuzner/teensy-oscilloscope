/**
 * Header file for my implementation of using the USB peripheral
 * Kevin Cuzner
 */
#ifndef _USB_H_
#define _USB_H_

#include "arm_cm4.h"

/**
 * Initializes the USB module
 */
void usb_init(void);

void usb_endp0_handler(uint8_t);
void usb_endp1_handler(uint8_t);
void usb_endp2_handler(uint8_t);
void usb_endp3_handler(uint8_t);
void usb_endp4_handler(uint8_t);
void usb_endp5_handler(uint8_t);
void usb_endp6_handler(uint8_t);
void usb_endp7_handler(uint8_t);
void usb_endp8_handler(uint8_t);
void usb_endp9_handler(uint8_t);
void usb_endp10_handler(uint8_t);
void usb_endp11_handler(uint8_t);
void usb_endp12_handler(uint8_t);
void usb_endp13_handler(uint8_t);
void usb_endp14_handler(uint8_t);
void usb_endp15_handler(uint8_t);

#endif // _USB_H_
