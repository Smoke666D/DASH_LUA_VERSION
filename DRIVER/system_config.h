/*
 * system_config.h
 *
 *  Created on: Nov 3, 2024
 *      Author: smoke
 */

#ifndef SYSTEM_CONFIG_H_
#define SYSTEM_CONFIG_H_


#define WCH32V3   2 
#define WCH32V2   1


#define CORE WCH32V3

#define INIT_FUNC_LOC //__attribute__((section(".stext"))) __attribute__((optimize(3)))

#if CORE == WCH32V2
    #include "ch32v20x_i2c.h"
    #include "ch32v20x_dma.h"
    #include "ch32v20x_adc.h"
    #include "ch32v20x_rcc.h"
    #include "ch32v20x_can.h"
    #include "ch32v20x_iwdg.h"
    #include "ch32v20x_spi.h"
    #include "ch32v20x_gpio.h"
    #include "ch32v20x_dac.h"
    #include "ch32v20x_usart.h"
    #include "ch32v20x.h"
#else
    #include "ch32v30x_i2c.h"
    #include "ch32v30x_adc.h"
    #include "ch32v30x_dma.h"
    #include "ch32v30x_rcc.h"
    #include "ch32v30x_can.h"
    #include "ch32v30x_iwdg.h"
    #include "ch32v30x_spi.h"
    #include "ch32v30x_gpio.h"
    #include "ch32v30x_dac.h"
    #include "ch32v30x_usart.h"
    #include "ch32v30x.h"
#endif



#include "hal_irq.h"


//#define DMA1_CH1_ENABLE 0
//#define DMA1_CH2_ENABLE 0
//#define DMA1_CH3_ENABLE 0
//#define DMA1_CH4_ENABLE 0
#define DMA1_CH5_ENABLE 1
//#define DMA1_CH6_ENABLE 0
//#define DMA1_CH7_ENABLE 0

/*妤把快把抑志忘扶我快 RTC*/
//#define RTC_IRQ_ENABLE

#define DMA1_CH5_PRIOR    0
#define DMA1_CH5_SUBPRIOR 2
#define TIMER3_PRIOR      1
#define TIMER3_SUBPRIOR   3
#define CAN1_PRIOR        1
#define CAN1_SUBPRIOR     0


//#define USART1_IT_ENABLE
//#define  USART2_IT_ENABLE
//#define  USART3_IT_ENABLE
//#define  USART4_IT_ENABLE

#define TIM3_UP_ENABLE

#define WDT_ENABLE

//#define ADC_1_IT_ENABLE
//#define ADC_2_IT_ENABLE

#endif /* SYSTEM_CONFIG_H_ */

