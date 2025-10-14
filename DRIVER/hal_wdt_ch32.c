/*
 * hal_wdt_ch32.c
 *
 *  Created on: 24 июл. 2024 г.
 *      Author: i.dymov
 */

#include "hal_wdt.h"

INIT_FUNC_LOC void HAL_WDT_Init( uint32_t period_in_3_2ms)
{
#ifdef WDT_ENABLE
    IWDG->CTLR = IWDG_WriteAccess_Enable;
    while ((IWDG->STATR & IWDG_FLAG_PVU) !=0 ) ;
    IWDG->PSCR = IWDG_Prescaler_64;
    while ((IWDG->STATR & IWDG_FLAG_RVU) !=0 ) ;
    IWDG->RLDR = period_in_3_2ms;
    IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;
#endif

}

INIT_FUNC_LOC 
void HAL_WDTInit()
{
#ifdef WDT_ENABLE
    IWDG->CTLR = IWDG_WriteAccess_Enable;
    IWDG->PSCR = IWDG_Prescaler_64;
    IWDG->RLDR = 4000;
    IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;
#endif
}

void 
HAL_WDTReset()
{
#ifdef WDT_ENABLE
    IWDG->CTLR = CTLR_KEY_Reload;
#endif
}
/*
 *
 */
INIT_FUNC_LOC void
HAL_WDT_Init1s()
{
#ifdef WDT_ENABLE
    IWDG->CTLR = IWDG_WriteAccess_Enable;
    IWDG->PSCR = IWDG_Prescaler_64;
    IWDG->RLDR =  625;
    IWDG->CTLR = CTLR_KEY_Reload;
    IWDG->CTLR = CTLR_KEY_Enable;

#endif
}

