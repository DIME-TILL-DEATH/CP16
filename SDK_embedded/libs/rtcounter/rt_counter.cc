#include "FreeRTOSConfig.h"
#include "rt_counter.h"

#if ( configGENERATE_RUN_TIME_STATS == 1 )

/* The highest available interrupt priority. */
#define timerHIGHEST_PRIORITY			( 0 )



/*-----------------------------------------------------------*/

/* Variable that counts at 20KHz to provide the time base for the run time
stats. */
TickType_t ulRunTimeStatsClock = 0UL;
float  rt_clock_period_uS ;
size_t rt_self_delay;

/*-----------------------------------------------------------*/
extern "C" void vSetupHighFrequencyTimer( void )
{
unsigned long ulFrequency;
TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
NVIC_InitTypeDef NVIC_InitStructure;


	// Enable timer clocks
	RCC_APB1PeriphClockCmd( RT_COUNTER_RCC_APB1Periph_TIMirq , ENABLE );
	RCC_APB1PeriphClockCmd( RT_COUNTER_RCC_APB1Periph_TIMfree , ENABLE );

	// Initialize data.
	TIM_DeInit( RT_COUNTER_TIMirq );
	TIM_DeInit( RT_COUNTER_TIMfree );
	TIM_TimeBaseStructInit( &TIM_TimeBaseStructure );

	ulFrequency = RT_COUNTER_TIMirq_CLOCK / (timerINTERRUPT_FREQUENCY) ;
	TIM_TimeBaseStructure.TIM_Period = ( unsigned short ) ( ulFrequency & 0xffffUL ) - 1;
	TIM_TimeBaseStructure.TIM_Prescaler = 0x0;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit( RT_COUNTER_TIMirq, &TIM_TimeBaseStructure );
	TIM_ARRPreloadConfig( RT_COUNTER_TIMirq, ENABLE );

	// Configuration for timer 'free' which is used as a high resolution time measurement.
	TIM_TimeBaseStructure.TIM_Period = 0xffffffff;
	TIM_TimeBaseInit( RT_COUNTER_TIMfree, &TIM_TimeBaseStructure );
	TIM_ARRPreloadConfig( RT_COUNTER_TIMfree, ENABLE );

	// Enable TIMirq IT.  TIMfree does not generate an interrupt.
	NVIC_InitStructure.NVIC_IRQChannel = RT_COUNTER_TIMirq_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = timerHIGHEST_PRIORITY;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init( &NVIC_InitStructure );
	TIM_ITConfig( RT_COUNTER_TIMirq , TIM_IT_Update, ENABLE );

	// Finally, enable both timers
	TIM_Cmd( RT_COUNTER_TIMirq , ENABLE );
	TIM_Cmd( RT_COUNTER_TIMfree , ENABLE );

	// init for RTCounter funcs
	rt_clock_period_uS =  1000000.0f / ((float)RT_COUNTER_TIMfree_CLOCK) ;

	// colibrate self delay
	rt_counter_calibrate();

	//DBGMCU_APB1PeriphConfig( RT_COUNTER_TIM_DEBUG_STOP , ENABLE) ;

}
//-----------------------------------------------------------

extern "C" TickType_t GetRunTimeStatsClock()
{
  return ulRunTimeStatsClock ;
}

extern "C" void  RT_COUNTER_TIM_IRQ_HANDLER ( void )  ;
void RT_COUNTER_TIM_IRQ_HANDLER ( void )
{
        RT_COUNTER_TIMirq->SR = (uint16_t)~TIM_IT_Update;
	/* Keep a count of the number of interrupts as a time base for the run time
	stats collection. */
	ulRunTimeStatsClock++;
}

#endif
