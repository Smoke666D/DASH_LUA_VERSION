#include "user_data.h"
#include "hw_lib_eeprom_i2c.h"



static u16 secondcounter;
static QueueHandle_t    pDataRegQueue;


QueueHandle_t * xDataRegQueue( void )
{
  return  (&pDataRegQueue);
}


INIT_FUNC_LOC  void DataModel_Init( void )
{

     if ( eEEPROMRd(0x00 ,GetDataRegister() , EEPROM_REGISER_COUNT,2) == EEPROM_OK)
    {
            if (getReg8(VALID_CODE_ADDRES)!=VALID_CODE )
            {

            }

    }

}



float getSpeed( void )
{

}

void setSpeed( void )
{


}

/*
 * Обработчик прерывания от секундного таймера RTC
 */
void vIncrementSystemCounters( void )
{
    u32 temp_odometr;
    if (++secondcounter >=  360 )
    {
        setReg32(HOUR_COUNTER_ADR,  (uint32_t)(getReg32(HOUR_COUNTER_ADR) + 1) );
        secondcounter = 0;
    }
    u32 distance = ((float)getSpeed())/10.0/3.6;
    temp_odometr = getReg32(ODOMETR_ADR)  + distance;
    if ( temp_odometr > 9999990 )  temp_odometr = 0;
    setReg32(ODOMETR_ADR,  temp_odometr );
    temp_odometr = getReg32(ODOMETR1_ADR) + distance;
    if ( temp_odometr > 999990 ) temp_odometr = 0;
    setReg32(ODOMETR1_ADR, temp_odometr);
    return;
}