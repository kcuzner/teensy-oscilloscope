#include "usb.h"
#include "arm_cm4.h"

#define PID_OUT   0x1
#define PID_IN    0x9
#define PID_SOF   0x5
#define PID_SETUP 0xd

#define ENDP0_SIZE 64

#define BDT_BC_SHIFT   16
#define BDT_OWN_MASK   0x80
#define BDT_DATA1_MASK 0x40
#define BDT_KEEP_MASK  0x20
#define BDT_NINC_MASK  0x10
#define BDT_DTS_MASK   0x08
#define BDT_STALL_MASK 0x04

#define BDT_VALUE(count, data) ((count << BDT_BC_SHIFT) | BDT_OWN_MASK | (data ? BDT_DATA1_MASK : 0x00) | BDT_DTS_MASK)
#define BDT_PID(desc) ((desc >> 2) & 0xF)

/**
 * Buffer Descriptor Table entry
 * There are two entries per direction per endpoint:
 *   In  Even/Odd
 *   Out Even/Odd
 * A bidirectional endpoint would then need 4 entries
 */
typedef struct {
    uint32_t desc;
    void* addr;
} bdt_t;

// we enforce a max of 15 endpoints (15 + 1 control = 16)
#if USB_N_ENDPOINTS > 15
#error Maximum USB endpoints must be <=15
#endif // USB_N_ENDPOINTS

//determines an appropriate BDT index for the given conditions (see fig. 41-3)
#define RX 0
#define TX 1
#define EVEN 0
#define ODD  1
#define BDT_INDEX(endpoint, tx, odd) ((endpoint << 2) | (tx << 1) | odd)

/**
 * Buffer descriptor table, aligned to a 512-byte boundary (see linker file)
 */
__attribute__ ((section(".usbdescriptortable"), used))
static bdt_t table[(USB_N_ENDPOINTS + 1)*4]; //max endpoints is 15 + 1 control

/**
 * Endpoint 0 receive buffers (2x64 bytes)
 */
static uint8_t endp0_rx[2][ENDP0_SIZE];

/**
 * Handler functions for when a token completes
 * TODO: Determine if this structure really will work for all kinds of handlers
 *
 * I hope this looks like a dynamic jump table to the compiler
 */
static void (*handlers[USB_N_ENDPOINTS + 2]) (uint8_t);

/**
 * Default handler for endpoints which does nothing (to make sure we don't jump anywhere terrible)
 */
static void usb_endp_default_handler(uint8_t stat) { }

/**
 * Endpoint 0 handler
 */
static void usb_endp0_handler(uint8_t stat)
{
    //determine which bdt we are looking at here
    bdt_t* bdt = &table[BDT_INDEX(0, (stat & USB_STAT_TX_MASK) >> USB_STAT_TX_SHIFT, (stat & USB_STAT_ODD_MASK) >> USB_STAT_ODD_SHIFT)];
    GPIOC_PSOR=(1<<5);

    switch (BDT_PID(bdt->desc))
    {
    case PID_SETUP:
    case PID_IN:
    case PID_OUT:
    case PID_SOF:
        break;
    }
}

void usb_register_handler(uint8_t endpoint, void (*f) (uint8_t))
{
    handlers[endpoint] = f;
}

void usb_init(void)
{
    uint32_t i;

    usb_register_handler(0, usb_endp0_handler);

    //switch all handlers to point to the default if they are null
    for (i = 0;i < USB_N_ENDPOINTS + 1; i++)
    {
        if (!handlers[i])
        {
            handlers[i] = usb_endp_default_handler;
        }
    }

    //reset the buffer descriptors
    for (i = 0; i < (USB_N_ENDPOINTS + 1) * 4; i++)
    {
        table[i].desc = 0;
        table[i].addr = 0;
    }

    //1: Select clock source
    SIM_SOPT2 |= SIM_SOPT2_USBSRC_MASK | SIM_SOPT2_PLLFLLSEL_MASK; //we use MCGPLLCLK divided by USB fractional divider
    SIM_CLKDIV2 = SIM_CLKDIV2_USBDIV(1);// SIM_CLKDIV2_USBDIV(2) | SIM_CLKDIV2_USBFRAC_MASK; //(USBFRAC + 1)/(USBDIV + 1) = (1 + 1)/(2 + 1) = 2/3 for 72Mhz clock

    //2: Gate USB clock
    SIM_SCGC4 |= SIM_SCGC4_USBOTG_MASK;

    //3: Software USB module reset
    USB0_USBTRC0 |= USB_USBTRC0_USBRESET_MASK;
    while (USB0_USBTRC0 & USB_USBTRC0_USBRESET_MASK);

    //4: Set BDT base registers
    USB0_BDTPAGE1 = ((uint32_t)table) >> 8;  //bits 15-9
    USB0_BDTPAGE2 = ((uint32_t)table) >> 16; //bits 23-16
    USB0_BDTPAGE3 = ((uint32_t)table) >> 24; //bits 31-24

    //5: Clear all ISR flags and enable weak pull downs
    USB0_ISTAT = 0xFF;
    USB0_ERRSTAT = 0xFF;
    USB0_OTGISTAT = 0xFF;
    USB0_USBTRC0 |= 0x40; //a hint was given that this is an undocumented interrupt bit

    //6: Enable USB reset interrupt
    USB0_CTL = USB_CTL_USBENSOFEN_MASK;
    USB0_USBCTRL = 0;

    USB0_INTEN |= USB_INTEN_USBRSTEN_MASK;
    //NVIC_SET_PRIORITY(IRQ(INT_USB0), 112);
    enable_irq(IRQ(INT_USB0));

    //7: Enable pull-up resistor on D+ (Full speed, 12Mbit/s)
    USB0_CONTROL = USB_CONTROL_DPPULLUPNONOTG_MASK;
}

void USBOTG_IRQHandler(void)
{
    uint8_t status;
    uint8_t stat, endpoint;

    status = USB0_ISTAT;

    if (status & USB_ISTAT_USBRST_MASK)
    {
        //handle USB reset

        //initialize endpoint 0 ping-pong buffers
        USB0_CTL |= USB_CTL_ODDRST_MASK;
        table[BDT_INDEX(0, RX, EVEN)].desc = BDT_VALUE(ENDP0_SIZE, 0);
        table[BDT_INDEX(0, RX, EVEN)].addr = endp0_rx[0];
        table[BDT_INDEX(0, RX, ODD)].desc = BDT_VALUE(ENDP0_SIZE, 0);
        table[BDT_INDEX(0, RX, ODD)].addr = endp0_rx[1];
        table[BDT_INDEX(0, TX, EVEN)].desc = 0;
        table[BDT_INDEX(0, TX, ODD)].addr = 0;

        //initialize endpoint0 to 0x0d (41.5.23)
        //transmit, recieve, and handshake
        USB0_ENDPT0 = USB_ENDPT_EPRXEN_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPHSHK_MASK;

        //clear all interrupts...this is a reset
        USB0_ERRSTAT = 0xff;
        USB0_ISTAT = 0xff;

        //after reset, we are address 0, per USB spec
        USB0_ADDR = 0;

        //all necessary interrupts are now active
        USB0_ERREN = 0xFF;
        USB0_INTEN = USB_INTEN_USBRSTEN_MASK | USB_INTEN_ERROREN_MASK |
            USB_INTEN_SOFTOKEN_MASK | USB_INTEN_TOKDNEEN_MASK |
            USB_INTEN_SLEEPEN_MASK | USB_INTEN_STALLEN_MASK;

        return;
    }
    if (status & USB_ISTAT_ERROR_MASK)
    {
        //handle error
        USB0_ERRSTAT = USB0_ERRSTAT;
        USB0_ISTAT = USB_ISTAT_ERROR_MASK;
    }
    if (status & USB_ISTAT_SOFTOK_MASK)
    {
        //handle start of frame token
        USB0_ISTAT = USB_ISTAT_SOFTOK_MASK;
    }
    if (status & USB_ISTAT_TOKDNE_MASK)
    {
        //handle completion of current token being processed
        stat = USB0_STAT;
        endpoint = stat >> 4;
        handlers[endpoint](stat);

        USB0_ISTAT = USB_ISTAT_TOKDNE_MASK;
    }
    if (status & USB_ISTAT_SLEEP_MASK)
    {
        //handle USB sleep
        USB0_ISTAT = USB_ISTAT_SLEEP_MASK;
    }
    if (status & USB_ISTAT_STALL_MASK)
    {
        //handle usb stall
        USB0_ISTAT = USB_ISTAT_STALL_MASK;
    }
}
