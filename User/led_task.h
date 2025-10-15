/*
 * led.h
 *
 *  Created on: Nov 12, 2021
 *      Author: igor.dymov
 */

#ifndef INC_LED_H_
#define INC_LED_H_


#include "main.h"
#include "types.h"




#define MAX_BRIGTH  15
#define BRIGTH_CONTER 200
#define BRIGTH_R 200
#define BRIGTH_G 80
#define BRIGTH_B 150


#define  MAX_DATA 0xFF
#define  MAX_BRIGTH_COUNTER    150//MAX_BRIGTH *2
#define  OFF 0x00

#define RGB_LED_COUNT   14
#define COLOR_COUNT      3
#define BAR_LED_COUNT  16

#define SPI_PACKET_SIZE 3U
#define SPI_TIMEOUT		100U

#define SPI1_CHIP_COUNT     5
#define SPI2_CHIP_COUNT     4

#define BAR_GREEN_WORD   0
#define BAR_RED_WORD     1
#define RGB_BLUE_WORD    2
#define RGB_GREEN_WORD   3
#define RGB_RED_WOED     4


extern const u8 DigitMask[];
void SetSegPoint( u8 on);
void SetSegDirect( u8 number, u8 mask);
void SetSEG( u16 mask, u32 value, u8 point);
void vSetInitBrigth(BRIGTH_CHANNEL_t ch, u8 brigth);
void vSetBrigth( BRIGTH_CHANNEL_t ch, u8 brigth);
void SetBigSeg( u16 mask);
void SetBarState( u8 start_g, u8 count_g, u8 start_r, u8 count_r );
void SetRGB(  u8 number, LED_COLOR_t color, LED_STATE_t state);
void vRGBProcess();
void vLedDriverStart(void);
void vLedProcess( void );
void SetDataSEG( u16 mask, u16 channel);
#endif /* INC_LED_H_ */
