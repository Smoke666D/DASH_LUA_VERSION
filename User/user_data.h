#ifndef USER_DATA_H_
#define USER_DATA_H_

#include "main.h"
#include "system_init.h"
#include "hw_lib_adc.h"
#include "hw_data_model.h"


#define SW_V                 1
#define SW_V2                1
#define SW_V3               22

#define VALID_CODE            ((SW_V2<<4) | (SW_V3))
#define VALID_CODE_ADDRES     0

#define ODOMETR_ADR           ( VALID_CODE_ADDRES  +1 )
#define HOUR_COUNTER_ADR       (ODOMETR_ADR + 4 )
#define ODOMETR1_ADR           (HOUR_COUNTER_ADR  + 4 )
#define ODOMETR_MAP            (ODOMETR1_ADR  + 4 )
#define BITRATE_ADR            (ODOMETR_MAP + 1)
#define RGB_BRIGTH_ADR         (BITRATE_ADR  +1)
#define WHITE_BRIGTH_ADR       (RGB_BRIGTH_ADR + 1)

#define EEPROM_REGISER_COUNT  ( WHITE_BRIGTH_ADR + 1 )

typedef struct
{
    u16 addr;
    u8 len;
} EEPROM_REG_Q_t;

void vIncrementSystemCounters( void );
void DataModel_Init( void );

#endif