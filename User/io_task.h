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

uint8_t getKeyData();
TaskHandle_t * xGetIOTaskHandle ();
void vIOTask(void *argument);
#endif