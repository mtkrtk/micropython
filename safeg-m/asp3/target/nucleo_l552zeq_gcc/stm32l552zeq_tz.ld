MEMORY
{
  FLASH (rx)      : ORIGIN = 0x0C000000, LENGTH = 160K
  SRAM (xrw)      : ORIGIN = 0x30000000, LENGTH = 96K
  ROM_NSC(rx)     : ORIGIN = 0x0C028000, LENGTH = 4K  
}

PROVIDE(hardware_init_hook = 0);
PROVIDE(software_init_hook = 0);
PROVIDE(software_term_hook = 0);

ENTRY(_kernel_start)

SECTIONS
{
	.text :
	{
		__text = .;
		*(.vector)
		*(.text)
		*(.text.*)
	} > FLASH
	_etext = .;
	PROVIDE(etext = .);

	.rodata :
	{
		*(.rodata)
		*(.rodata.*)
	} > FLASH

	.ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) }
	__exidx_start = .;
	.ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
	__exidx_end = .;

	. = ALIGN(4);
	__idata_start = .;
	.data : 
	{
		__data_start = .;
		*(.data)
		*(.data.*)
	} > SRAM AT > FLASH
	__idata_end = __idata_start + SIZEOF(.data);
	_edata = .;
	PROVIDE(edata = .);

	.gnu.sgstubs :
	{
		. = ALIGN(8);
		*(.gnu.sgstubs*)   /* Secure Gateway stubs */
		. = ALIGN(8);
	} >ROM_NSC

	. = ALIGN(8);
	.bss :
	{
		__bss_start  = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
	} > SRAM
	. = ALIGN(4);
	_end = .;
	__bss_end = .;
	PROVIDE(end = .);
}
