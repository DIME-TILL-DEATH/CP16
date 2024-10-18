#define __KLIBC_WRAPS_IMPL__

#include "appdefs.h"
#include "mmgr.h"
#include "usb.h"
#include "storage.h"
#include "AT45DB321.h"
#include "cs.h"
#include "bt.h"
#include "errno.h"
#include "ADAU/adau1701.h"
#include "usb_bsp.h"
#include "debug_led.h"
#include "ff.h"
#include "gpio.h"

volatile uint8_t usb_type = 0;
volatile uint32_t sysclock ;
extern "C" unsigned long GetCpuClock(void)
{
    RCC_ClocksTypeDef rrc ;
    RCC_GetClocksFreq ( &rrc) ;
    return rrc.SYSCLK_Frequency ;
}

extern "C" void _init(void)
{
	heap_init();
	sysclock = GetCpuClock() ;
	*(__errno()) = 0 ;

	adau_init_reset_pin();
	sig_reset(true);
	DELAY_NOP(5000);
	sig_reset(false);
}

extern "C" void _fini(void)
{
}

extern "C" void vApplicationStackOverflowHook( TaskHandle_t *pxTask, char *pcTaskName )
{
	//volatile char * task_name = (volatile char*) pcTaskName ;
	//err ( "task stack overflow: %s 0x%x\n" , pcTaskName , pxTask ) ;
	while(1)
		NOP();
}

extern "C" void vApplicationTickHook()
{
	NOP();
}

volatile uint32_t stack ;
extern "C" void vApplicationIdleHook()
{
	NOP();
}

void pin_usb_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE);
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

//volatile uint64_t zw = 78 ;
int main(void)
{
	pin_usb_init();
	if(!(GPIOA->IDR & GPIO_Pin_9))
		usb_type = 0;
	else
		usb_type = 1;

	DebugLedConfig();
	AT45DB321_Init();

	CSTask =  new TCSTask() ;
	CSTask->Create("CS", 20*configMINIMAL_STACK_SIZE , 0);

	//sudo rfcomm connect rfcomm1 98:D3:33:80:D1:1E
	ConsoleTask = new TConsoleTask(256) ;
	ConsoleTask->Create("CON", 30*configMINIMAL_STACK_SIZE, 1) ;
	ConsoleTask->SetIo(&bt_io);
	ConsoleTask->SetNoSyncMode();

	TScheduler::StartScheduler();
}
//---------------------------------------------------------------------------
void start_usb(uint8_t type)
{
	if(!type)
	{
		ConsoleTask->SetIo(&cdc_io);
		UsbTask =  new TUsbTask(TUsbTask::mCDC);
	}
	else
		UsbTask =  new TUsbTask(TUsbTask::mMSC);

	UsbTask->Create("USB", 10*configMINIMAL_STACK_SIZE, 0);
}

// пеерходный код к свежему sdk
extern void (*__preinit_array_start__ []) (void) ;
extern void (*__preinit_array_end__ []) (void) ;
extern void (*__init_array_start__ []) (void) ;
extern void (*__init_array_end__ []) (void) ;

extern void (*__fini_array_start__ []) (void);
extern void (*__fini_array_end__ []) (void);

extern void _init (void);
extern void _fini (void);
extern void _premain (void);

// Iterate over all the init routines.
extern "C" void __libc_init_array (void)
{
	size_t count;
	size_t i;

	count = __preinit_array_end__ - __preinit_array_start__ ;
	for (i = 0; i < count; i++)
		if ( __preinit_array_start__[i] ) __preinit_array_start__[i] ();

	_init ();

	count = __init_array_end__ - __init_array_start__;
	for (i = 0; i < count; i++)
		if (__init_array_start__[i]) __init_array_start__[i] ();
}

/* Run all the cleanup routines.  */
extern "C" void __libc_fini_array (void)
{
	size_t count;
	size_t i;

	count = __fini_array_end__ - __fini_array_start__;
	for (i = count; i > 0; i--)
		__fini_array_start__[i-1] ();

	_fini ();
}
