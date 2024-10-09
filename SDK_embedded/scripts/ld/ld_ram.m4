OUTPUT_FORMAT ("elf32-littlearm")
ENTRY(ENTRY_SYMBOL)
MEMORY
{
  flash   (rx)  : ORIGIN = FLASH_ORIGIN, LENGTH = FLASH_SIZE - EEPROM_SIZE
  eeprom (r)	: ORIGIN = FLASH_ORIGIN + LENGTH(flash), LENGTH = EEPROM_SIZE
  ram    (rwx)  : ORIGIN = RAM_ORIGIN,   LENGTH = RAM_SIZE
}

/* define a global symbol _stack_end  */
_stack_end_ = ORIGIN(ram) + LENGTH(ram) - 1 - STACK_END_OFFSET ;

/* now define the output sections  */
SECTIONS 
{
	. = 0;	
								/* set location counter to address zero  */
	.text :								
	{
		_image_start_  = . ;
		_vec_start_ = . ;
		KEEP(*(.flash_vec_table*))
		_vec_end_ = ALIGN( . , 4) ;
		
		_text_start_ = _vec_end_ ;
		/* ����� ����������� ������������� */
		__preinit_array_start = . ;
		KEEP(*(.preinit_array*))
		__preinit_array_end = . ;
		__init_array_start  = . ;
		KEEP(*(.init_array*))
		__init_array_end    = . ;
		
		*(.crt*)
		*(.rodata*)						/* all .rodata* sections (constants, strings, etc.)  */
		*(.glue_7*)						/* all .glue_7 sections  (no idea what these are) */
		*(.const_data*)					/* const data tables */
		*(.glue_7t*)						/* all .glue_7t sections (no idea what these are) */
		*(.gnu*)
		*(.gcc*)
		*/
	
		/*    �� ������������ �� ������������ ��������
		__fini_array_start = . ;
		KEEP(*(.fini_array*))
		__fini_array_end   = . ;
		*/
               
		. = ALIGN(4) ;
		_text_end_ = .;					/* define a global symbol _etext just after the last code byte */
	} >flash							/* put all the above into FLASH */

	.eeprom_emul :
	{
		_eeprom_start_  = . ;
		*(.eeprom*)
		_eeprom_stop_  = . ;
	} > eeprom

	_data_load_start_ = _text_end_ ;
	.data :								/* collect all initialized .data sections that go into RAM  */ 
	{
		_ram_vec_start_ = .;  /* area for STM32 in-RAM mapped IRQ vector table, not need initialize in startup*/
		*(.ram_vec_table*)			
		_ram_vec_end_ = ALIGN( . , 4) ;		
		
		_data_start_ = .;						/* create a global symbol marking the start of the .data section  */
		*(.data*)
		*(.text*)						/* all .text sections (code)  */
		_data_end_ = ALIGN( . , 4);
		
							/* define a global symbol marking the end of the .data section  */
	} >ram AT >flash					/* put all the above into RAM (but load the LMA copy into FLASH) */
	_data_load_end_ = _data_load_start_ +  SIZEOF(.data) ;	
   

	.bss (NOLOAD) :								 
	{
		_bss_start_ = _data_end_ ;  /*initialize in startup for zero fill of gloabal objects*/		  
		*(.bss*)
		_bss_end_ = .;
          
	}  >ram


   /DISCARD/ :
    {
     libc.a ( * )
     libm.a ( * )
     libgcc.a ( * )
     *(.ARM.exidx*)
	 *(.ARM.extab*)
	
    }	

    /* LTO support sections */
	.gnu.lto_ :
	{
		*(.gnu.lto_)
		*(.gnu.lto_*)
	}
     
    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
}
