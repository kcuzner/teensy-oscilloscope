#include "usb.h"
#include "arm_cm4.h"

/**
 * Descriptor bits struct
 * See Table 41-4
 */
typedef struct {
    unsigned rsrv0 :6;  //31-26
    unsigned bc    :10; //25-16
    unsigned rsrv1 :8;  //15-8
    unsigned own   :1;  //7
    unsigned data0 :1;  //6
    union {             //5-2
        unsigned tok_pid :4;
        struct {
            unsigned keep      :1;
            unsigned ninc      :1;
            unsigned dts       :1;
            unsigned bdt_stall :1;
        };
    };
    unsigned rsrv2 :2;  //1-0
} bdt_bits_t;

/**
 * Buffer Descriptor Table entry
 * There are two entries per direction per endpoint:
 *   In  Even/Odd
 *   Out Even/Odd
 * A bidirectional endpoint would then need 4 entries
 */
typedef struct {
    union {
        uint32_t desc;
        bdt_bits_t desc_bits;
    };
    void* addr;
} bdt_t;

// we enforce a max of 15 endpoints (15 + 1 control = 16)
#if USB_N_ENDPOINTS > 15
#error Maximum USB endpoints must be <=15
#endif // USB_N_ENDPOINTS

/**
 * Buffer descriptor table, aligned to a 512-byte boundary (see linker file)
 */
__attribute__ ((section(".usbdescriptortable"), used))
static bdt_t table[(USB_N_ENDPOINTS + 1)*4]; //max endpoints is 15 + 1 control

void usb_init(void)
{
    uint32_t i;

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
    USB0_ISTAT = USB0_ISTAT;
    GPIOC_PSOR=(1<<5);
}
