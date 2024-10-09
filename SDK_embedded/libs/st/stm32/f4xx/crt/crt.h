#ifndef __CRT_H__
#define __CRT_H__

#ifdef  __cplusplus
 extern "C" {
#endif

typedef void( *IrqHandlerFunc )( void );

typedef enum
{
                vtThreadMode=0,         /* Thread CPU mode, and the initial stack pointer offset in vec table*/
		vtResetHandler,             /* The reset handler*/
		vtNMIException,
		vtHardFaultException,
		vtMemManageException,
		vtBusFaultException,
		vtUsageFaultException,
		vtReserved0, vtReserved2, vtReserved3, vtReserved4,            /* Reserved */
	#if defined __USE_FREERTOS__
		vtvPortSVCHandler,
	#else
		vtSVCHandler,
	#endif
		vtDebugMonitor,
		vtReserved5,                      /* Reserved */
	#if defined __USE_FREERTOS__
		vtxPortPendSVHandler,
		vtxPortSysTickHandler,
	#else
		vtPendSVC,
		vtSysTickHandler,
	#endif
		vtWWDG_IRQHandler,
		vtPVD_IRQHandler,
		vtTAMPER_IRQHandler,
		vtRTC_IRQHandler,
		vtFLASH_IRQHandler,
		vtRCC_IRQHandler,
		vtEXTI0_IRQHandler,
		vtEXTI1_IRQHandler,
		vtEXTI2_IRQHandler,
		vtEXTI3_IRQHandler,
		vtEXTI4_IRQHandler,
		vtDMA1_Stream0_IRQHandler,
		vtDMA1_Stream1_IRQHandler,
		vtDMA1_Stream2_IRQHandler,
		vtDMA1_Stream3_IRQHandler,
		vtDMA1_Stream4_IRQHandler,
		vtDMA1_Stream5_IRQHandler,
		vtDMA1_Stream6_IRQHandler,
		vtADC_IRQHandler,     		/*!< ADC1, ADC2 and ADC3 global Interrupts                             */
		vtCAN1_TX_IRQHandler,     	/*!< CAN1 TX Interrupt                                                 */
		vtCAN1_RX0_IRQHandler,     	/*!< CAN1 RX0 Interrupt                                                */
		vtCAN1_RX1_IRQHandler,    	 /*!< CAN1 RX1 Interrupt                                                */
		vtCAN1_SCE_IRQHandler,    	 /*!< CAN1 SCE Interrupt                                                */
		vtEXTI9_5_IRQHandler,    	 /*!< External Line[9:5] Interrupts                                     */
		vtTIM1_BRK_TIM9_IRQHandler,     /*!< TIM1 Break interrupt and TIM9 global interrupt                    */
		vtTIM1_UP_TIM10_IRQHandler,     /*!< TIM1 Update Interrupt and TIM10 global interrupt                  */
		vtTIM1_TRG_COM_TIM11_IRQHandler,     /*!< TIM1 Trigger and Commutation Interrupt and TIM11 global interrupt */
		vtTIM1_CC_IRQHandler,     /*!< TIM1 Capture Compare Interrupt                                    */
		vtTIM2_IRQHandler,     /*!< TIM2 global Interrupt                                             */
		vtTIM3_IRQHandler,     /*!< TIM3 global Interrupt                                             */
		vtTIM4_IRQHandler,     /*!< TIM4 global Interrupt                                             */
		vtI2C1_EV_IRQHandler,     /*!< I2C1 Event Interrupt                                              */
		vtI2C1_ER_IRQHandler,     /*!< I2C1 Error Interrupt                                              */
		vtI2C2_EV_IRQHandler,     /*!< I2C2 Event Interrupt                                              */
		vtI2C2_ER_IRQHandler,     /*!< I2C2 Error Interrupt                                              */
		vtSPI1_IRQHandler,     /*!< SPI1 global Interrupt                                             */
		vtSPI2_IRQHandler,     /*!< SPI2 global Interrupt                                             */
		vtUSART1_IRQHandler,     /*!< USART1 global Interrupt                                           */
		vtUSART2_IRQHandler,     /*!< USART2 global Interrupt                                           */
		vtUSART3_IRQHandler,     /*!< USART3 global Interrupt                                           */
		vtEXTI15_10_IRQHandler,     /*!< External Line[15:10] Interrupts                                   */
		vtRTC_Alarm_IRQHandler,     /*!< RTC Alarm (A and B) through EXTI Line Interrupt                   */
		vtOTG_FS_WKUP_IRQHandler,     /*!< USB OTG FS Wakeup through EXTI line interrupt                     */
		vtTIM8_BRK_TIM12_IRQHandler,     /*!< TIM8 Break Interrupt and TIM12 global interrupt                   */
		vtTIM8_UP_TIM13_IRQHandler,     /*!< TIM8 Update Interrupt and TIM13 global interrupt                  */
		vtTIM8_TRG_COM_TIM14_IRQHandler,     /*!< TIM8 Trigger and Commutation Interrupt and TIM14 global interrupt */
		vtTIM8_CC_IRQHandler,     /*!< TIM8 Capture Compare Interrupt                                    */
		vtDMA1_Stream7_IRQHandler,     /*!< DMA1 Stream7 Interrupt                                            */
		vtFSMC_IRQHandler,     /*!< FSMC global Interrupt                                             */
		vtSDIO_IRQHandler,     /*!< SDIO global Interrupt                                             */
		vtTIM5_IRQHandler,     /*!< TIM5 global Interrupt                                             */
		vtSPI3_IRQHandler,     /*!< SPI3 global Interrupt                                             */
		vtUART4_IRQHandler,     /*!< UART4 global Interrupt                                            */
		vtUART5_IRQHandler,     /*!< UART5 global Interrupt                                            */
		vtTIM6_DAC_IRQHandler,     /*!< TIM6 global and DAC1&2 underrun error  interrupts                 */
		vtTIM7_IRQHandler,     /*!< TIM7 global interrupt                                             */
		vtDMA2_Stream0_IRQHandler,     /*!< DMA2 Stream 0 global Interrupt                                    */
		vtDMA2_Stream1_IRQHandler,     /*!< DMA2 Stream 1 global Interrupt                                    */
		vtDMA2_Stream2_IRQHandler,     /*!< DMA2 Stream 2 global Interrupt                                    */
		vtDMA2_Stream3_IRQHandler,     /*!< DMA2 Stream 3 global Interrupt                                    */
		vtDMA2_Stream4_IRQHandler,     /*!< DMA2 Stream 4 global Interrupt                                    */
		vtETH_IRQHandler,     /*!< Ethernet global Interrupt                                         */
		vtETH_WKUP_IRQHandler,     /*!< Ethernet Wakeup through EXTI line Interrupt                       */
		vtCAN2_TX_IRQHandler,     /*!< CAN2 TX Interrupt                                                 */
		vtCAN2_RX0_IRQHandler,     /*!< CAN2 RX0 Interrupt                                                */
		vtCAN2_RX1_IRQHandler,     /*!< CAN2 RX1 Interrupt                                                */
		vtCAN2_SCE_IRQHandler,     /*!< CAN2 SCE Interrupt                                                */
		vtOTG_FS_IRQHandler,     /*!< USB OTG FS global Interrupt                                       */
		vtDMA2_Stream5_IRQHandler,     /*!< DMA2 Stream 5 global interrupt                                    */
		vtDMA2_Stream6_IRQHandler,     /*!< DMA2 Stream 6 global interrupt                                    */
		vtDMA2_Stream7_IRQHandler,     /*!< DMA2 Stream 7 global interrupt                                    */
		vtUSART6_IRQHandler,     /*!< USART6 global interrupt                                           */
		vtI2C3_EV_IRQHandler,     /*!< I2C3 event interrupt                                              */
		vtI2C3_ER_IRQHandler,     /*!< I2C3 error interrupt                                              */
		vtOTG_HS_EP1_OUT_IRQHandler,     /*!< USB OTG HS End Point 1 Out global interrupt                       */
		vtOTG_HS_EP1_IN_IRQHandler,     /*!< USB OTG HS End Point 1 In global interrupt                        */
		vtOTG_HS_WKUP_IRQHandler,     /*!< USB OTG HS Wakeup through EXTI interrupt                          */
		vtOTG_HS_IRQHandler,     /*!< USB OTG HS global interrupt                                       */
		vtDCMI_IRQHandler,     /*!< DCMI global interrupt                                             */
		vtCRYP_IRQHandler,     /*!< CRYP crypto global interrupt                                      */
		vtHASH_RNG_IRQHandler,      /*!< Hash and Rng global interrupt                                     */
		vtFPU_IRQHandler,    /*!< FPU global interrupt   */

                #if defined (STM32F427X)
			vtUART7_IRQHandler,   /*!< UART7 global interrupt  */
			vtUART8_IRQHandler,   /*!< UART8 global interrupt  */
			vtSPI4_IRQHandler,    /*!< SPI4 global Interrupt   */
			vtSPI5_IRQHandler,    /*!< SPI5 global Interrupt   */
			vtSPI6_IRQHandler,    /*!< SPI6 global Interrupt   */
		#endif

		vtVecCount
} VectorType ;

#include <stdint.h>
#include "system_stm32f4xx.h" // defines of SystemInit

#include "crt_common.h"


#ifdef  __cplusplus
 }
#endif

#endif /*__CRT_H__*/
