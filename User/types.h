/*
 * types.h
 *
 *  Created on: Feb 29, 2024
 *      Author: i.dymov
 */

#ifndef USER_TYPES_H_
#define USER_TYPES_H_

typedef enum
{
    STATE_ON = 1,
    STATE_OFF  = 0,
} LED_STATE_t;

typedef enum
{
    RED_COLOR   =  1,
    GREEN_COLOR  = 2,
    BLUE_COLOR   = 3,
    WHITE_COLOR  = 4,
    NO_COLOR     = 5,
} LED_COLOR_t;

typedef struct {
    uint32_t Pin;
    GPIO_TypeDef * GPIOx;
} PIN_CONFIG;

typedef enum
{
    RGB_CHANNEL = 0,
    WHITE_CHANNEL = 1,
} BRIGTH_CHANNEL_t;



#endif /* USER_TYPES_H_ */