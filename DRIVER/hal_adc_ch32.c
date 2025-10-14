/*
 * hal_adc.c
 *
 *  Created on: 11 閭攲瑜�. 2024 璋�.
 *      Author: i.dymov
 */

#include "hal_adc.h"
#include "hal_irq.h"


ADC_CF_t ADC_Callback[2];
ADC_t adcs;

static const  uint8_t ADC_chennel_ref[]={  ADC_Channel_0,  ADC_Channel_1,  ADC_Channel_2, ADC_Channel_3, ADC_Channel_4,  ADC_Channel_5,  ADC_Channel_6,  ADC_Channel_7,  ADC_Channel_8,  ADC_Channel_9,  ADC_Channel_10,
        ADC_Channel_11, ADC_Channel_12, ADC_Channel_13, ADC_Channel_14, ADC_Channel_15,  ADC_Channel_16,  ADC_Channel_17} ;




/* ADC SQx mask */
#define RSQR3_SQ_Set                     ((uint32_t)0x0000001F)
#define RSQR2_SQ_Set                     ((uint32_t)0x0000001F)
#define RSQR1_SQ_Set                     ((uint32_t)0x0000001F)
/* ADC SMPx mask */
#define SAMPTR1_SMP_Set                  ((uint32_t)0x00000007)
#define SAMPTR2_SMP_Set                  ((uint32_t)0x00000007)
/* CTLR1 register Mask */
#define CTLR1_CLEAR_Mask                 ((uint32_t)0xE0F0FEFF)
/* CTLR2 register Mask */
#define CTLR2_CLEAR_Mask                 ((uint32_t)0xFFF1F7FD)
/* RSQR1 register Mask */
#define RSQR1_CLEAR_Mask                 ((uint32_t)0xFF0FFFFF)
#define CFGR0_ADCPRE_Reset_Mask          ((uint32_t)0xFFFF3FFF)




static ADC_TypeDef* ADCS[]={ADC1,ADC2};


uint16_t HAL_ADC_DATA(ADC_NUMBER_t adc )            {  return (uint16_t)ADCS[adc]->RDATAR;  }
static inline void ADC_BUFFER_ENABLE ( ADC_NUMBER_t adc )   {  ADCS[adc]->CTLR1 |= (1 << 26);       }
static inline void ADC_BUFFER_DISABLE ( ADC_NUMBER_t adc )  {  ADCS[adc]->CTLR1 &= ~(1 << 26);      }
static inline void ADC_ENABLE( ADC_NUMBER_t adc )           {  ADCS[adc]->CTLR2 |= CTLR2_ADON_Set;  }
static inline void ADC_DISABLE (ADC_NUMBER_t adc  ) { ADCS[adc]->CTLR2 &= CTLR2_ADON_Reset;}
static inline void ADC_EXT_TRIG_ENABLE ( ADC_NUMBER_t adc )  {ADCS[adc]->CTLR2 |= CTLR2_EXTTRIG_Set;}
static inline void ADC_EXT_TRIG_DISABLE ( ADC_NUMBER_t adc )  {ADCS[adc]->CTLR2 &= CTLR2_EXTTRIG_Reset;}
static inline void ADC_DMA_ENABLE ( ADC_NUMBER_t adc ) {  ADCS[adc]->CTLR2 |= CTLR2_DMA_Set; }
static inline void ADC_DMA_DISABLE ( ADC_NUMBER_t adc ) {  ADCS[adc]->CTLR2  &= CTLR2_DMA_Reset; }
static inline void ADC_RESET_CAL( ADC_NUMBER_t adc  )  { ADCS[adc]->CTLR2 |= CTLR2_RSTCAL_Set; }
static inline void ADC_START_CAL ( ADC_NUMBER_t adc  )     {ADCS[adc]->CTLR2 |= CTLR2_CAL_Set; }


#if defined(ADC_1_IT_ENABLE) || defined(ADC_2_IT_ENABLE)

void ADC1_2_IRQHandler(void) __attribute__((interrupt()));


void ADC1_2_IRQHandler(void)
{

#ifdef ADC_1_IT_ENABLE
    if(ADC_GetITStatus(ADC1, ADC_IT_EOC))
    {
        ADC_Callback[0].CallBack();
    }
#endif
#ifdef ADC_2_IT_ENABLE
    if(ADC_GetITStatus(ADC2, ADC_IT_EOC))
    {
        ADC_Callback[1].CallBack();
    }
#endif
   // if(ADC_GetITStatus( ADC1, ADC_IT_AWD))
   // {
    //    ADC_ClearITPendingBit( ADC1, ADC_IT_AWD);
   //     adcs.awdt_callback();
  //  }
}
#endif



#if CORE == WCH32V3

static s16 CalibrattionVal[2] = {0,0};

u16 Get_ConversionVal( ADC_NUMBER_t adc , s16 val)
{
    if((val+CalibrattionVal[adc])<0|| val==0) return 0;
    if((CalibrattionVal[adc]+val)>4095||val==4095) return 4095;
    return (val+CalibrattionVal[adc]);
}
#endif
/*
 * Запускаем тактирование AЦП и сбрасвае его
 */
static void HAL_ADC_InitReinitRCC( ADC_NUMBER_t adc )
{
#ifdef ADC_1_ENABLE
    if ( adc == ADC_1)
    {
        RCC->APB2PCENR |= RCC_APB2Periph_ADC1;
        RCC->APB2PRSTR |= RCC_APB2Periph_ADC1;
        RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC1;
    }
#endif
#ifdef ADC_2_ENABLE
    if ( adc == ADC_2)
    {
        RCC->APB2PCENR |= RCC_APB2Periph_ADC2;
        RCC->APB2PRSTR |= RCC_APB2Periph_ADC2;
        RCC->APB2PRSTR &= ~RCC_APB2Periph_ADC2;
    }
    RCC_ADCCLKConfig(RCC_PCLK2_Div8);
#endif
}

#if CORE == WCH32V3
static void HAL_ADC_Calubration(ADC_NUMBER_t adc )
{
    ADC_ENABLE(adc);
    ADC_BUFFER_DISABLE(adc); //disable buffer
    ADC_RESET_CAL(adc);
    while(ADC_GetResetCalibrationStatus(ADCS[adc]));
    ADC_START_CAL(adc);
    while(ADC_GetCalibrationStatus(ADCS[adc]));
    CalibrattionVal[adc] = Get_CalibrationValue(ADCS[adc]);
}




void HAL_ADC_ContiniusScanTrigCinvertionDMA( ADC_NUMBER_t adc, uint8_t channel_count, uint8_t *  channel_nmber, u32 ext_trig)
{
    ADC_InitTypeDef  ADC_InitStructure = {0};
    HAL_ADC_InitReinitRCC(adc);
     //Получаем указаетель на флаги работы с АЦП

    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode =ENABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv =ext_trig ;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = channel_count;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;
    ADC_Init(ADCS[adc], &ADC_InitStructure);
    for (u8 i=0;i<channel_count;i++)
    {
          ADC_RegularChannelConfig(ADCS[adc],  ADC_chennel_ref[channel_nmber[ i  ]], i + 1,  ADC_SampleTime_13Cycles5);
     }
    if (ext_trig != ADC_ExternalTrigConv_None)
       {
            ADC_EXT_TRIG_ENABLE(adc);
       }
       else
       {
           ADC_EXT_TRIG_DISABLE (adc);
       }
      ADC_DMA_ENABLE (adc);
      HAL_ADC_Calubration(adc);

}
void HAL_ADC_InitIT( ADC_NUMBER_t adc, u32 ext_trig, u8 prior, u8 subprior, void (*f)(void) )
{
    HAL_ADC_InitReinitRCC(adc);
    ADC_Callback[adc].CallBack=f;
    ADC_InitTypeDef  ADC_InitStructure = {0};
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode =DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv =ext_trig ;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    ADC_InitStructure.ADC_OutputBuffer = ADC_OutputBuffer_Disable;
    ADC_InitStructure.ADC_Pga = ADC_Pga_1;
    if (ext_trig != ADC_ExternalTrigConv_None)
    {
        ADC_EXT_TRIG_ENABLE(adc);
    }
    else
    {
        ADC_EXT_TRIG_DISABLE (adc);
    }
    ADC_Init(ADCS[adc], &ADC_InitStructure);
    PFIC_IRQ_ENABLE_PG2( ADC1_2_IRQn ,prior,subprior);
    ADC_ITConfig(ADCS[adc], ADC_IT_EOC, ENABLE);
    HAL_ADC_Calubration(adc);
}

#endif



#if CORE == WCH32V2

static s16 Calibrattion_Val = 0;
u16 Get_ConversionVal(s16 val)
{
    if((val+Calibrattion_Val)<0|| val==0) return 0;
    if((Calibrattion_Val+val)>4095||val==4095) return 4095;
    return (val+Calibrattion_Val);
}

#endif

void HAL_ADC_RegularChannelConfig(ADC_TypeDef *ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
{
    uint32_t tmpreg1 = 0, tmpreg2 = 0;

#if ADC_CHANNEL_NUMBER_10_16 == 1
    if(ADC_Channel > ADC_Channel_9)
    {
        tmpreg1 = ADCx->SAMPTR1;
        tmpreg2 = SAMPTR1_SMP_Set << (3 * (ADC_Channel - 10));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * (ADC_Channel - 10));
        tmpreg1 |= tmpreg2;
        ADCx->SAMPTR1 = tmpreg1;
    }
    else
#endif
    {
        tmpreg1 = ADCx->SAMPTR2;
        tmpreg2 = SAMPTR2_SMP_Set << (3 * ADC_Channel);
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_SampleTime << (3 * ADC_Channel);
        tmpreg1 |= tmpreg2;
        ADCx->SAMPTR2 = tmpreg1;
    }
#if ADC_CHANNELRANK_0_6 == 1
    if(Rank < 7)
    {
        tmpreg1 = ADCx->RSQR3;
        tmpreg2 = RSQR3_SQ_Set << (5 * (Rank - 1));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 1));
        tmpreg1 |= tmpreg2;
        ADCx->RSQR3 = tmpreg1;
    }
#endif
#if ADC_CHANNELRANK_7_12 == 1
    else if(Rank < 13)
    {
        tmpreg1 = ADCx->RSQR2;
        tmpreg2 = RSQR2_SQ_Set << (5 * (Rank - 7));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 7));
        tmpreg1 |= tmpreg2;
        ADCx->RSQR2 = tmpreg1;
    }
#endif
#if ADC_CHANNELRANK_13_16 == 1
    else
    {
        tmpreg1 = ADCx->RSQR1;
        tmpreg2 = RSQR1_SQ_Set << (5 * (Rank - 13));
        tmpreg1 &= ~tmpreg2;
        tmpreg2 = (uint32_t)ADC_Channel << (5 * (Rank - 13));
        tmpreg1 |= tmpreg2;
        ADCx->RSQR1 = tmpreg1;
    }
#endif
}


#if CORE == WCH32V3
void HAL_ADC_ContiniusScanCinvertionDMA( ADC_NUMBER_t adc, uint8_t channel_count, uint8_t *  channel_nmber)
 {
#ifdef ADC_1_ENABLE
    if ( adc == ADC_1)
    {
        HAL_InitAPB2( RCC_APB2Periph_ADC1);
    }
#endif
#ifdef ADC_2_ENABLE
    else
    {
        HAL_InitAPB2( RCC_APB2Periph_ADC2);
    }
#endif
#if defined(ADC_1_ENABLE) || defined( ADC_2_ENABLE)
     uint32_t tmpreg = 0;
     tmpreg = RCC->CFGR0;
     tmpreg &= CFGR0_ADCPRE_Reset_Mask;
     tmpreg |= RCC_PCLK2_Div8;
     RCC->CFGR0 = tmpreg;
     ADC_TypeDef * ADCx = ADCS[adc];
     uint32_t tmpreg1 = 0;
     uint8_t  tmpreg2 = 0;
     tmpreg1 = ADCx->CTLR1;
     tmpreg1 &= CTLR1_CLEAR_Mask;
     tmpreg1 |= (uint32_t)(ADC_Mode_Independent | (uint32_t)ADC_OutputBuffer_Disable |(uint32_t)ADC_Pga_1 | ((uint32_t)ENABLE<< 8));
     ADCx->CTLR1 = tmpreg1;
     tmpreg1 = ADCx->CTLR2;
     tmpreg1 &= CTLR2_CLEAR_Mask;
     tmpreg1 |= (uint32_t)( ADC_DataAlign_Right | ADC_ExternalTrigConv_None | ((uint32_t)ENABLE<< 1));
     ADCx->CTLR2 = tmpreg1;
     tmpreg1 = ADCx->RSQR1;
     tmpreg1 &= RSQR1_CLEAR_Mask;
   //  tmpreg2 |= (uint8_t)(ADC_CHANNEL - (uint8_t)1);
     tmpreg1 |= (uint32_t)tmpreg2 << 20;
     ADCx->RSQR1 = tmpreg1;
     for (u8 i=0; i< (channel_count) ;i++)
     {
         HAL_ADC_RegularChannelConfig(ADCS[adc],ADC_chennel_ref[channel_nmber[ i  ]], i + 1,  ADC_SampleTime_239Cycles5 );
     }
     ADC_EXT_TRIG_ENABLE( adc);
     ADC_DMA_ENABLE (adc);
     ADC_ENABLE(adc);
     ADC_BUFFER_DISABLE(adc);
     ADC_RESET_CAL(adc);
     while(ADC_GetResetCalibrationStatus(ADCS[adc]));
     ADC_START_CAL(adc);
     while(ADC_GetCalibrationStatus(ADCS[adc]));
     CalibrattionVal[adc] = Get_CalibrationValue(ADCS[adc]);
     ADC_BUFFER_ENABLE(adc);
#endif
 }
#endif
#if CORE == WCH32V2
void HAL_ADC_ContiniusScanCinvertionDMA( ADC_NUMBER_t adc, uint8_t channel_count, uint8_t *  channel_nmber)
 {
#if ADC_1_ENABLE == 1
    if ( adc == ADC_1)
    {
        HAL_InitAPB2( RCC_APB2Periph_ADC1);
    }
#endif
#if ADC_2_ENABLE == 1
    else
    {
        HAL_InitAPB2( RCC_APB2Periph_ADC2);
    }
#endif
#if ((ADC_2_ENABLE == 1) ||  (ADC_1_ENABLE == 1))
     uint32_t tmpreg = 0;
     tmpreg = RCC->CFGR0;
     tmpreg &= CFGR0_ADCPRE_Reset_Mask;
     tmpreg |= RCC_PCLK2_Div8;
     RCC->CFGR0 = tmpreg;

     ADCS[adc]->CTLR1 = (uint32_t)(ADC_Mode_Independent | (uint32_t)ADC_OutputBuffer_Disable |(uint32_t)ADC_Pga_1 | ((uint32_t)ENABLE<< 8));
     ADCS[adc]->CTLR2 = (uint32_t)( ADC_DataAlign_Right | ADC_ExternalTrigConv_None | ((uint32_t)ENABLE<< 1));
     ADCS[adc]->RSQR1 = ((uint32_t)(ADC_CHANNEL-1)) << 20;;
     for (u8 i=0; i< (channel_count) ;i++)
     {
         HAL_ADC_RegularChannelConfig(ADCS[adc],ADC_chennel_ref[channel_nmber[ i  ]], i + 1,  ADC_SampleTime_239Cycles5 );
     }
     ADC_EXT_TRIG_ENABLE( adc);
     ADC_DMA_ENABLE (adc);
     ADC_ENABLE(adc);
     ADC_BUFFER_DISABLE(adc);
     ADC_RESET_CAL(adc);
     while(ADC_GetResetCalibrationStatus(ADCS[adc]));
     ADC_START_CAL(adc);
     while(ADC_GetCalibrationStatus(ADCS[adc]));
     Calibrattion_Val = Get_CalibrationValue(ADCS[adc]);
     ADC_BUFFER_ENABLE(adc);
#endif
 }


#endif

 void HAL_ADC_StartDMA( DMA_Stram_t chanel, uint16_t size)
 {
    HAL_DMA_SetCounter(chanel, size);
    ADC_DMA_ENABLE (ADC_1);
    ADCS[ADC_1]->STATR = ~(uint32_t)(ADC_IT_EOC >> 8);
    HAL_DMA_ITENABLE( chanel, DMA_IT_TC );
    HAL_DMA_Enable(chanel );
    ADC_ENABLE(ADC_1);
    ADCS[ADC_1]->CTLR2 |= CTLR2_EXTTRIG_SWSTART_Set;
}








