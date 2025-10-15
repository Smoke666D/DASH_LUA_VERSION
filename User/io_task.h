#ifndef USER_IO_TASK_H_
#define USER_IO_TASK_H_

#include "main.h"
#define KEY_COUNT           8
#define ADC1_DATA_READY 0x01
#define K1   0x01
#define K2   0x02
#define K3   0x04
#define K4   0x08
#define K5   0x10
#define K6   0x20
#define K7   0x40
#define K8   0x80

#define AIN_MAX_BUFFER_SIZE 100
#define RA  10000.0
#define R22 15000.0
#define R21 10000.0
#define VADD_COOF ((R21+R22)/R22)
#define Rgnd  15000.0
#define Rup   10000.0
#define Rpup  3000.0
#define RpupAIN3  500.0
#define AIN_COOF   ((Rup+Rgnd)/Rgnd)
#define DIOD    0.2

typedef enum
{
    START_UP_STATE,
    RUN_STATE_INIT,
    RUN_STATE,
    SAVE_STATE,
} INPUTS_FSM_t;

uint8_t getKeyData();
TaskHandle_t * xGetIOTaskHandle ();
void vIOTask(void *argument);
#endif