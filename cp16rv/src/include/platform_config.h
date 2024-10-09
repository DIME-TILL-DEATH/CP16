// ���� ������������ ���������
// ��������� ����������� �����
// ����� ���������� ������� �� � ���������� ����� � ��.
// �������� ����������� ���������� ������

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PLATFORM_CONFIG_H__
#define __PLATFORM_CONFIG_H__

/* Includes ------------------------------------------------------------------*/

                #define RT_COUNTER_TIMirq  TIM7
                #define RT_COUNTER_TIMfree TIM2
                #define RT_COUNTER_TIM_IRQ_HANDLER TIM7_IRQHandler
                #define RT_COUNTER_RCC_APB1Periph_TIMirq RCC_APB1Periph_TIM7
                #define RT_COUNTER_RCC_APB1Periph_TIMfree RCC_APB1Periph_TIM2
                #define RT_COUNTER_TIMirq_IRQn TIM7_IRQn
                #define RT_COUNTER_TIM_DEBUG_STOP  DBGMCU_TIM7_STOP | DBGMCU_TIM7_STOP
                #define RT_COUNTER_TIMirq_CLOCK (0.5f*GetCpuClock())
                #define RT_COUNTER_TIMfree_CLOCK (0.5f*GetCpuClock())

#define adau_i2s_port        GPIOB
#define adau_i2s_port_rcc    RCC_AHB1Periph_GPIOB
#define adau_i2s_d_port      GPIOC
#define adau_i2s_d_port_rcc  RCC_AHB1Periph_GPIOC
#define adau_i2s_ws          GPIO_Pin_12
#define adau_i2s_ws_af       GPIO_PinSource12
#define adau_i2s_clk         GPIO_Pin_13
#define adau_i2s_clk_af      GPIO_PinSource13
#define adau_i2s_in          GPIO_Pin_2
#define adau_i2s_in_af       GPIO_PinSource2
#define adau_i2s_out         GPIO_Pin_3
#define adau_i2s_out_af      GPIO_PinSource3
#define adau_i2s_af          GPIO_AF_SPI2
#define adau_i2s_ext_af      GPIO_AF_SPI3

#define adau_i2s_mclk_port   GPIOC
#define adau_i2s_mclk_rcc    RCC_AHB1Periph_GPIOC
#define adau_i2s_mclk        GPIO_Pin_6
#define adau_i2s_mclk_af     GPIO_PinSource6

#define adau_i2s_spi         SPI2
#define adau_i2s_spi_ext     I2S2ext
#define adau_i2s_spi_rcc     RCC_APB1Periph_SPI2

#define flash_cs_port        GPIOA
#define flash_cs_rcc         RCC_AHB1Periph_GPIOA
#define flash_cs             GPIO_Pin_3

#define AT45DB321_CS0 GPIO_ResetBits(flash_cs_port,flash_cs);
#define AT45DB321_CS1 GPIO_SetBits(flash_cs_port,flash_cs);

#define DSP_RESET_PORT       GPIOA
#define DSP_RESET_PIN        GPIO_Pin_15
#define DSP_RESET_GPIO_MODE  GPIO_Mode_OUT
#define DSP_RESET_RCC_GPIO   RCC_AHB1Periph_GPIOA

//-------------------------------------------------------------------------------

#define adau_spi_port        GPIOC
#define adau_spi_port_rcc    RCC_AHB1Periph_GPIOC
#define adau_spi_clk_port    GPIOB
#define adau_spi_clk_port_rcc    RCC_AHB1Periph_GPIOB
#define adau_spi_cs_port    GPIOD
#define adau_spi_cs_port_rcc    RCC_AHB1Periph_GPIOD
#define adau_spi_clk         GPIO_Pin_3
#define adau_spi_clk_af      GPIO_PinSource3
#define adau_spi_out         GPIO_Pin_12
#define adau_spi_out_af      GPIO_PinSource12
#define adau_spi_cs          GPIO_Pin_2
#define adau_spi_af          GPIO_AF_SPI3

#define adau_com_spi         SPI3
#define adau_com_spi_rcc     RCC_APB1Periph_SPI3
#define adau_com_spi_rcc_c   RCC_APB1PeriphClockCmd

#define flash_spi            SPI1
#define flash_spi_rcc        RCC_APB2Periph_SPI1
#define flash_spi_rcc_c      RCC_APB2PeriphClockCmd

#define flash_port           GPIOA
#define flash_port_rcc       RCC_AHB1Periph_GPIOA
#define flash_port_clk       GPIO_Pin_5
#define flash_port_clk_af    GPIO_PinSource5
#define flash_port_in        GPIO_Pin_6
#define flash_port_in_af     GPIO_PinSource6
#define flash_port_out       GPIO_Pin_7
#define flash_port_out_af    GPIO_PinSource7
#define flash_af             GPIO_AF_SPI1

#endif /* __PLATFORM_CONFIG_H__ */

