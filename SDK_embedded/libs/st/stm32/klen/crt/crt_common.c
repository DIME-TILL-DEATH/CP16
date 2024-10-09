#include "misc.h" // defines of NVIC_SetVectorTable


// this source is included to port version crt 
void ResetHandler(void)  __attribute__((/*naked*/,noreturn)) ; // comment naked attr for workaround compiler fault


// c++ error handler , that is invoked when a pure virtual function is called
void __attribute__((weak,noreturn))  __cxa_pure_virtual()
{
	while(1) {}
}

// c++ error handler , that is invoked when in function calls exceptions caused
void __attribute__((weak,noreturn,used))  abort()
{
	while(1) {}
}

//  normal main() return handler
void __attribute__((weak,noreturn)) __main_exit_handler(int retval)
{
  (void)  retval ;
  while(1) {}
}

// _init stub function
void __attribute__ ((weak)) _init(void)
{
}
// _fini stub function
void __attribute__ ((weak)) _fini(void)
{
}

void __attribute__((weak)) DefaultExceptionHandler(void)
{
  while(1)
    {
      asm volatile  ("nop");
    }
}
//-------------------------------------------------------------------------
void __attribute__((weak)) assert_failed(uint8_t* file, uint32_t line)
{
  while(1)
    {
      asm volatile  ("nop");
    }
}
//-------------------------------------------------------------------------
void __attribute__((weak, noreturn)) exit(int status)
{
  while(1)
    {
      asm volatile  ("nop");
    }
}
//-------------------------------------------------------------------------
void __attribute__((weak,used))  hard_fault_handler_c(uint32_t* hardfault_args)
{
	volatile uint32_t stacked_r0 =  hardfault_args[0];
	volatile uint32_t stacked_r1 =  hardfault_args[1];
	volatile uint32_t stacked_r2 =  hardfault_args[2];
	volatile uint32_t stacked_r3 =  hardfault_args[3];
	volatile uint32_t stacked_r12 = hardfault_args[4];
	volatile uint32_t stacked_lr =  hardfault_args[5];
	volatile uint32_t stacked_pc =  hardfault_args[6];
	volatile uint32_t stacked_psr = hardfault_args[7];

	register volatile uint8_t trigger = 1 ;

	volatile uint32_t scb_vtor = SCB->VTOR ; // wiev vector location
	(void)scb_vtor ;
	while (trigger)
  		{
			asm volatile  ("nop");
  		}

  	asm volatile ("ldr r0,  %0" : : "m"(stacked_r0) : ) ;
  	asm volatile ("ldr r1,  %0" : : "m"(stacked_r1) : ) ;
  	asm volatile ("ldr r2,  %0" : : "m"(stacked_r2) : ) ;
  	asm volatile ("ldr r3,  %0" : : "m"(stacked_r3) : ) ;
  	asm volatile ("ldr r12, %0" : : "m"(stacked_r12) : ) ;
  	asm volatile ("ldr lr,  %0" : : "m"(stacked_lr) : ) ;
  	asm volatile ("ldr pc,  %0" : : "m"(stacked_pc) : ) ;


	(void) stacked_r0 ;
	(void) stacked_r1 ;
	(void) stacked_r2 ;
	(void) stacked_r3 ;
	(void) stacked_r12 ;
	(void) stacked_lr ;
	(void) stacked_pc ;
	(void) stacked_psr ;
}
//-------------------------------------------------------------------------
void __attribute__((weak)) HardFaultException(void)
{
  // read PSP and save return adress from stack
  asm volatile (
      "TST LR, #4              \n"
      "ITE EQ                  \n"
      "MRSEQ R0, MSP           \n"
      "MRSNE R0, PSP           \n"
      "B hard_fault_handler_c" );
} ;
//-------------------------------------------------------------------------

#if defined __USE_RAM_VEC_TABLE__
IrqHandlerFunc __attribute__ ((externally_visible,section(".ram_vec_table")))
ram_vec_table [ sizeof(flash_vec_table) / sizeof(IrqHandlerFunc)] ;

void vec_table_copy2ram(unsigned map_needed )
{
  for (uint32_t vec_index = 0 ; vec_index <  sizeof(flash_vec_table) / sizeof(IrqHandlerFunc) ; vec_index++ )
      ram_vec_table[vec_index] = flash_vec_table[vec_index] ;

  if ( map_needed )
    vec_map2ram (0) ;
}
//---------------------------------------------------------------------
void vec_set (  VectorType vec_type  , void* handler )
{
  ram_vec_table[vec_type] =  (IrqHandlerFunc)handler ;
}

void vec_map2ram ( unsigned* vec_table )
{
  NVIC_SetVectorTable(NVIC_VectTab_RAM, (unsigned)(vec_table - SRAM_BASE) ) ;
}
//-------------------------------------------------------------------
inline void vec_map2flash ( unsigned* vec_table )
{
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, (unsigned)(vec_table - FLASH_BASE) ) ;
}
//-------------------------------------------------------------------

#endif //__USE_RAM_VEC_TABLE__
//---------------------------------------------------------------------
void crt_init()
{
   // fill  all internal memory for dummy pattern
   #ifndef FILL_RAM_PATTRERN
               #define FILL_RAM_PATTRERN 0x12345678
   #endif
   extern unsigned long  __ram_vec_start__ ;
   unsigned long* ram = &__ram_vec_start__ ;
   unsigned long* ram_end  ;
   asm volatile ("mov %0 , sp \n" : "=r"(ram_end) : : );
   while( ram < ram_end )
          {
           *(ram++) = FILL_RAM_PATTRERN;
          }

  // init .ccm_data section
  extern unsigned long  __ccm_data_load_start__ ;
  extern unsigned long  __ccm_data_start__ ;
  extern unsigned long  __ccm_data_end__ ;
  unsigned long* data_load = &__ccm_data_load_start__ ;
  unsigned long* data = &__ccm_data_start__ ;
  unsigned long* data_end = &__ccm_data_end__ ;
  while( data < data_end )
            {
             *(data++) = *(data_load++);
            }


  // init .data section
  extern unsigned long  __data_load_start__ ;
  extern unsigned long  __data_start__ ;
  extern unsigned long  __data_end__ ;
  data_load = &__data_load_start__ ;
  data = &__data_start__ ;
  data_end = &__data_end__ ;
  while( data < data_end )
         {
          *(data++) = *(data_load++);
         }

  // init .ccm_bss section
  extern unsigned long  __ccm_bss_start__ ;
  extern unsigned long  __ccm_bss_end__   ;
  unsigned long* bss = &__ccm_bss_start__ ;
  unsigned long* bss_end = &__ccm_bss_end__ ;
  while(bss < bss_end )
   {
    *(bss++) = 0 ;
   }

  // init .bss section
  extern unsigned long  __bss_start__ ;
  extern unsigned long  __bss_end__   ;
  bss = &__bss_start__ ;
  bss_end = &__bss_end__ ;
  while(bss < bss_end )
   {
    *(bss++) = 0 ;
   }

#ifdef __EXT_MEM_BANK0__

  // init external memory bank0 interface and device, user defined code
  ext_mem_bank0_init();

  // init .data section on ext_mem_bank0
  extern unsigned long  __ext_mem_bank0_data_load_start__ ;
  extern unsigned long  __ext_mem_bank0_data_start__ ;
  extern unsigned long  __ext_mem_bank0_data_end__ ;
  unsigned long* ext_mem_bank0_data_load = &__ext_mem_bank0_data_load_start__ ;
  unsigned long* ext_mem_bank0_data = &__ext_mem_bank0_data_start__ ;
  unsigned long* ext_mem_bank0_data_end = &__ext_mem_bank0_data_end__ ;
  while( ext_mem_bank0_data < ext_mem_bank0_data_end )
    {
      *(ext_mem_bank0_data++) = *(ext_mem_bank0_data_load++);
    }

  // init .bss section
  extern unsigned long  __ext_mem_bank0_bss_start__ ;
  extern unsigned long  __ext_mem_bank0_bss_end__   ;
  unsigned long* ext_mem_bank0_bss = &__ext_mem_bank0_bss_start__ ;
  unsigned long* ext_mem_bank0_bss_end = &__ext_mem_bank0_bss_end__ ;
  while(ext_mem_bank0_bss < ext_mem_bank0_bss_end )
    {
      *(ext_mem_bank0_bss++) = 0 ;
    }
#endif

#ifdef __EXT_MEM_BANK1__

  // init external memory bank1 interface and device, user defined code
  ext_mem_bank1_init();

  // init .data section on ext_mem_bank1
  extern unsigned long  __ext_mem_bank1_data_load_start_ ;
  extern unsigned long  __ext_mem_bank1_data_start__ ;
  extern unsigned long  __ext_mem_bank1_data_end__ ;
  unsigned long* ext_mem_bank1_data_load = &__ext_mem_bank1_data_load_start__ ;
  unsigned long* ext_mem_bank1_data = &__ext_mem_bank1_data_start__ ;
  unsigned long* ext_mem_bank1_data_end = &__ext_mem_bank1_data_end__ ;
  while( ext_mem_bank1_data < ext_mem_bank1_data_end )
    {
      *(ext_mem_bank1_data++) = *(ext_mem_bank1_data_load++);
    }

  // init .bss section
  extern unsigned long  __ext_mem_bank1_bss_start__ ;
  extern unsigned long  __ext_mem_bank1_bss_end__   ;
  unsigned long* ext_mem_bank1_bss = &__ext_mem_bank1_bss_start__ ;
  unsigned long* ext_mem_bank1_bss_end = &__ext_mem_bank1_bss_end__ ;
  while(ext_mem_bank1_bss < ext_mem_bank1_bss_end )
    {
      *(ext_mem_bank1_bss++) = 0 ;
    }
#endif

#ifdef __EXT_MEM_BANK2__

  // init external memory bank2 interface and device, user defined code
  ext_mem_bank2_init();

  // init .data section on ext_mem_bank2
  extern unsigned long  __ext_mem_bank2_data_load_start__ ;
  extern unsigned long  __ext_mem_bank2_data_start__ ;
  extern unsigned long  __ext_mem_bank2_data_end__ ;
  unsigned long* ext_mem_bank2_data_load = &__ext_mem_bank2_data_load_start__ ;
  unsigned long* ext_mem_bank2_data = &__ext_mem_bank2_data_start__ ;
  unsigned long* ext_mem_bank2_data_end = &__ext_mem_bank2_data_end__ ;
  while( ext_mem_bank2_data < ext_mem_bank2_data_end )
    {
      *(ext_mem_bank2_data++) = *(ext_mem_bank2_data_load++);
    }

  // init .bss section
  extern unsigned long  __ext_mem_bank2_bss_start__ ;
  extern unsigned long  __ext_mem_bank2_bss_end__   ;
  unsigned long* ext_mem_bank2_bss = &__ext_mem_bank2_bss_start__ ;
  unsigned long* ext_mem_bank2_bss_end = &__ext_mem_bank2_bss_end__ ;
  while(ext_mem_bank2_bss < ext_mem_bank2_bss_end )
    {
      *(ext_mem_bank2_bss++) = 0 ;
    }
#endif

#ifdef __EXT_MEM_BANK3__

  // init external memory bank3 interface and device, user defined code
  ext_mem_bank3_init();

  // init .data section on ext_mem_bank3
  extern unsigned long  __ext_mem_bank3_data_load_start__ ;
  extern unsigned long  __ext_mem_bank3_data_start__ ;
  extern unsigned long  __ext_mem_bank3_data_end__ ;
  unsigned long* ext_mem_bank3_data_load = &__ext_mem_bank3_data_load_start__ ;
  unsigned long* ext_mem_bank3_data = &__ext_mem_bank3_data_start__ ;
  unsigned long* ext_mem_bank3_data_end = &__ext_mem_bank3_data_end__ ;
  while( ext_mem_bank3_data < ext_mem_bank3_data_end )
    {
      *(ext_mem_bank3_data++) = *(ext_mem_bank3_data_load++);
    }

  // init .bss section
  extern unsigned long  __ext_mem_bank3_bss_start__ ;
  extern unsigned long  __ext_mem_bank3_bss_end__   ;
  unsigned long* ext_mem_bank3_bss = &__ext_mem_bank3_bss_start__ ;
  unsigned long* ext_mem_bank3_bss_end = &__ext_mem_bank3_bss_end__ ;
  while(ext_mem_bank3_bss < ext_mem_bank3_bss_end )
    {
      *(ext_mem_bank3_bss++) = 0 ;
    }
#endif

}
//---------------------------------------------------------------------
void ResetHandler(void)
{
	// init PLLs, RCC , Vector Table Relocation in Internal FLASH
	void SystemInit(void);
	SystemInit();

	// delay for GDB connect
	#ifndef DELAY_FOR_GDB
		#define DELAY_FOR_GDB 10000
	#endif

	volatile unsigned delay_counter = DELAY_FOR_GDB ;
	while ( delay_counter-- ) ;

	// fill memory and initialize .bss , .data  sections
	crt_init();

	// switch vec table to ram
	#if defined __USE_RAM_VEC_TABLE__
		vec_table_copy2ram( 1 ) ;
	#endif //RAM_VEC_TABLE

	// вызов конструкторов глобальных объектов
	// и функций с атрибутом constructor
	void __libc_init_array(void);
	__libc_init_array() ;

	// вызов основной функции


	int main (void) ;	int retval = main();

	// вызов деструкторов  глобальных объектов
	// и функций с атрибутом destructor
	void __libc_fini_array(void);
	__libc_fini_array() ;

	// call exit function
	__main_exit_handler(retval);

}


