#include "led_task.h"
#include <math.h>
#include "init.h"
#include "string.h"
#include "hal_dma.h"
#include "hal_gpio.h"
#include "hal_spi.h"//"../DRIVER/hal_spi.h"
#include "hal_timers.h"



/*                        0     1    2    3    4   5    6    7    8   9*/
const u8 DigitMask[] = {0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};
//                                       0  1  2  3 4  5  6  7     8  9   10   11  12 13   14
static const u16 Brigth[MAX_BRIGTH] =  { 0, 2, 3, 5,8,100,200,300,400,500,600,700,800,900,PWM_TIM_PERIOD+1};
static uint16_t SPI1_DATA[SPI1_CHIP_COUNT];
static uint16_t data[SPI1_CHIP_COUNT];
static uint16_t SPI2_DATA[SPI2_CHIP_COUNT];
static u16 counterRGB =0;


void SetBarState( u8 start_g, u8 count_g, u8 start_r, u8 count_r )
{
   u32 bar_led_buffer=0;
   u32 maskr= 0x01;
   u32 maskg =0x02;

   maskr = maskr<<(start_r*2);
   maskg = maskg<<(start_g*2);
   for (u8 i =0;i<count_g;i++)
   {
      bar_led_buffer|= maskg;
      maskg = maskg<<2;
   }
   SPI1_DATA[ 3] = (u16)(bar_led_buffer & 0xFFFF);
   SPI1_DATA[ 4] = (u16)(bar_led_buffer>>16 & 0xFFFF);
   bar_led_buffer = 0;
   for (u8 i =0;i< count_r;i++)
   {
        bar_led_buffer|= maskr;
        maskr = maskr<<2;
   }
   SPI1_DATA[ 3] |= (u16)(bar_led_buffer & 0xFFFF);
   SPI1_DATA[ 4] |= (u16)(bar_led_buffer>>16 & 0xFFFF);
   return;
}

static const u8 offset_mask[] = {2,2,1,1,0,0,2,2,1,1,0,0,2,0};
static const u8 step_mask[]   = {0,3,0,3,0,3,9,6,13,10,12,9,12,6};
static const u8 color_mask[]  = {0x01,0x02,0x04};
/*
 * Функция вывода в RGB светодиод. Из-за неравномернго распределения
 * светодидов по драйверу, нуобходимо вычилить номер драйвера и биты
 * в драйвере, отвечающие за нужный цвет выбранного светодиода
 */
void SetRGB(  u8 number, LED_COLOR_t color, LED_STATE_t state)
{
   u8 offset =offset_mask[number] ;
   u8 step= step_mask[number];
   u16 mask = color_mask[color - 1 ];
   if ( state == STATE_ON )
   {
         SPI1_DATA[offset]   |= mask << step;
   }
   else
   {
         SPI1_DATA[offset] &= ~(mask << step );
   }
}

/*
 *
 */
void SetSegDirect( u8 number, u8 mask)
{
    switch (number)
    {
           case 0:
               SPI2_DATA[0] &= 0xFFFC;
               SPI2_DATA[0] |= mask>>5;
               SPI2_DATA[1] &= 0x07FF;
               SPI2_DATA[1] |= (mask)<<11;
               break;
           case 1:
               SPI2_DATA[1] &= 0xFC03;
               SPI2_DATA[1] |= (mask << 3 );
               break;
           case 2:
               SPI2_DATA[1] &= 0xFFF8;
               SPI2_DATA[1] |= (mask >> 4);
               SPI2_DATA[2] &= 0x0FFF;
               SPI2_DATA[2] |= (mask <<12);
               break;
           case 3:
               SPI2_DATA[2] &= 0xF01F;
               SPI2_DATA[2] |= (mask << 5);
               break;
           case 4:
               SPI2_DATA[2] &= 0xFFE0;
               SPI2_DATA[2] |= ((mask>>2) & 0x1F);
               SPI2_DATA[3] &= 0x3FFF;
               SPI2_DATA[3] |= ((mask & 0x03) <<14);
               break;
           case 5:
               SPI2_DATA[3] &= 0xC07F;
               SPI2_DATA[3] |= ((u16)mask << 7 );
               break;
           case 6:
               SPI2_DATA[3] &= 0xFF80;
               SPI2_DATA[3] |=  mask ;
    }
}

void SetSegPoint( u8 on)
{
    if (on)
        SPI2_DATA[1] |= 0x0400;
    else
        SPI2_DATA[1] &= ~0x0400;
   return;
}








void SetSEG( u16 mask, u32 value, u8 point)
{
  int32_t val = value;
  u8 min = 0;
  u8 dc = 2,mask_count = 2;
  if (val <0)
  {
      min = 1;
      val=val*-1;
  }
  if (val/1000000)
  {
      dc = 7;
      mask_count = 0;
  }
  else
      if (val/100000)
      {
          mask_count = 1;
          dc = 6;
      }
       else
          if (val/10000)
              dc = 5;
          else
              if (val/1000)
                dc = 4;
              else
               if (val/100)
                   dc =3;

  for (u8 i = 0;i<dc;i++)
  {
     u8 digit = val % 10;

     SetSegDirect(i,DigitMask[digit]);
     val = val/10;
  }
  SetSegPoint(point);//Ставим точку
  if ( min )
  {
      SetSegDirect(dc,0x40);
      dc++;
  }
  for (u8 i=(dc);i<7;i++)
  {
      SetSegDirect(i,0);
  }
  switch(mask_count)
  {
      case 1:
          break;
          SPI2_DATA[3] &= 0xFF80;
          SPI2_DATA[3] |= ((mask>>8) & 0x7F);
          break;
      case 2:
          SPI2_DATA[3] &= 0xC000;
          SPI2_DATA[3] |=  ( (mask>>8 & 0x7F) | (mask & 0x7F)<<7);
          break;
      default:
          break;
  }

}


void SetBigSeg( u16 mask)
{
    SPI2_DATA[0] &= 0x0003;
    SPI2_DATA[0] |= (mask <<2);
}

/*
 *
 */
void vSetBrigth( BRIGTH_CHANNEL_t ch, u8 brigth)
{
    HAL_TIMER_SetPWMPulse(TIMER3, (ch == RGB_CHANNEL) ? TIM_CHANNEL_3 :  TIM_CHANNEL_4 , Brigth[brigth]);
    HAL_TIMER_EnablePWMCH(TIMER3);
    return;
}


void SPI1_DMA_Callback( void )
{
    HAL_DMA_Disable(DMA1_CH3);
    HAL_SPI_RXOveleyClear(HAL_SPI1 );
    while (HAL_SPI_GetBusy(HAL_SPI1) == HAL_SET);
    HAL_SetBit(  SPI1_Port , SPI1_NSS_Pin);
    return;
}

void SPI2_DMA_Callback( void )
{
    HAL_DMA_Disable(DMA1_CH5);
    HAL_SPI_RXOveleyClear( HAL_SPI2 );
    while (HAL_SPI_GetBusy( HAL_SPI2 ) == HAL_SET);
    HAL_SetBit(  SPI2_Port , SPI2_NSS_Pin);
    return;
}


/*
 *
 */
INIT_FUNC_LOC void vLedDriverStart(void)
{
    memset(SPI2_DATA,0,SPI2_CHIP_COUNT*2);
    memset(SPI1_DATA,0,SPI1_CHIP_COUNT*2);
    memset(data,0,SPI1_CHIP_COUNT*2);
	HAL_DMAInitIT(DMA1_CH5,MTOP, DMA_HWORD  ,(u32)&SPI2->DATAR, (u32)SPI2_DATA,SPI2_DMA_PRIOR,SPI2_DMA_SUBPRIOR ,&SPI2_DMA_Callback);
	HAL_DMAInitIT(DMA1_CH3,MTOP, DMA_HWORD  ,(u32)&SPI1->DATAR, (u32)data,     SPI1_DMA_PRIOR,SPI1_DMA_SUBPRIOR ,&SPI1_DMA_Callback);
	return;
}

/*
 *  Функция вывода данных в SPI, вызывается по прерыванию таймра №4
 */
//uint16_t data1 = 0x1;
void vLedProcess( void )
{
   HAL_ResetBit(  SPI2_Port , SPI2_NSS_Pin);
   HAL_DMA_SetCouterAndEnable(DMA1_CH5, SPI2_CHIP_COUNT);
   return;
}



void vRGBProcess()
{
     memcpy(data,SPI1_DATA,SPI1_CHIP_COUNT*2);
     if (++counterRGB >= BRIGTH_CONTER ) counterRGB = 0;
     if (counterRGB >= BRIGTH_R)
     {
          data[0]&=0x6DB6;
          data[1]&=0xDBF6;
          data[2]&=0x6DB6;
          data[3]&=0xAAAA;
          data[4]&=0xAAAA;
     }
     if (counterRGB >= BRIGTH_G)
     {
          data[0]&=0x5B6D;
          data[1]&=0xB7ED;
          data[2]&=0x5B6D;
          data[3]&=0x5555;
          data[4]&=0x5555;
      }
      if (counterRGB >=BRIGTH_B)
      {
          data[0]&=0x36DB;
          data[1]&=0x6FDB;
          data[2]&=0x56DB;
      }
    HAL_ResetBit(  SPI1_Port , SPI1_NSS_Pin);
    HAL_DMA_SetCouterAndEnable(DMA1_CH3, SPI1_CHIP_COUNT);
}
