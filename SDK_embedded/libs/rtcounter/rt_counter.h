#ifndef __RT_COUNTER_H__
#define __RT_COUNTER_H__

#include "appdefs.h"

extern "C" TickType_t GetRunTimeStatsClock();

#if ( configGENERATE_RUN_TIME_STATS == 1 )


//-----------------------------------------------------------------------------------
// warning: not usable for mesure OS sync call as Delay,DelayUntil,etc... becouse
// this calls set a scheduller to iddle state, what deactivate a OS sync calls
inline void __attribute__((always_inline))  rt_counter_start(size_t& counter)
{
	extern size_t rt_self_delay;
	TScheduler::EnterCritical();
	counter = RT_COUNTER_TIMfree->CNT + rt_self_delay ;
}
//-----------------------------------------------------------------------------------
inline void __attribute__((always_inline))  rt_counter_start_woc(size_t& counter)
{
	extern size_t rt_self_delay;
	counter = RT_COUNTER_TIMfree->CNT + rt_self_delay ;
}
//-----------------------------------------------------------------------------------
// warning: not usable for mesure OS sync call as Delay,DelayUntil,etc... becouse
// this calls set a scheduller to iddle state, what deactivate a OS sync calls
inline void __attribute__((always_inline))  rt_counter_stop(size_t& counter)
{
    counter = RT_COUNTER_TIMfree->CNT - counter ;
    TScheduler::ExitCritical();
}
//-----------------------------------------------------------------------------------
inline size_t __attribute__((always_inline))  rt_counter_stop_woc(size_t& counter)
{
  counter = RT_COUNTER_TIMfree->CNT - counter ;
  return counter ;
}
//-----------------------------------------------------------------------------------
inline float __attribute__((always_inline))  rt_counter_stop_us_woc(size_t counter)
{
  extern float  rt_clock_period_uS ;
  float tmp = rt_counter_stop_woc(counter) * rt_clock_period_uS ;
  return tmp ;
}
//-----------------------------------------------------------------------------------
// warning: not usable for mesure OS sync call as Delay,DelayUntil,etc... becouse
// this calls set a scheduller to iddle state, what deactivate a OS sync calls
inline float __attribute__((always_inline))  rt_counter_stop_us(size_t counter)
{
  float tmp = rt_counter_stop_us_woc(counter);
  TScheduler::ExitCritical();
  return tmp ;
}
//-----------------------------------------------------------------------------------
inline float __attribute__((always_inline))  rt_counter_us(size_t counter)
{
  extern float  rt_clock_period_uS ;
  return counter * rt_clock_period_uS ;
}
//-----------------------------------------------------------------------------------
inline void __attribute__((always_inline))  rt_counter_calibrate()
{
  extern size_t rt_self_delay;
  rt_self_delay = 0 ;
  size_t tmp ;
  rt_counter_start(tmp);
  rt_counter_stop(tmp);
  rt_self_delay = tmp ;
}

#endif


#endif /*__RT_COUNTER_H__*/
