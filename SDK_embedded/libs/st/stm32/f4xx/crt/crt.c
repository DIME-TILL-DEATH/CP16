#include "crt.h"
#include <stdint.h>

#ifdef __cplusplus
 extern "C" {
#endif

//------------------------------------------------------------------------------------
void __attribute__((weak)) ResetHandler(void);

void __attribute__((weak)) NMIException(void);
#pragma weak NMIException = DefaultExceptionHandler

void __attribute__((weak)) HardFaultException(void);

void __attribute__((weak)) MemManageException(void);
#pragma weak MemManageException = DefaultExceptionHandler

void __attribute__((weak)) BusFaultException(void);
#pragma weak BusFaultException = DefaultExceptionHandler

void __attribute__((weak)) UsageFaultException(void);
#pragma weak UsageFaultException = DefaultExceptionHandler

#if defined __USE_FREERTOS__
	void vPortSVCHandler(void);
#else
	void __attribute__((weak)) SVCHandler(void);
	#pragma weak SVCHandler = DefaultExceptionHandler

#endif
void __attribute__((weak)) DebugMonitor(void);
#pragma weak DebugMonitor = DefaultExceptionHandler

#if defined __USE_FREERTOS__
	void xPortPendSVHandler(void);
	void xPortSysTickHandler(void);
#else
	void __attribute__((weak)) PendSVC(void);
	#pragma weak PendSVC = DefaultExceptionHandler
	void __attribute__((weak)) SysTickHandler(void);
	#pragma weak SysTickHandler = DefaultExceptionHandler
#endif

	void __attribute__((weak)) WWDG_IRQHandler(void);
	#pragma weak WWDG_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) PVD_IRQHandler(void);
	#pragma weak PVD_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TAMPER_IRQHandler(void);
	#pragma weak TAMPER_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) RTC_IRQHandler(void);
	#pragma weak RTC_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) FLASH_IRQHandler(void);
	#pragma weak FLASH_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) RCC_IRQHandler(void);
	#pragma weak RCC_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) EXTI0_IRQHandler(void);
	#pragma weak EXTI0_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) EXTI1_IRQHandler(void);
	#pragma weak EXTI1_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) EXTI2_IRQHandler(void);
	#pragma weak EXTI2_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) EXTI3_IRQHandler(void);
	#pragma weak EXTI3_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) EXTI4_IRQHandler(void);
	#pragma weak EXTI4_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA1_Stream0_IRQHandler(void);
	#pragma weak DMA1_Stream0_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA1_Stream1_IRQHandler(void);
	#pragma weak DMA1_Stream1_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA1_Stream2_IRQHandler(void);
	#pragma weak DMA1_Stream2_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA1_Stream3_IRQHandler(void);
	#pragma weak DMA1_Stream3_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA1_Stream4_IRQHandler(void);
	#pragma weak DMA1_Stream4_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA1_Stream5_IRQHandler(void);
	#pragma weak DMA1_Stream5_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA1_Stream6_IRQHandler(void);
	#pragma weak DMA1_Stream6_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) ADC_IRQHandler(void);
	#pragma weak ADC_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CAN1_TX_IRQHandler(void);
	#pragma weak CAN1_TX_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CAN1_RX0_IRQHandler(void);
	#pragma weak CAN1_RX0_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CAN1_RX1_IRQHandler(void);
	#pragma weak CAN1_RX1_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CAN1_SCE_IRQHandler(void);
	#pragma weak CAN1_SCE_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) EXTI9_5_IRQHandler(void);
	#pragma weak EXTI9_5_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM1_BRK_TIM9_IRQHandler(void);
	#pragma weak TIM1_BRK_TIM9_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM1_UP_TIM10_IRQHandler(void);
	#pragma weak TIM1_UP_TIM10_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM1_TRG_COM_TIM11_IRQHandler(void);
	#pragma weak TIM1_TRG_COM_TIM11_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM1_CC_IRQHandler(void);
	#pragma weak TIM1_CC_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM2_IRQHandler(void);
	#pragma weak TIM2_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM3_IRQHandler(void);
	#pragma weak TIM3_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM4_IRQHandler(void);
	#pragma weak TIM4_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) I2C1_EV_IRQHandler(void);
	#pragma weak I2C1_EV_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) I2C1_ER_IRQHandler(void);
	#pragma weak I2C1_ER_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) I2C2_EV_IRQHandler(void);
	#pragma weak I2C2_EV_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) I2C2_ER_IRQHandler(void);
	#pragma weak I2C2_ER_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) SPI1_IRQHandler(void);
	#pragma weak SPI1_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) SPI2_IRQHandler(void);
	#pragma weak SPI2_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) USART1_IRQHandler(void);
	#pragma weak USART1_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) USART2_IRQHandler(void);
	#pragma weak USART2_IRQHandler = DefaultExceptionHandler


	void __attribute__((weak)) USART3_IRQHandler(void);
	#pragma weak USART3_IRQHandler = DefaultExceptionHandler


	void __attribute__((weak)) EXTI15_10_IRQHandler(void);
	#pragma weak EXTI15_10_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) RTC_Alarm_IRQHandler(void);
	#pragma weak RTC_Alarm_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) OTG_FS_WKUP_IRQHandler(void);
	#pragma weak OTG_FS_WKUP_IRQHandler = DefaultExceptionHandler


	void __attribute__((weak)) TIM8_BRK_TIM12_IRQHandler(void);
	#pragma weak TIM8_BRK_TIM12_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM8_UP_TIM13_IRQHandler(void);
	#pragma weak TIM8_UP_TIM13_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM8_TRG_COM_TIM14_IRQHandler(void);
	#pragma weak TIM8_TRG_COM_TIM14_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM8_CC_IRQHandler(void);
	#pragma weak TIM8_CC_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA1_Stream7_IRQHandler(void);
	#pragma weak DMA1_Stream7_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) FSMC_IRQHandler(void);
	#pragma weak FSMC_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) SDIO_IRQHandler(void);
	#pragma weak SDIO_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM5_IRQHandler(void);
	#pragma weak TIM5_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) SPI3_IRQHandler(void);
	#pragma weak SPI3_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) UART4_IRQHandler(void);
	#pragma weak UART4_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) UART5_IRQHandler(void);
	#pragma weak UART5_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM6_DAC_IRQHandler(void);
	#pragma weak TIM6_DAC_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) TIM7_IRQHandler(void);
	#pragma weak TIM7_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA2_Stream0_IRQHandler(void);
	#pragma weak DMA2_Stream0_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA2_Stream1_IRQHandler(void);
	#pragma weak DMA2_Stream1_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA2_Stream2_IRQHandler(void);
	#pragma weak DMA2_Stream2_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA2_Stream3_IRQHandler(void);
	#pragma weak DMA2_Stream3_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA2_Stream4_IRQHandler(void);
	#pragma weak DMA2_Stream4_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) ETH_IRQHandler(void);
	#pragma weak ETH_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) ETH_WKUP_IRQHandler(void);
	#pragma weak ETH_WKUP_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CAN2_TX_IRQHandler(void);
	#pragma weak CAN2_TX_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CAN2_RX0_IRQHandler(void);
	#pragma weak CAN2_RX0_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CAN2_RX1_IRQHandler(void);
	#pragma weak CAN2_RX1_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CAN2_SCE_IRQHandler(void);
	#pragma weak CAN2_SCE_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) OTG_FS_IRQHandler(void);
	#pragma weak OTG_FS_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA2_Stream5_IRQHandler(void);
	#pragma weak DMA2_Stream5_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA2_Stream6_IRQHandler(void);
	#pragma weak DMA2_Stream6_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DMA2_Stream7_IRQHandler(void);
	#pragma weak DMA2_Stream7_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) USART6_IRQHandler(void);
	#pragma weak USART6_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) I2C3_EV_IRQHandler(void);
	#pragma weak I2C3_EV_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) I2C3_ER_IRQHandler(void);
	#pragma weak I2C3_ER_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) OTG_HS_EP1_OUT_IRQHandler(void);
	#pragma weak OTG_HS_EP1_OUT_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) OTG_HS_EP1_IN_IRQHandler(void);
	#pragma weak OTG_HS_EP1_IN_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) OTG_HS_WKUP_IRQHandler(void);
	#pragma weak OTG_HS_WKUP_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) OTG_HS_IRQHandler(void);
	#pragma weak OTG_HS_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) DCMI_IRQHandler(void);
	#pragma weak DCMI_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) CRYP_IRQHandler(void);
	#pragma weak CRYP_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) HASH_RNG_IRQHandler(void);
	#pragma weak HASH_RNG_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) FPU_IRQHandler(void);
	#pragma weak FPU_IRQHandler = DefaultExceptionHandler

	void __attribute__((weak)) UART7_IRQHandler(void);
	#pragma weak UART7_IRQHandler = DefaultExceptionHandler

        void __attribute__((weak)) UART8_IRQHandler(void);
        #pragma weak UART8_IRQHandler = DefaultExceptionHandler

        void __attribute__((weak)) SPI4_IRQHandler(void);
        #pragma weak SPI4_IRQHandler = DefaultExceptionHandler

        void __attribute__((weak)) SPI5_IRQHandler(void);
        #pragma weak SPI5_IRQHandler = DefaultExceptionHandler

        void __attribute__((weak)) SPI6_IRQHandler(void);
        #pragma weak SPI6_IRQHandler = DefaultExceptionHandler

        void __attribute__((weak)) SAI1_IRQHandler(void);
        #pragma weak SAI1_IRQHandler = DefaultExceptionHandler

        void __attribute__((weak)) LTDC_IRQHandler(void);
        #pragma weak LTDC_IRQHandler = DefaultExceptionHandler

        void __attribute__((weak)) LTDC_ER_IRQHandler(void);
        #pragma weak LTDC_ER_IRQHandler = DefaultExceptionHandler

        void __attribute__((weak)) DMA2D_IRQHandler(void);
        #pragma weak DMA2D_IRQHandler = DefaultExceptionHandler


#ifdef __cplusplus
 }
#endif


/* init value for the stack pointer. defined in linker script */
extern unsigned long _stack_end_;

__attribute__ (( section(".flash_vec_table")))
IrqHandlerFunc vec_table[] =
{
  (IrqHandlerFunc)&_stack_end_,            /* The initial stack pointer*/
  ResetHandler,             /* The reset handler*/
  NMIException,
  HardFaultException,
  MemManageException,
  BusFaultException,
  UsageFaultException,
  0, 0, 0, 0,            /* Reserved */
#if defined __USE_FREERTOS__
  vPortSVCHandler,
#else
  SVCHandler,
#endif
  DebugMonitor,
  0,                      /* Reserved */
#if defined __USE_FREERTOS__
  xPortPendSVHandler,
  xPortSysTickHandler,
#else
  PendSVC,
  SysTickHandler,
#endif
	WWDG_IRQHandler,
	PVD_IRQHandler,
	TAMPER_IRQHandler,
	RTC_IRQHandler,
	FLASH_IRQHandler,
	RCC_IRQHandler,
	EXTI0_IRQHandler,
	EXTI1_IRQHandler,
	EXTI2_IRQHandler,
	EXTI3_IRQHandler,
	EXTI4_IRQHandler,
	DMA1_Stream0_IRQHandler,
	DMA1_Stream1_IRQHandler,
	DMA1_Stream2_IRQHandler,
	DMA1_Stream3_IRQHandler,
	DMA1_Stream4_IRQHandler,
	DMA1_Stream5_IRQHandler,
	DMA1_Stream6_IRQHandler,
	ADC_IRQHandler,
#ifndef STM32F401xx
	CAN1_TX_IRQHandler,
	CAN1_RX0_IRQHandler,
	CAN1_RX1_IRQHandler,
	CAN1_SCE_IRQHandler,
#else
	0,0,0,0,  // stm32f401 has no CAN1 module
#endif
	EXTI9_5_IRQHandler,
	TIM1_BRK_TIM9_IRQHandler,
	TIM1_UP_TIM10_IRQHandler,
	TIM1_TRG_COM_TIM11_IRQHandler,
	TIM1_CC_IRQHandler,
	TIM2_IRQHandler,
	TIM3_IRQHandler,
	TIM4_IRQHandler,
	I2C1_EV_IRQHandler,
	I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler,
	I2C2_ER_IRQHandler,
	SPI1_IRQHandler,
	SPI2_IRQHandler,
	USART1_IRQHandler,
	USART2_IRQHandler,
#ifndef STM32F401xx
	USART3_IRQHandler,
#else
	0,  // stm32f401 has no USART3 module
#endif
	EXTI15_10_IRQHandler,
	RTC_Alarm_IRQHandler,
	OTG_FS_WKUP_IRQHandler,
#ifndef STM32F401xx
	TIM8_BRK_TIM12_IRQHandler,
	TIM8_UP_TIM13_IRQHandler,
	TIM8_TRG_COM_TIM14_IRQHandler,
	TIM8_CC_IRQHandler,
#else
        0,0,0,0,  // stm32f401 has no TIM8 module
#endif
	DMA1_Stream7_IRQHandler,
#ifndef STM32F401xx
	FSMC_IRQHandler,
#else
        0,  // stm32f401 has no FSMC/FMS module
#endif
	SDIO_IRQHandler,
	TIM5_IRQHandler,
	SPI3_IRQHandler,
#ifndef STM32F401xx
	UART4_IRQHandler,
	UART5_IRQHandler,
	TIM6_DAC_IRQHandler,
	TIM7_IRQHandler,
#else
        0,0,0,0,  // stm32f401 has no UART4/UART5/TIM6/TIM7 module
#endif
	DMA2_Stream0_IRQHandler,
	DMA2_Stream1_IRQHandler,
	DMA2_Stream2_IRQHandler,
	DMA2_Stream3_IRQHandler,
	DMA2_Stream4_IRQHandler,
#ifndef STM32F401xx
	ETH_IRQHandler,
	ETH_WKUP_IRQHandler,
	CAN2_TX_IRQHandler,
	CAN2_RX0_IRQHandler,
	CAN2_RX1_IRQHandler,
	CAN2_SCE_IRQHandler,
#else
        0,0,0,0,0,0,  // stm32f401 has no ETH/CAN2 module
#endif
	OTG_FS_IRQHandler,
	DMA2_Stream5_IRQHandler,
	DMA2_Stream6_IRQHandler,
	DMA2_Stream7_IRQHandler,
	USART6_IRQHandler,
	I2C3_EV_IRQHandler,
	I2C3_ER_IRQHandler,
#ifndef STM32F401xx
	OTG_HS_EP1_OUT_IRQHandler,
	OTG_HS_EP1_IN_IRQHandler,
	OTG_HS_WKUP_IRQHandler,
	OTG_HS_IRQHandler,
	DCMI_IRQHandler,
#if defined(STM32F407xx) || defined(STM32F415xx) || defined(STM32F417xx) || defined (STM32F429_439xx)
	CRYP_IRQHandler,
#else
	0,
#endif
	HASH_RNG_IRQHandler,
#else
        0,0,0,0,0,0,0,  // stm32f401 has no OTG_HS/DCMI/CRYP/HASH_RNG module
#endif
	FPU_IRQHandler,

#if defined STM32F401xx
	0,0,
	SPI4_IRQHandler,
#endif

#if defined(STM32F427_437xx) || defined(STM32F429_439xx)
        UART7_IRQHandler,
        UART8_IRQHandler,
        SPI4_IRQHandler,
 	SPI5_IRQHandler,
	SPI6_IRQHandler,
	SAI1_IRQHandler,
  #if defined (STM32F429_439xx)
	LTDC_IRQHandler,
	LTDC_ER_IRQHandler,
  #else
	0,0, // STM32F427_437xx has no LTDC module
  #endif
	DMA2D_IRQHandler
#endif

};


#include "crt_common.c"






