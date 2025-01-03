OUTPUT_FORMAT ("elf32-littlearm")
ENTRY(ENTRY_SYMBOL)
MEMORY
{
  
  ccm_ram (rwx)	: ORIGIN = CCM_RAM_ORIGIN,   LENGTH = CCM_RAM_SIZE
  ram    (rwx)  : ORIGIN = RAM_ORIGIN,   LENGTH = RAM_SIZE
    
  ifdef(`EXT_MEM_BANK0_SIZE', `ext_mem_bank0  (rwx)  : ORIGIN = EXT_MEM_BANK0_ORIGIN ,   LENGTH = EXT_MEM_BANK0_SIZE', `' )
  ifdef(`EXT_MEM_BANK1_SIZE', `ext_mem_bank1  (rwx)  : ORIGIN = EXT_MEM_BANK1_ORIGIN ,   LENGTH = EXT_MEM_BANK1_SIZE', `' )
  ifdef(`EXT_MEM_BANK2_SIZE', `ext_mem_bank2  (rwx)  : ORIGIN = EXT_MEM_BANK2_ORIGIN ,   LENGTH = EXT_MEM_BANK2_SIZE', `' )
  ifdef(`EXT_MEM_BANK3_SIZE', `ext_mem_bank3  (rwx)  : ORIGIN = EXT_MEM_BANK3_ORIGIN ,   LENGTH = EXT_MEM_BANK3_SIZE', `' )

  flash  (rx)  : ORIGIN = FLASH_ORIGIN + FLASH_TEXT_SECTION_OFFSET, LENGTH = FLASH_SIZE - FLASH_TEXT_SECTION_OFFSET - ALIGN( EEPROM_SIZE , 4 )
  eeprom (rw)	: ORIGIN = FLASH_ORIGIN + FLASH_TEXT_SECTION_OFFSET + LENGTH(flash), LENGTH = ALIGN( EEPROM_SIZE , 4 )

}

/* define a global symbol _stack_end  */
_stack_end_ = ORIGIN(ram) + LENGTH(ram) - 4 - STACK_END_OFFSET ;

__FLASH_start__ = FLASH_ORIGIN;
__FLASH_end__ = __FLASH_start__ + FLASH_SIZE ;

__SRAM_start__ = RAM_ORIGIN ;
__SRAM_end__ = __SRAM_start__ + RAM_SIZE;

__CCM_SRAM_start__ = CCM_RAM_ORIGIN ;
__CCM_SRAM_end__ = __CCM_SRAM_start__ + CCM_RAM_SIZE ;


/* now define the output sections  */
SECTIONS 
{
	. = 0;	
								/* set location counter to address zero  */
	.text :								
	{
		__image_start__  = . ;
		__vec_start__ = . ;
		KEEP(*(.flash_vec_table*))
		KEEP(*(.flash_vec_table))
		__vec_end__ = . ;
		
		__sunset_start__ = . ;
                KEEP(*(.sunset*))
		__sunset_end__ = . ;
		
		__code_start__ = . ;
		
		__preinit_array_start__ = . ;
		KEEP(*(SORT(.preinit_array*)))
		KEEP(*(.preinit_array))
		__preinit_array_end__ = . ;
		__init_array_start__  = . ;
		KEEP(*(SORT(.init_array*)))
		KEEP(*(.init_array))
		__init_array_end__    = . ;
		__fini_array_start__ = . ;
		KEEP(*(SORT(.fini_array*)))
		KEEP(*(.fini_array))
		__fini_array_end__   = . ;
		
		__privileged_functions_start__ = . ;
		*(.privileged_functions*)
		__privileged_functions_end__ = . ;
		
		__text_start__ = . ;
		*(.text*)						/* all .text sections (code)  */
		*(.text)
		__text_end__ = . ;
		__rodata_start__ = .;
		*(.rodata*)						/* all .rodata* sections (constants, strings, etc.)  */
		__rodata_end__ = .;
		__const_data_start__ = . ;
		*(.const_data*)					/* const data tables */
		__const_data_end__ = . ;
		*(.glue_7)						/* all .glue_7 sections  (no idea what these are) */
		*(.glue_7t)						/* all .glue_7t sections (no idea what these are) */
		*(.gnu*)
		*(.gcc*)
		. = ALIGN(16) ;
		__code_end__ = . ;

	} >flash

	.eeprom_emul :
	{
		__eeprom_start__  = . ;
		*(.eeprom)
		*(.eeprom*)
		__eeprom_stop__  = . ;
	} > eeprom

	
	.ccm_bss (NOLOAD):
	{
		__ccm_privileged_data_start__ = .;
		*(ccm_privileged_data*)
		__ccm_privileged_data_end__  = .;

		__ccm_bss_start__ = . ;     
		*(.ccm_bss*)
		__ccm_bss_end__ = .;
	} >ccm_ram
	
	__ccm_data_load_start__ = __code_end__ ;
	.ccm_data :								/* collect all initialized .data sections that go into RAM  */ 
	{
		. = ALIGN(4) ;
		__ccm_data_start__ = . ;
		*(.ccm_data*) 						/* all .ccm_data sections  */
		. = ALIGN(4);
		__ccm_data_end__ = .;							/* define a global symbol marking the end of the .data section  */
	} >ccm_ram AT >flash					     /* put all the above into RAM (but load the LMA copy into FLASH) */
	__ccm_data_load_end__ = __ccm_data_load_start__ +  SIZEOF(.ccm_data) ;	

	.bss (NOLOAD):								 
	{
		__ram_vec_start__ = .;
		*(.ram_vec_table*)	
		__ram_vec_end__ = .;

		__privileged_data_start__ = .;
		*(privileged_data*)
		__privileged_data_end__  = .;

		__bss_start__ = . ;     
		*(.bss*)
		*(COMMON)
		__bss_end__ = .;
	}  >ram
		
	__data_load_start__ = __ccm_data_load_end__ ;
	.data :								/* collect all initialized .data sections that go into RAM  */ 
	{
		. = ALIGN(4) ;
		__data_start__ = . ;
		
		__ramfunc_start__ = .;
		*(.ramfunc*)
		__ramfunc_end__ = . ;
		. = ALIGN(4) ;  
		
		*(.data*) 						/* all .data sections  */
		*(.data)
		. = ALIGN(4);
		__data_end__ = .;							/* define a global symbol marking the end of the .data section  */
	} >ram AT >flash					     /* put all the above into RAM (but load the LMA copy into FLASH) */
	__data_load_end__ = __data_load_start__ +  SIZEOF(.data) ;	
	 
	  ifdef(`EXT_MEM_BANK0_SIZE', 
	 	 `/* external memory bank 0 layout  */
   .ext_mem_bank0_bss (NOLOAD):								 
	{
		. = ALIGN(4) ;
		__ext_mem_bank0_bss_start__ = . ;     
		*(.ext_mem_bank0_bss*)
		. = ALIGN(4) ;
		__ext_mem_bank0_bss_end__ = . ;
	}  >ext_mem_bank0
	
	__ext_mem_bank0_data_load_start__ = __data_load_end__ ;
	.ext_mem_bank0_data :								 
	{
		. = ALIGN(4) ;
		__ext_mem_bank0_data_start__ = . ;
		*(.ext_mem_bank0_data*)
		. = ALIGN(4) ;
		__ext_mem_bank0_data_end__ = .;	
	}  >ext_mem_bank0 AT >flash
	__ext_mem_bank0_data_load_end__ = __ext_mem_bank0_data_load_start__ + SIZEOF(.ext_mem_bank0_data) ;
	',
	`	/*__ext_mem_bank0_data_load_end__ = __data_load_end__  ;*/'
    ) 
    
    ifdef(`EXT_MEM_BANK1_SIZE', 
	 	 `/* external memory bank 1 layout  */
   .ext_mem_bank1_bss (NOLOAD):								 
	{
		. = ALIGN(4) ;
		__ext_mem_bank1_bss_start__ = . ;     
		*(.ext_mem_bank1_bss*)
		. = ALIGN(4) ;
		__ext_mem_bank1_bss_end__ = . ;
	}  >ext_mem_bank1
	
	__ext_mem_bank1_data_load_start__ = __ext_mem_bank0_data_load_end__ ;
	.ext_mem_bank1_data :								 
	{
		. = ALIGN(4) ;
		__ext_mem_bank1_data_start__ = . ;
		*(.ext_mem_bank1_data*)
		. = ALIGN(4) ;
		__ext_mem_bank1_data_end__ = .;
	}  >ext_mem_bank1 AT >flash
	__ext_mem_bank1_data_load_end__ = __ext_mem_bank1_data_load_start__ + SIZEOF(.ext_mem_bank1_data) ;
	',
	 `	/*__ext_mem_bank1_data_load_end__ = __ext_mem_bank0_data_load_end__  ;*/'
    ) 
    
    ifdef(`EXT_MEM_BANK2_SIZE', 
	 	 `/* external memory bank 2 layout  */
   .ext_mem_bank2_bss (NOLOAD):								 
	{
		. = ALIGN(4) ;
		__ext_mem_bank2_bss_start__ = . ;     
		*(.ext_mem_bank2_bss*)
		. = ALIGN(4) ;
		__ext_mem_bank2_bss_end__ = . ;
	}  >ext_mem_bank2
	
	__ext_mem_bank2_data_load_start__ = __ext_mem_bank1_data_load_end__ ;
	.ext_mem_bank2_data :								 
	{
		. = ALIGN(4) ;
		__ext_mem_bank2_data_start__ = . ;
		*(.ext_mem_bank2_data*)
		. = ALIGN(4) ;
		__ext_mem_bank2_data_end__ = .;
	}  >ext_mem_bank2 AT >flash
	__ext_mem_bank2_data_load_end__ = __ext_mem_bank2_data_load_start__ + SIZEOF(.ext_mem_bank2_data) ;
	',
	 `	/*__ext_mem_bank2_data_load_end_ = _ext_mem_bank1_data_load_end__ ;*/'
    ) 
	
	ifdef(`EXT_MEM_BANK3_SIZE', 
	 	 `/* external memory bank 3 layout  */
   .ext_mem_bank3_bss (NOLOAD):								 
	{
		. = ALIGN(4) ;
		__ext_mem_bank3_bss_start__ = . ;     
		*(.ext_mem_bank3_bss*)
		. = ALIGN(4) ;
		__ext_mem_bank3_bss_end__ = . ;
	}  >ext_mem_bank3
	
	__ext_mem_bank3_data_load_start__ = __ext_mem_bank2_data_load_end__ ;
	.ext_mem_bank3_data :								 
	{
		. = ALIGN(4) ;
		__ext_mem_bank3_data_start__ = . ;
		*(.ext_mem_bank3_data*)
		. = ALIGN(4) ;
		__ext_mem_bank3_data_end__ = .;
	}  >ext_mem_bank3 AT >flash
	__ext_mem_bank3_data_load_end__ = __ext_mem_bank3_data_load_start__ + SIZEOF(.ext_mem_bank3_data) ;
	',
	 `	/*__ext_mem_bank3_data_load_end__ = __ext_mem_bank2_data_load_end__ ;*/'
    ) 

	 . = ALIGN(4) ;
	__image_end__ = .;
	

	 
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
		

    .igot          0 : { *(.igot*) } 
    .vfp11_veneer  0 : { *(.vfp11_veneer*) } 
    .v4_bx         0 : { *(.v4_bx*) } 
    .iplt          0 : { *(.iplt*) } 
    .rel           0 : { *(.rel*) } 
     
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
