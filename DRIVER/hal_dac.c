/*
 * hal_dac.c
 *
 *  Created on: Aug 8, 2024
 *      Author: i.dymov
 */


#include "hal_dac.h"

#if  CORE == WCH32V3


uint32_t dac_ch[]={ DAC_Channel_1,DAC_Channel_2 };

void HAL_DAC_Init(HAL_DAC_InitTypeDef * init)
{
    HAL_InitAPB1(RCC_APB1Periph_DAC);
    uint32_t  tmpreg2 = 0;
    tmpreg2 = (init->DAC_Trigger | init->DAC_WaveGeneration |
                  init->DAC_LFSRUnmask_TriangleAmplitude | init->DAC_OutputBuffer);

    DAC->CTLR = tmpreg2 << dac_ch[init->channel];

    DAC->CTLR |= (DAC_EN1 << dac_ch[init->channel]);
    DAC_SetChannel1Data(DAC_Align_12b_R, 0);
}

#endif
