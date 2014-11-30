/**
 * Header file for my implementation of using the USB peripheral
 * Kevin Cuzner
 */
#ifndef _USB_H_
#define _USB_H_

/**
 * Number of USB endpoints
 * 15 max
 */
#define USB_N_ENDPOINTS 0

/**
 * Initializes the USB module
 */
void usb_init(void);

#endif // _USB_H_
