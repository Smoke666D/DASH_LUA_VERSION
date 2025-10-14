#include "io_task.h"
#include "hal_dma.h"
#include "string.h"
#include "hw_lib_keyboard.h"
#include "init.h"

static TaskHandle_t  IOTaskHandle;
static uint8_t  STATUS[KEY_COUNT];
static uint8_t  COUNTERS[KEY_COUNT];
static MessageBufferHandle_t pKeyboardMessageBuffer;
static KeyEvent          TempEvent        = { 0U };

TaskHandle_t * xGetIOTaskHandle ()
{
    return  &IOTaskHandle ;
}

BitState_t fPortState (uint8_t i)
{
    switch (i)
    {
        case 0:
            return HAL_GetBit( KL1_Port, KL1Pin  );
        case 1:
            return HAL_GetBit( KL2_8_Port, KL2Pin  );
        case 2:
            return HAL_GetBit( KL2_8_Port, KL3Pin  );
        case 3:
            return HAL_GetBit( KL2_8_Port, KL4Pin  );
        case 4:
            return HAL_GetBit( KL2_8_Port, KL5Pin  );
        case 5:
            return HAL_GetBit( KL2_8_Port, KL6Pin  );
        case 6:
            return HAL_GetBit( KL2_8_Port, KL7Pin  );
        case 7:
            return HAL_GetBit( KL2_8_Port, KL8Pin  );
        default:
            return 0;
    }
}



void vInitKeybord()
{
    KeybaordStruct_t KeyboardInit;
    KeyboardInit.KEYBOARD_COUNT    = KEY_COUNT;
    KeyboardInit.COUNTERS          = COUNTERS;
    KeyboardInit.STATUS            = STATUS;
    KeyboardInit.REPEAT_TIME       = 3;
    KeyboardInit.KEYDOWN_HOLD_TIME = 4;
    KeyboardInit.KEYDOWN_DELAY     = 2;
    KeyboardInit.KEYBOARD_PERIOD   = 20;
    KeyboardInit.getPortCallback = &fPortState;
    eKeyboardInit(&KeyboardInit);
}

static uint8_t data;

uint8_t getKeyData()
{
    return data;
}

void vIOTask(void *argument)
{
 static uint8_t key_mask;
   vInitKeybord();
   pKeyboardMessageBuffer= *(xKeyboardMessageBuffer());
	while(1)
	{  
		vTaskDelay(1); 
        HW_LIB_KeyboradFSM();
        if ( xMessageBufferReceive(pKeyboardMessageBuffer,&TempEvent,sizeof(KeyEvent),0) != 0)
		{
			switch (TempEvent.KeyCode)
			{
				case kl1_key:
				   key_mask = K1;
				   break;
				case kl2_key:
				   key_mask = K2;
			   	   break;
				case kl3_key:
				   key_mask = K3;
			   	   break;
				case kl4_key:
				   key_mask = K4;
 			   	   break;
				case kl5_key:
				   key_mask = K5;
   			   	   break;
				case kl6_key:
				   key_mask = K6;
			  	   break;
				case kl7_key:
				   key_mask = K7;
			   	   break;
				case kl8_key:
				   key_mask = K8;
			   	   break;
				default:
				   key_mask = 0U;
				   break;
			}
			if ( TempEvent.Status == MAKECODE )
			{
				data |= key_mask;
			}
			else
			{
				data &= ~key_mask;
			}
        }
	}
}