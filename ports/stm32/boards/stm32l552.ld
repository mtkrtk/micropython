/*
    GNU linker script for STM32L552
*/

/* Specify the memory areas */
MEMORY
{
    /* FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 512K */
    FLASH_ISR (rx)  : ORIGIN = 0x08029000, LENGTH = 16K
    FLASH_FS (rx)   : ORIGIN = 0x0802D000, LENGTH = 32K
    FLASH_TEXT (rx) : ORIGIN = 0x08035000, LENGTH = 300K
    RAM (xrw)       : ORIGIN = 0x20018000, LENGTH = 160K

    /*FLASH_ISR (rx)  : ORIGIN = 0x08000000, LENGTH = 16K
    FLASH_FS (rx)   : ORIGIN = 0x08004000, LENGTH = 112K
    FLASH_TEXT (rx) : ORIGIN = 0x08020000, LENGTH = 384K
    RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 256K*/
}

/* produce a link error if there is not this amount of RAM for these sections */
_minimum_stack_size = 2K;
_minimum_heap_size = 16K;

/* RAM extents for the garbage collector */
_ram_start = ORIGIN(RAM);
_ram_end = ORIGIN(RAM) + LENGTH(RAM);

_ram_fs_cache_end = _ram_end;
_ram_fs_cache_start = _ram_fs_cache_end - 2K; /* fs cache = 2K */

_flash_fs_start = ORIGIN(FLASH_FS);
_flash_fs_end   = ORIGIN(FLASH_FS) + LENGTH(FLASH_FS);

/* Define the stack.  The stack is full descending so begins just above last byte
   of RAM.  Note that EABI requires the stack to be 8-byte aligned for a call. */
_estack = _ram_fs_cache_start - _estack_reserve;
_sstack = _estack - 16K; /* tunable */

_heap_start = _ebss; /* heap starts just after statically allocated memory */
_heap_end = _sstack;
