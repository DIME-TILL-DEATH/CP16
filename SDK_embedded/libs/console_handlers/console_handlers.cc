#include "console_handlers.h"

#include "mmgr.h"
#include "rt_counter.h"
#include "portmacro.h"
#include "newlib.h"
#include "arch/unique_id.h"

//------------------------------------------------------------------------------
static void test_console_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	for ( size_t i = 0 ; i < count ; i++ )
		msg_console ( "\t%s\n" , args[i] ) ;
}
//------------------------------------------------------------------------------
static void reset_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
        NVIC_SystemReset();
}

//------------------------------------------------------------------------------
static void help_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	TReadLine::command_handler_map_iterator_t current ;
	TReadLine::command_handler_map_iterator_t end ;
	rl->GetCommandHandlerMapIterator( current, end );

	 while ( current!=end)
		 msg_console ("\t%s\n" , (current++)->first );
}
//------------------------------------------------------------------------------
static void version_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console("target board: %s\n" , DEV_DESCRIPTION ) ;
	msg_console("firmware:     %s\n" , PROJECT_DESCRIPTION ) ;
	msg_console("os: FreeRTOS " tskKERNEL_VERSION_NUMBER "\n");
	msg_console("compiler: GCC " __VERSION__ "\n");
	msg_console("GLIBCXX: %d\n", __GLIBCXX__ ) ;
	msg_console("newlib: " _NEWLIB_VERSION "\n"  ) ;
}
//------------------------------------------------------------------------------

static void dev_inf_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
          msg_console("\theap ptr ... 0x%x\n" , heap_ptr() ) ;
	  msg_console("\theap control struct size ... %u bytes\n" , heap_control_stuct_size()) ;
	  msg_console("\theap align size ...          %u bytes\n" , heap_align_size()) ;
	  msg_console("\theap block size_min ...      %u bytes\n" , heap_block_size_min()) ;
	  msg_console("\theap block size_max ...      %u bytes\n" , heap_block_size_max()) ;
	  msg_console("\theap pool overhead  ...      %u bytes\n" , heap_pool_overhead()) ;
	  msg_console("\theap alloc overhead ...      %u bytes\n" , heap_alloc_overhead()) ;

	  RCC_ClocksTypeDef rrc ;
	  RCC_GetClocksFreq ( &rrc) ;
	  msg_console("\tSYSCLK  clock  ... %6f MHz\n" , 1.0f*rrc.SYSCLK_Frequency  / 1.0e6 ) ;
	  msg_console("\tHCLK    clock  ... %6f MHz\n" , 1.0f*rrc.HCLK_Frequency  / 1.0e6 ) ;
	  msg_console("\tPCLK1   clock  ... %6f MHz\n" , 1.0f*rrc.PCLK1_Frequency / 1.0e6 ) ;
	  msg_console("\tPCLK2   clock  ... %6f MHz\n" , 1.0f*rrc.PCLK2_Frequency / 1.0e6 ) ;
	  //rmsg("\tADCCLK2 clock  ... %6f MHz\n" , 1.0f*rrc.ADCCLK_Frequency / 1.0e6 ) ;


	  extern uint32_t  _stack_end_ ;
	  msg_console("\n\tMain stack end addr: 0x%x\n" , (uint8_t*)&_stack_end_ ) ;


	  extern uint32_t  __FLASH_start__ ;
	  extern uint32_t  __FLASH_end__ ;
	  msg_console("\tMemory layout:\n\t\tInternal flash addr: 0x%x size: %d\n" , (uint8_t*)&__FLASH_start__ ,  (uint8_t*)&__FLASH_end__ - (uint8_t*)&__FLASH_end__ ) ;


	  msg_console("\t\tflash section layout:\n");
	  extern uint32_t  __vec_start__ ;
	  extern uint32_t  __vec_end__ ;
	  msg_console("%sflash_vec_table\taddr: 0x%x size: %d\n" , "\t\t\t." , (uint8_t*)&__vec_start__ ,  (uint8_t*)&__vec_end__ - (uint8_t*)&__vec_start__ ) ;


	  extern uint32_t __code_start__ ;
	  extern uint32_t __code_end__ ;
	  msg_console("%scode\t\t\taddr: 0x%x size: %d\n" , "\t\t\t." , (uint8_t*)&__code_start__ ,  (uint8_t*)&__code_end__ - (uint8_t*)&__code_start__ ) ;

              extern uint32_t __preinit_array_start__ ;
              extern uint32_t __preinit_array_end__ ;
              msg_console("%spreinit_array\t\taddr: 0x%x size: %d\n" , "\t\t\t\t." , (uint8_t*)&__preinit_array_start__ ,  (uint8_t*)&__preinit_array_end__ - (uint8_t*)&__preinit_array_start__ ) ;

              extern uint32_t __init_array_start__ ;
              extern uint32_t __init_array_end__ ;
              msg_console("%sinit_array\t\taddr: 0x%x size: %d\n" , "\t\t\t\t." , (uint8_t*)&__init_array_start__ ,  (uint8_t*)&__init_array_end__ - (uint8_t*)&__init_array_start__ ) ;

              extern uint32_t __fini_array_start__ ;
              extern uint32_t __fini_array_end__ ;
              msg_console("%sfini_array\t\taddr: 0x%x size: %d\n" , "\t\t\t\t." , (uint8_t*)&__fini_array_start__ ,  (uint8_t*)&__fini_array_end__ - (uint8_t*)&__fini_array_start__ ) ;

              extern uint32_t __privileged_functions_start__ ;
              extern uint32_t __privileged_functions_end__ ;
              msg_console("%sprivileged_functions\taddr: 0x%x size: %d\n" , "\t\t\t\t." , (uint8_t*)&__privileged_functions_start__ ,  (uint8_t*)&__privileged_functions_end__ - (uint8_t*)&__privileged_functions_start__ ) ;

              extern uint32_t __text_start__ ;
              extern uint32_t __text_end__ ;
              msg_console("%stext\t\t\taddr: 0x%x size: %d\n" , "\t\t\t\t." , (uint8_t*)&__text_start__ ,  (uint8_t*)&__text_end__ - (uint8_t*)&__text_start__ ) ;


              extern uint32_t __rodata_start__ ;
              extern uint32_t __rodata_end__ ;
              msg_console("%srodata\t\t\taddr: 0x%x size: %d\n" , "\t\t\t\t." , (uint8_t*)&__rodata_start__ ,  (uint8_t*)&__rodata_end__ - (uint8_t*)&__rodata_start__ ) ;

              extern uint32_t __const_data_start__ ;
              extern uint32_t __const_data_end__ ;
              msg_console("\%sconst_data\t\taddr: 0x%x size: %d\n" , "\t\t\t\t." , (uint8_t*)&__const_data_start__ ,  (uint8_t*)&__const_data_end__ - (uint8_t*)&__const_data_start__ ) ;

              extern uint32_t __ccm_data_load_start__ ;
              extern uint32_t __ccm_data_load_end__ ;
              msg_console("%sccm_data LMA\t\taddr: 0x%x size: %d\n" , "\t\t\t." , (uint8_t*)&__ccm_data_load_start__ ,  (uint8_t*)&__ccm_data_load_end__ - (uint8_t*)&__ccm_data_load_start__ ) ;

              extern uint32_t __data_load_start__ ;
              extern uint32_t __data_load_end__ ;
              msg_console("%sdata LMA\t\taddr: 0x%x size: %d\n" , "\t\t\t." , (uint8_t*)&__data_load_start__ ,  (uint8_t*)&__data_load_end__ - (uint8_t*)&__data_load_start__ ) ;


              extern uint32_t __eeprom_start__ ;
              extern uint32_t __eeprom_stop__ ;
              msg_console("%seeprom\t\t\taddr: 0x%x size: %d\n" , "\t\t\t." , (uint8_t*)&__eeprom_start__ ,  (uint8_t*)&__eeprom_stop__ - (uint8_t*)&__eeprom_start__ ) ;


              //extern unsigned long  __image_start__ ;
              //extern unsigned long  __image_end__ ;
              //msg_console("\n\timage addr: 0x%x size: %d\n" , &__image_start__ ,  &__image_end__ - &__image_start__ ) ;


              extern uint32_t  __SRAM_start__;
              extern uint32_t  __SRAM_end__ ;
              msg_console("\t\tInternal ram addr: 0x%x size: %d\n" , (uint8_t*)&__SRAM_start__ ,  (uint8_t*)&__SRAM_end__ - (uint8_t*)&__SRAM_start__ ) ;


              msg_console("\t\tram section layout:\n");
              extern uint32_t  __ccm_bss_start__ ;
              extern uint32_t  __ccm_bss_end__ ;
              msg_console("\tCCM RAM .bss addr: 0x%x size: %d\n" , (uint8_t*)&__ccm_bss_start__ , (uint8_t*)&__ccm_bss_end__ - (uint8_t*)&__ccm_bss_start__ ) ;
              extern uint32_t  __ccm_data_start__ ;
              extern uint32_t  __ccm_data_end__ ;
              msg_console("\tCCM RAM .data addr: 0x%x size: %d\n" , (uint8_t*)&__ccm_data_start__ , (uint8_t*)&__ccm_data_end__ - (uint8_t*)&__ccm_data_start__ ) ;

              extern uint32_t  __bss_start__ ;
              extern uint32_t  __bss_end__ ;
              msg_console("\tRAM bss  .addr: 0x%x size: %d\n" , (uint8_t*)&__bss_start__, (uint8_t*)&__bss_end__ - (uint8_t*)&__bss_start__) ;

              extern uint32_t  __data_start__ ;
              extern uint32_t  __data_end__ ;
              msg_console("\tRAM data .addr: 0x%x size: %d\n" , (uint8_t*)&__data_start__, (uint8_t*)&__data_end__ - (uint8_t*)&__data_start__) ;

}
//------------------------------------------------------------------------------
static void time_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
        msg_console("sec %f\n" , (float)GetRunTimeStatsClock() / 20000.0f ) ;
}
//------------------------------------------------------------------------------
static void sys_state_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
TaskStatus_t *pxTaskStatusArray;
volatile UBaseType_t uxArraySize = 0 ;
volatile UBaseType_t  x = 0 ;
unsigned long ulTotalRunTime, ulTotalRunTimeExeptIrq,  ulStatsAsPercentage;

/*
         uint32_t tmp , days , hours, minutes, secs ;

         TickType_t rt_tic_counter = portGET_RUN_TIME_COUNTER_VALUE() ;

         TickType_t uSecs = rt_counter_us(rt_tic_counter);
         tmp = uSecs/1000000 ;

         secs = tmp % 60 ;
         minutes  = tmp / 60 ;

         tmp = minutes ;

         hours = tmp % 24 ;
         days = tmp / 24 ;
         msg_console ("\trt tics\t: %U ( %u day %u hour %u min %u sec   )\n" , uSecs , days, hours,minutes,secs );
*/


const static char* task_state_str[] = { "Running" , "Ready" , "Blocked", "Suspended", "Deleted" } ;

    /* Take a snapshot of the number of tasks in case it changes while this
    function is executing. */
    uxArraySize = uxTaskGetNumberOfTasks();


    /* Allocate a TaskStatus_t structure for each task.  An array could be
    allocated statically at compile time. */
    pxTaskStatusArray = (TaskStatus_t*)malloc( uxArraySize * sizeof( TaskStatus_t ) );

    if( pxTaskStatusArray != NULL )
    {
        /* Generate raw status information about each task. */
        uxArraySize = uxTaskGetSystemState( pxTaskStatusArray, uxArraySize, &ulTotalRunTime );
        ulTotalRunTimeExeptIrq = ulTotalRunTime ;

        /* For percentage calculations. */
        ulTotalRunTime /= 100UL;

        /* Avoid divide by zero errors. */
        if( ulTotalRunTime > 0 )
        {

            msg_console( "%s\t%s\t\t%s\t\t%s\t\t%s\t%s\t%s\t\%s\t%%\n",
                                    "#",
                                    "Name",
                                    "Handle",
                                    "State",
                                    "BPrio",
                                    "CPrio",
                                    "Stack",
                                    "Run time"
                                     );
            msg_console("----------------------------------------------------------------------------------------------------\n");

            /* For each populated position in the pxTaskStatusArray array,
            format the raw data as human readable ASCII data. */
            for( x = 0; x < uxArraySize; x++ )
            {
                /* What percentage of the total run time has the task used?
                This will always be rounded down to the nearest integer.
                ulTotalRunTimeDiv100 has already been divided by 100. */
                ulStatsAsPercentage = pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;

                // eval a exepteion % irq time
                ulTotalRunTimeExeptIrq -= pxTaskStatusArray[ x ].ulRunTimeCounter ;

                if( ulStatsAsPercentage > 0UL )
                {
                    msg_console( "%u\t%s\t\t0x%x\t%s\t\t%u\t%u\t%u\t\%u\t\t%u%%\r\n",
                                            pxTaskStatusArray[ x ].xTaskNumber,
                                            pxTaskStatusArray[ x ].pcTaskName,
                                            pxTaskStatusArray[ x ].xHandle,
                                            task_state_str[pxTaskStatusArray[ x ].eCurrentState],
                                            pxTaskStatusArray[ x ].uxBasePriority,
                                            pxTaskStatusArray[ x ].uxCurrentPriority,
                                            pxTaskStatusArray[ x ].usStackHighWaterMark,
                                            pxTaskStatusArray[ x ].ulRunTimeCounter,
                                            ulStatsAsPercentage );
                }
                else
                {
                    /* If the percentage is zero here then the task has
                    consumed less than 1% of the total run time. */
                    msg_console( "%u\t%s\t\t0x%x\t%s\t\t%u\t%u\t%u\t\%u\t\t<1%%\r\n",
                                            pxTaskStatusArray[ x ].xTaskNumber,
                                            pxTaskStatusArray[ x ].pcTaskName,
                                            pxTaskStatusArray[ x ].xHandle,
                                            task_state_str[pxTaskStatusArray[ x ].eCurrentState],
                                            pxTaskStatusArray[ x ].uxBasePriority,
                                            pxTaskStatusArray[ x ].uxCurrentPriority,
                                            pxTaskStatusArray[ x ].usStackHighWaterMark,
                                            pxTaskStatusArray[ x ].ulRunTimeCounter );
                }
            }

            ulStatsAsPercentage = ulTotalRunTimeExeptIrq / ulTotalRunTime;
            if ( ulStatsAsPercentage > 1UL )
              msg_console( "%s\t%s\t%s\t\t%s\t\t%s\t%s\t%s\t%u\t\t%u%%\n",
                                                "*",
                                                "Exept/IRQ",
                                                "*",
                                                "*",
                                                "*",
                                                "*",
                                                "*",
                                                ulTotalRunTimeExeptIrq,
                                                ulStatsAsPercentage
                                                 );
            else
              msg_console( "%s\t%s\t%s\t\t%s\t\t%s\t%s\t%s\t%u\t\t<1%%\n",
                                                "*",
                                                "Exept/IRQ",
                                                "*",
                                                "*",
                                                "*",
                                                "*",
                                                "*",
                                                ulTotalRunTimeExeptIrq
                                                 );

        }

        /* The array is no longer needed, free the memory it consumes. */
        free( pxTaskStatusArray );
    }
}
//------------------------------------------------------------------------------
static void unique_id_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
    msg_console("\tdev_id 0x%x, rev_id 0x%x, flash size %d kB, ram size %d kB\n\tunique_id: 0x%x 0x%x 0x%x\n" , DBGMCU_GetDEVID() , DBGMCU_GetREVID(), CHIP_FLASH_SIZE, CHIP_RAM_SIZE ,UNIQUE_ID_0 , UNIQUE_ID_1, UNIQUE_ID_2) ;
}
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
static void standby_console_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
    TScheduler::EndScheduler();
    PWR_EnterSTANDBYMode();
}
//------------------------------------------------------------------------------
static void flash_erase_command_handler ( TReadLine* rl , TReadLine::const_symbol_type_ptr_t* args , const size_t count )
{
	msg_console("FLASH will be erased and all non volatile programm and data losted, device will be damaged! continue[N/y]?\n");
	  int c ;
	  rl->RecvChar(c);
	  if  (c != 'y')
	    return ;

	  msg_console("\tmass erase performed...\n") ;

      FLASH_Unlock();
      FLASH_EraseAllSectors(VoltageRange_3);
      FLASH_Lock();
      NVIC_SystemReset();
}
//------------------------------------------------------------------------------
void SetConsoleCmdDefaultHandlers(TReadLine* rl)
{
	rl->AddCommandHandler("help" , help_command_handler);
	rl->AddCommandHandler("reset" , reset_command_handler);
	rl->AddCommandHandler("test_console" , test_console_command_handler);
	rl->AddCommandHandler("ver" , version_command_handler);
	rl->AddCommandHandler("dev_inf" , dev_inf_command_handler);
	rl->AddCommandHandler("time" , time_command_handler);
	rl->AddCommandHandler("sys_state", sys_state_command_handler);
	rl->AddCommandHandler("id", unique_id_command_handler);
	rl->AddCommandHandler("standby" , standby_console_command_handler);
	rl->AddCommandHandler("flash_erase" , flash_erase_command_handler);
}

//------------------------------------------------------------------------------
