/*
 *  Teensy31_flash.ld      generic linker script for Teensy 3.1 flash-based projects
 */
 
 
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_startup)

/*
 *  SRAM is split into two contiguous blocks of equal size.  SRAM_L
 *  is the lower half of RAM and SRAM_H is the higher half.
 *
 *  For devices with 64K, the low 32K will appear 0x1fff8000 to 0x2000000
 *  and the high 32K will appear 0x20000000 to 0x20007fff.
 */
MEMORY
{
    sram (W!RX) : ORIGIN = 0x1fff8000, LENGTH = 64K
    flash (RX)  : ORIGIN = 0x00000000, LENGTH = 256K
}

/* Define the top our stack at the end of SRAM */
TOTAL_RESERVED_STACK = 8196;		/* note that printf() and other stdio routines use 4K+ from stack! */
_top_stack = (0x1fff8000+64K);	    /* calc top of stack */

/*
 *  Define the amount of heap space to reserve.
 */
TOTAL_RESERVED_HEAP = 0;


EXTERN(__interrupt_vector_table);

SECTIONS
{
	.text :
	{
		CREATE_OBJECT_SYMBOLS
		/* Insert the interrupt vector table first */
		__interrupt_vector_table = .;
		*(.interrupt_vector_table)

		/* Startup assembly */
		*(.startup)

		/* Rest of the code (C) */
		*(.text)
		*(.text.*)
		*(.glue_7)
		*(.glue_7t)

/*  Added following section for holding initializers for variables
 *  that will be accessed from RAM; see also the AT(_end_data_flash)
 *  usage below.
 *
 *  The _data_size value will be used in the startup code to step through
 *  the image of data in flash and copy it to RAM.
 */
		. = ALIGN(4);
		*(.rodata)
		*(.rodata*)
		*(.init)					/* added */
		*(.fini)					/* added */
		. = ALIGN(4);
		_end_data_flash = .;
	} >flash

/*
 *  ------------------- Start of SRAM sections ---------------
 */
 
/*
 *  If moving vectors from flash to RAM, declare a reserved area for
 *  the RAM vector table.  This section should appear first, so it gets
 *  assigned to the first available SRAM address.  Due to requirements
 *  of the NVIC subsystem, this address MUST be on a 1024-byte
 *  boundary.
 */
	.RAMVectorTable :
	{
		*(.RAMVectorTable)
	}
	. = ALIGN(4);
	
			
  /*  From generic.ld, supplied by CodeSourcery  */
  /* .ARM.exidx is sorted, so has to go in its own output section.  */
	PROVIDE_HIDDEN (__exidx_start = .);
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} >sram  
	PROVIDE_HIDDEN (__exidx_end = .);

	
/*	.data : AT (_end_data_flash)   */
	.data :
	{
		_start_data_flash = LOADADDR(.data);
		_start_data = .;
		*(.data)
		*(.data.*)
		*(.shdata)
		_end_data = .;
	} >sram  AT>flash
	. = ALIGN(4);
	_data_size = _end_data - _start_data;

	.noinit :
	{
		*(.noinit)
		*(.noinit.*)
	} >sram
	
	_start_bss = .;
	.bss :
	{
		*(.bss)
		*(.bss.*)
		*(COMMON)
	} >sram
	. = ALIGN(4);
	PROVIDE(_end_bss = .);				/* make value of _end_bss available externally */

	bss_size = _end_bss - _start_bss;

	/* Stack can grow down to here, right after data and bss sections in 
	 * SRAM */
	_start_stack = _top_stack - TOTAL_RESERVED_STACK;
	_top_stack = _top_stack;			/* just to make the map file easier to read */


/*
 *  If you want a heap, declare it here.
 */
/*
  	.heap :
 	{
 		PROVIDE(_startHeap = .);
 		*(.heap)
 		. += TOTAL_RESERVED_HEAP;
 		. = ALIGN(4);
 		PROVIDE(_endHeap = .);
 	} > ahbsram1
*/
 


	/* Linker wants .eh_frame section defined because of gcc 4.4.X bug,
	 * just discard it here. */
	/DISCARD/ :
	{
		*(.eh_*)
	}
	
}

_end = .;
PROVIDE(end = .);

