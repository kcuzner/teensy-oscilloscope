#include "usb.h"
#include "arm_cm4.h"

#define PID_OUT   0x1
#define PID_IN    0x9
#define PID_SOF   0x5
#define PID_SETUP 0xd

#define ENDP0_SIZE 64

typedef struct {
    union {
        struct {
            uint8_t bmRequestType;
            uint8_t bRequest;
        };
        uint16_t wRequestAndType;
    };
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} setup_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t bcdUSB;
    uint8_t bDeviceClass;
    uint8_t bDeviceSubClass;
    uint8_t bDeviceProtocol;
    uint8_t bMaxPacketSize0;
    uint16_t idVendor;
    uint16_t idProduct;
    uint16_t bcdDevice;
    uint8_t iManufacturer;
    uint8_t iProduct;
    uint8_t iSerialNumber;
    uint8_t bNumConfigurations;
} dev_descriptor_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint8_t bInterfaceNumber;
    uint8_t bAlternateSetting;
    uint8_t bNumEndpoints;
    uint8_t bInterfaceClass;
    uint8_t bInterfaceSubClass;
    uint8_t bInterfaceProtocol;
    uint8_t iInterface;
} int_descriptor_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wTotalLength;
    uint8_t bNumInterfaces;
    uint8_t bConfigurationValue;
    uint8_t iConfiguration;
    uint8_t bmAttributes;
    uint8_t bMaxPower;
    int_descriptor_t interfaces[];
} cfg_descriptor_t;

typedef struct {
    uint8_t bLength;
    uint8_t bDescriptorType;
    uint16_t wString[];
} str_descriptor_t;

typedef struct {
    uint16_t wValue;
    uint16_t wIndex;
    const void* addr;
    uint8_t length;
} descriptor_entry_t;

#define BDT_BC_SHIFT   16
#define BDT_OWN_MASK   0x80
#define BDT_DATA1_MASK 0x40
#define BDT_KEEP_MASK  0x20
#define BDT_NINC_MASK  0x10
#define BDT_DTS_MASK   0x08
#define BDT_STALL_MASK 0x04

#define BDT_DESC(count, data) ((count << BDT_BC_SHIFT) | BDT_OWN_MASK | (data ? BDT_DATA1_MASK : 0x00) | BDT_DTS_MASK)
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
#define USB_N_ENDPOINTS 15

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
 * Device descriptor
 * NOTE: This cannot be const because without additional attributes, it will
 * not be placed in a part of memory that the usb subsystem can access. I
 * have a suspicion that this location is somewhere in flash, but not copied
 * to RAM.
 */
static dev_descriptor_t dev_descriptor = {
    .bLength = 18,
    .bDescriptorType = 1,
    .bcdUSB = 0x0200,
    .bDeviceClass = 0xff,
    .bDeviceSubClass = 0x0,
    .bDeviceProtocol = 0x0,
    .bMaxPacketSize0 = ENDP0_SIZE,
    .idVendor = 0x16c0,
    .idProduct = 0x05dc,
    .bcdDevice = 0x0001,
    .iManufacturer = 1,
    .iProduct = 0,
    .iSerialNumber = 0,
    .bNumConfigurations = 1
};

/**
 * Configuration descriptor
 * NOTE: Same thing about const applies here
 */
static cfg_descriptor_t cfg_descriptor = {
    .bLength = 9,
    .bDescriptorType = 2,
    .wTotalLength = 18,
    .bNumInterfaces = 1,
    .bConfigurationValue = 1,
    .iConfiguration = 0,
    .bmAttributes = 0x80,
    .bMaxPower = 250,
    .interfaces = {
        {
            .bLength = 9,
            .bDescriptorType = 4,
            .bInterfaceNumber = 0,
            .bAlternateSetting = 0,
            .bNumEndpoints = 0,
            .bInterfaceClass = 0xff,
            .bInterfaceSubClass = 0x0,
            .bInterfaceProtocol = 0x0,
            .iInterface = 0
        }
    }
};

static str_descriptor_t lang_descriptor = {
    .bLength = 4,
    .bDescriptorType = 3,
    .wString = { 0x0409 } //english (US)
};

static str_descriptor_t manuf_descriptor = {
    .bLength = 2 + 4 * 2,
    .bDescriptorType = 3,
    .wString = {'a','s','d','f'}
};

static const descriptor_entry_t descriptors[] = {
    { 0x0100, 0x0000, &dev_descriptor, sizeof(dev_descriptor) },
    { 0x0200, 0x0000, &cfg_descriptor, 18 },
    { 0x0300, 0x0000, &lang_descriptor, 4 },
    { 0x0301, 0x0409, &manuf_descriptor, 10 },
    { 0x0000, 0x0000, NULL, 0 }
};

static uint8_t endp0_odd, endp0_data = 0;
static void usb_endp0_transmit(const void* data, uint8_t length)
{
    table[BDT_INDEX(0, TX, endp0_odd)].addr = (void *)data;
    table[BDT_INDEX(0, TX, endp0_odd)].desc = BDT_DESC(length, endp0_data);
    //toggle the odd and data bits
    endp0_odd ^= 1;
    endp0_data ^= 1;
}

/**
 * Endpoint 0 setup handler
 */
static void usb_endp0_handle_setup(setup_t* packet)
{
    const descriptor_entry_t* entry;
    const uint8_t* data = NULL;
    uint8_t data_length = 0;


    switch(packet->wRequestAndType)
    {
    case 0x0500: //set address (wait for IN packet)
        break;
    case 0x0900: //set configuration
        //we only have one configuration at this time
        break;
    case 0x0680: //get descriptor
    case 0x0681:
        for (entry = descriptors; 1; entry++)
        {
            if (entry->addr == NULL)
                break;

            if (packet->wValue == entry->wValue && packet->wIndex == entry->wIndex)
            {
                //this is the descriptor to send
                data = entry->addr;
                data_length = entry->length;
                goto send;
            }
        }
        goto stall;
        break;
    default:
        goto stall;
    }

    //if we are sent here, we need to send some data
    send:
        if (data_length > packet->wLength)
            data_length = packet->wLength;
        usb_endp0_transmit(data, data_length);
        return;

    //if we make it here, we are not able to send data and have stalled
    stall:
        USB0_ENDPT0 = USB_ENDPT_EPSTALL_MASK | USB_ENDPT_EPRXEN_MASK | USB_ENDPT_EPTXEN_MASK | USB_ENDPT_EPHSHK_MASK;
}

/**
 * Endpoint 0 handler
 */
void usb_endp0_handler(uint8_t stat)
{
    static setup_t last_setup;

    //determine which bdt we are looking at here
    bdt_t* bdt = &table[BDT_INDEX(0, (stat & USB_STAT_TX_MASK) >> USB_STAT_TX_SHIFT, (stat & USB_STAT_ODD_MASK) >> USB_STAT_ODD_SHIFT)];

    switch (BDT_PID(bdt->desc))
    {
    case PID_SETUP:
        //extract the setup token
		last_setup = *((setup_t*)(bdt->addr));

		//we are now done with the buffer
        bdt->desc = BDT_DESC(ENDP0_SIZE, 1);

        //clear any pending IN stuff
        table[BDT_INDEX(0, TX, EVEN)].desc = 0;
		table[BDT_INDEX(0, TX, ODD)].desc = 0;
        endp0_data = 1;

        //cast the data into our setup type and run the setup
        usb_endp0_handle_setup(&last_setup);//&last_setup);

        //unfreeze this endpoint
        USB0_CTL = USB_CTL_USBENSOFEN_MASK;
        break;
    case PID_IN:
        if (last_setup.wRequestAndType == 0x0500)
        {
            USB0_ADDR = last_setup.wValue;
        }
        break;
    case PID_OUT:
        //nothing to do here..just give the buffer back
        bdt->desc = BDT_DESC(ENDP0_SIZE, 1);
        break;
    case PID_SOF:
        break;
    }

    USB0_CTL = USB_CTL_USBENSOFEN_MASK;
}

static void (*handlers[16])(uint8_t) = {
    usb_endp0_handler,
    usb_endp1_handler,
    usb_endp2_handler,
    usb_endp3_handler,
    usb_endp4_handler,
    usb_endp5_handler,
    usb_endp6_handler,
    usb_endp7_handler,
    usb_endp8_handler,
    usb_endp9_handler,
    usb_endp10_handler,
    usb_endp11_handler,
    usb_endp12_handler,
    usb_endp13_handler,
    usb_endp14_handler,
    usb_endp15_handler,
};

/**
 * Default handler for USB endpoints that does nothing
 */
static void usb_endp_default_handler(uint8_t stat) { }

//weak aliases as "defaults" for the usb endpoint handlers
void usb_endp1_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp2_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp3_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp4_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp5_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp6_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp7_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp8_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp9_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp10_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp11_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp12_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp13_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp14_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));
void usb_endp15_handler(uint8_t) __attribute__((weak, alias("usb_endp_default_handler")));

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
    uint8_t status;
    uint8_t stat, endpoint;

    status = USB0_ISTAT;

    if (status & USB_ISTAT_USBRST_MASK)
    {
        //handle USB reset

        //initialize endpoint 0 ping-pong buffers
        USB0_CTL |= USB_CTL_ODDRST_MASK;
        endp0_odd = 0;
        table[BDT_INDEX(0, RX, EVEN)].desc = BDT_DESC(ENDP0_SIZE, 0);
        table[BDT_INDEX(0, RX, EVEN)].addr = endp0_rx[0];
        table[BDT_INDEX(0, RX, ODD)].desc = BDT_DESC(ENDP0_SIZE, 0);
        table[BDT_INDEX(0, RX, ODD)].addr = endp0_rx[1];
        table[BDT_INDEX(0, TX, EVEN)].desc = 0;
        table[BDT_INDEX(0, TX, ODD)].desc = 0;

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
        handlers[endpoint & 0xf](stat);

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
