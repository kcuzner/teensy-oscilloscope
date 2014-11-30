/**
 * Header file for my implementation of using the USB peripheral
 * Kevin Cuzner
 */
#ifndef _USB_H_
#define _USB_H_

#include "arm_cm4.h"

/**
 * Number of USB endpoints
 * 15 max
 */
#define USB_N_ENDPOINTS 0

/**
 * Registers an endpoint handler
 */
void usb_register_handler(uint8_t endpoint, void (*f) (uint8_t));

/**
 * Initializes the USB module
 */
void usb_init(void);

#endif // _USB_H_
