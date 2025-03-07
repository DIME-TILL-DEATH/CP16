
#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

      #include "stdint.h"
      #include "portmacro.h"

      #ifdef __cplusplus
	extern "C" {
      #endif
	    unsigned long GetCpuClock(void) ;
	    TickType_t GetRunTimeStatsClock();
      #ifdef __cplusplus
           }
      #endif

        #define configUSE_64_BIT_TICKS          0
        #define configUSE_16_BIT_TICKS          0

	#define configUSE_PREEMPTION                    1
	#define configUSE_IDLE_HOOK                     1
	#define configUSE_TICK_HOOK                     1
	#define configCPU_CLOCK_HZ                      ( ( unsigned long ) GetCpuClock() )
	#define configTICK_RATE_HZ                      ( ( TickType_t ) TICK_RATE_HZ_DEFAULT )
	#define configMAX_PRIORITIES                    ( ( unsigned portBASE_TYPE ) MAX_PRIORITIES )
	#define configMINIMAL_STACK_SIZE                ( ( unsigned short ) 70 )
	#define configTOTAL_HEAP_SIZE                   ( ( size_t ) ( TOTAL_HEAP_SIZE ) )
	#define configMAX_TASK_NAME_LEN                 ( MAX_TASK_NAME_LEN )
	#define configUSE_TRACE_FACILITY                1
	#define configUSE_16_BIT_TICKS                  0
	#define configIDLE_SHOULD_YIELD                 1
	#define configUSE_MUTEXES                       1
	#define configQUEUE_REGISTRY_SIZE               5
	#define configCHECK_FOR_STACK_OVERFLOW          2
	#define configUSE_RECURSIVE_MUTEXES             0
	#define configUSE_MALLOC_FAILED_HOOK            1
	#define configUSE_APPLICATION_TASK_TAG          0
    #define configUSE_COUNTING_SEMAPHORES           1


	/* Co-routine definitions. */
	#define configUSE_CO_ROUTINES           0
	#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

    #define configUSE_TIMERS                1
    #define configTIMER_TASK_PRIORITY       2   // Priority lower than sound DMA and SPI (1, 0).
    #define configTIMER_QUEUE_LENGTH        32
    #define configTIMER_TASK_STACK_DEPTH    (configMINIMAL_STACK_SIZE*2)

	/* Set the following definitions to 1 to include the API function, or zero
	to exclude the API function. */

	#define INCLUDE_vTaskPrioritySet                1
	#define INCLUDE_uxTaskPriorityGet               1
	#define INCLUDE_vTaskDelete                     1
	#define INCLUDE_vTaskCleanUpResources           0
	#define INCLUDE_vTaskSuspend                    1
	#define INCLUDE_vTaskDelayUntil                 1
	#define INCLUDE_vTaskDelay                      1
    #define INCLUDE_uxTaskGetStackHighWaterMark     1
    #define INCLUDE_eTaskGetState                   1

	/* Use the system definition, if there is one */
	#ifdef __NVIC_PRIO_BITS
	        #define configPRIO_BITS       __NVIC_PRIO_BITS
	#else
	        #define configPRIO_BITS       4        /* 15 priority levels */
	#endif

	#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         15
	#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

	/* The lowest priority. */
	#define configKERNEL_INTERRUPT_PRIORITY         ( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
	/* Priority 5, or 95 as only the top four bits are implemented. */
	/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
	See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
	#define configMAX_SYSCALL_INTERRUPT_PRIORITY    ( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

	/* Prevent the following definitions being included when FreeRTOSConfig.h
	is included from an asm file. */



        #define configGENERATE_RUN_TIME_STATS 1

        #ifdef __cplusplus
                extern "C" {
        #endif
                        void vSetupHighFrequencyTimer( void );
        #ifdef __cplusplus
                }
        #endif


	#define timerINTERRUPT_FREQUENCY                ( ( unsigned short ) 20000 )
	#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() vSetupHighFrequencyTimer()
	#define portGET_RUN_TIME_COUNTER_VALUE() GetRunTimeStatsClock()
//	#define portGET_RUN_TIME_MS(RUN_TIME) 1000*RUN_TIME/(TickType_t)timerINTERRUPT_FREQUENCY
//	#define portGET_RUN_TIME_S(RUN_TIME) RUN_TIME/timerINTERRUPT_FREQUENCY
//	#define portGET_RUN_TIME_HZ(RUN_TIME) 1.0f*timerINTERRUPT_FREQUENCY/RUN_TIME


#endif /* FREERTOS_CONFIG_H */
