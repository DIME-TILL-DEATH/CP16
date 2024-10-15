#include "appdefs.h"
#include "init.h"
#include "eepr.h"

//-----------------------------------------------------------------
void init(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructure;

	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP ;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Low_Speed ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_10 | GPIO_Pin_11 ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
											 GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 |
								GPIO_Pin_15 ;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_4 | GPIO_Pin_8 | GPIO_Pin_10
								/*| GPIO_Pin_13 | GPIO_Pin_14*/;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// управление мапами пресетов gpiob pin11
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	EXTI_InitTypeDef exti ;
	EXTI_StructInit( &exti);
	exti.EXTI_Line = EXTI_Line11 ;
	exti.EXTI_Mode = EXTI_Mode_Interrupt ;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling ;
	exti.EXTI_LineCmd = ENABLE ;
	EXTI_Init(&exti);

	EXTI_ClearFlag(EXTI_Line11);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn ;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init( &NVIC_InitStructure );
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource11);
}

uint32_t sw1_state()
{
	return (GPIOB->IDR) & GPIO_Pin_11 ;
}

extern "C" void EXTI15_10_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line11);
	static volatile bool fst = true ;
	if ( fst ) { fst = false; return; }

	BaseType_t  HigherPriorityTaskWoken ;
	const char* cmd = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) ? "um0\r" : "um1\r" ;
	for ( size_t i = 0 ; i<4 ; i++ )
		ConsoleTask->WriteToInputBuffFromISR(  cmd + i ,&HigherPriorityTaskWoken) ;

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}

inline unsigned long GetCpuClock(void)
{
	RCC_ClocksTypeDef rrc ;
	RCC_GetClocksFreq ( &rrc) ;
	return rrc.SYSCLK_Frequency ;
}
//---------------------------------------------------------------------------
