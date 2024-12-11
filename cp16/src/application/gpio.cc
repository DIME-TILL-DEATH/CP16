#include "appdefs.h"
#include "gpio.h"
#include "eeprom.h"

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

	exti.EXTI_Mode = EXTI_Mode_Interrupt;
	exti.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	exti.EXTI_LineCmd = ENABLE;

	exti.EXTI_Line = EXTI_Line0;
	EXTI_Init(&exti);
	EXTI_ClearFlag(EXTI_Line0);

	exti.EXTI_Line = EXTI_Line1;
	EXTI_Init(&exti);
	EXTI_ClearFlag(EXTI_Line1);

	exti.EXTI_Line = EXTI_Line10;
	EXTI_Init(&exti);
	EXTI_ClearFlag(EXTI_Line10);

	exti.EXTI_Line = EXTI_Line11;
	EXTI_Init(&exti);
	EXTI_ClearFlag(EXTI_Line11);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 15;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
	NVIC_Init(&NVIC_InitStructure);

	NVIC_DisableIRQ(EXTI0_IRQn);
	NVIC_DisableIRQ(EXTI1_IRQn);
	NVIC_DisableIRQ(EXTI15_10_IRQn);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource11);
}

void pinInterrupt()
{
	static volatile bool fst = true ;
	if ( fst ) { fst = false; return; }

	uint8_t bank = (~((GPIOB->IDR & 0xc00) >> 10)) & 0x3;
	uint8_t preset = (~(GPIOB->IDR & 0x3)) & 0x3;


	BaseType_t  HigherPriorityTaskWoken;

	char cmd[] = "pc 00\r\n";
	cmd[3] = bank + '0'; // make hex. Works for digits 0-9
	cmd[4] = preset + '0';

	for (size_t i = 0; i<7; i++)
		ConsoleTask->WriteToInputBuffFromISR(cmd + i ,&HigherPriorityTaskWoken);

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}

extern "C" void EXTI0_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line0);

	pinInterrupt();
}

extern "C" void EXTI1_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line1);

	pinInterrupt();
}

extern "C" void EXTI15_10_IRQHandler()
{
	EXTI_ClearFlag(EXTI_Line10);
	EXTI_ClearFlag(EXTI_Line11);

	pinInterrupt();
}

inline unsigned long GetCpuClock(void)
{
	RCC_ClocksTypeDef rrc ;
	RCC_GetClocksFreq ( &rrc) ;
	return rrc.SYSCLK_Frequency ;
}
//---------------------------------------------------------------------------
