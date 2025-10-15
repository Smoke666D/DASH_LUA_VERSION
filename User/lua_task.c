/*
 * lua_task.c
 *
 *  Created on: Feb 14, 2025
 *      Author: i.dymov
 */
#include "lua_task.h"
#include "lprefix.h"
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
#include "lapi.h"
#include "hw_lib_can.h"
#include "hal_timers.h"
#include "led_task.h"
#include "io_task.h"
#include "string.h"
#include "hal_wdt.h"


#define  FLASH_STORAGE_ADR         0x00025000UL
#define  FLASH_STORAGE_LENGTH      0x2800U
#define  FLASH_STORAGE_LENGTH_SIZE 4U

/*
Global variables
*/
static TaskHandle_t  LuaTaskHandle;

static char * pcLuaErrorString;

/* 
Static function
*/
static int iCanSetConfig(lua_State *L);
static int iCanSendData( lua_State *L );
static int iCanSetResiveFilter(lua_State *L );
static int iCanGetResivedData(lua_State *L );
static int iCanResetResiveFilter(lua_State *L );
static int iCanCheckData( lua_State *L );
static int iSetCanNodeID( lua_State *L );
static int iGetKeyMask(lua_State *L);
static int iSetLedRed(lua_State *L);
static int iSetLedGreen(lua_State *L);
static int iSetLedBlue(lua_State *L);
static int iSetBackBrigth(lua_State *L);
static int iSetBackColor(lua_State *L);
static int iSetLedBrigth(lua_State *L);

TaskHandle_t * xGetLuaTaskHandle ()
{
    return  &LuaTaskHandle ;
}

static RESULT_t eIsLuaSkriptValid(const uint8_t* pcData, uint32_t size, uint32_t * real_size)
{
	uint8_t ucRes = RESULT_FALSE;
  if (size < MAX_SCRIPT_SIZE)
  {
    if ( ( pcData[0] == LUA_SIGNATURE[0] ) && ( pcData[1] == LUA_SIGNATURE[1] ) && ( pcData[2] == LUA_SIGNATURE[2] ) && (pcData[3] == LUA_SIGNATURE[3]) )
		{
            printf("Find signature\r\n");
            *real_size =size;
					  ucRes = RESULT_TRUE;
		}
    else 
    {
	      for (uint32_t ulIndex = 0;ulIndex < size; ulIndex++)
	      {
		          if ( pcData[ulIndex] == 0x00 )
              {
                  *real_size =ulIndex;
					        ucRes = RESULT_TRUE;
					        break;
				      }
			  }
		}
  }
	return ( ucRes );
}



const uint8_t* uFLASHgetScript ( void )
{
  return ( const uint8_t* )( FLASH_STORAGE_ADR + FLASH_STORAGE_LENGTH_SIZE );
}
uint32_t uFLASHgetLength ( void )
{
  uint32_t size ;
  size = *( uint8_t* )( FLASH_STORAGE_ADR  );
  size |= (*( uint8_t* )( FLASH_STORAGE_ADR  +1 ))<<8;
  size |= (*( uint8_t* )( FLASH_STORAGE_ADR  +2 ))<<16;
  size |= (*( uint8_t* )( FLASH_STORAGE_ADR  +3 ))<<24;
  return  size;
}

static const luaL_Reg dev_funcs[] = {
  {"Send",           iCanSendData          },
  {"CheckFilter",    iCanCheckData         },
	{"GetFrame",       iCanGetResivedData    },
  {"SetFilter",      iCanSetResiveFilter   },
  {"ResetFilter",    iCanResetResiveFilter },
	{"Config",         iCanSetConfig         },
  {"SetNodeID",      iSetCanNodeID         },
  {"GetKeys",     iGetKeyMask}, 
  {NULL, NULL}
};

LUAMOD_API int luaopen_dev (lua_State *L) {
  luaL_newlib(L, dev_funcs);
  return 1;
}

static const luaL_Reg userlibs[] = {

  {"Keypad8", luaopen_dev  },
  {NULL, NULL}
};


LUALIB_API void luaL_openuser (lua_State *L) {
  const luaL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = userlibs; lib->func; lib++) {
    luaL_requiref(L, lib->name, lib->func, 1);
    lua_pop(L, 1);  /* remove lib */
  }
}


void vLuaTask( void * argument )
{
    int res ;
    TickType_t xLastWakeTime;
    lua_State *L1 = NULL;
    LUA_STATE_t lua_state = LUA_INIT;
    uint32_t ulWorkCicleIn10us;
    uint32_t mid_time= 0;
    xLastWakeTime = xTaskGetTickCount();
#ifdef DEBUG_PRINT
    uint16_t counter = 0;
    uint32_t max_clock = 0;

#endif
    while(1)
    {
        vTaskDelayUntil( &xLastWakeTime,2);
        HAL_WDTReset();
        switch (lua_state)
        {
            case LUA_INIT:
               lua_state = LUA_ERROR;
               L1  = luaL_newstate();  //Созадем состояние LUA, занимет 2К оперативной памяти
               luaL_openlibs(L1);      //Подлючаем библиотеки
               //Регестрируем пользовательские функции
               luaL_openuser(L1);
              
              uint32_t real_size;
              if ( eIsLuaSkriptValid(uFLASHgetScript(), uFLASHgetLength()+1,&real_size) == RESULT_TRUE )
	   	         {     
	   	    	      if (luaL_loadbuffer(L1, uFLASHgetScript(), real_size , uFLASHgetScript())==0 )  
                  {
                   
                    if  (lua_pcall(L1, 0, LUA_MULTRET, 0) == 0 )
                    {
                        lua_getglobal(L1, "main");
#ifdef DEBUG_PRINT
                            printf("Memory %d\r\n",lua_gc(L1,LUA_GCCOUNT,0)*1024);
#endif
                            HAL_TiemrEneblae(TIMER1);
                            lua_getglobal(L1, "main");  //Закидываем в стек глобальнйо имя рабочей функции lua скрипта
                            lua_state = LUA_RUN;
                    }
                  }
                  else 
                  {
                      pcLuaErrorString =  (char *) lua_tostring( L1, LAST_ARGUMENT );
#ifdef DEBUG_PRINT
                      printf("Error = %s\r\n",pcLuaErrorString);
#endif
                  }  
	   	          }
	   	          else
	   	          {
                  printf("File break\r\n");
                }
               break;
            case LUA_RUN:
                ulWorkCicleIn10us= HAL_GetTimerCnt(TIMER1);
                HAL_TimerReset(TIMER1);
                lua_pushinteger(L1, ulWorkCicleIn10us);
                res = lua_resume(L1,0,1);       //Возобновляем выполнение скрита
#ifdef DEBUG_PRINT
                mid_time+=ulWorkCicleIn10us;
                if (++counter == 1000)
                {
                  counter=0;
                  if( max_clock<ulWorkCicleIn10us) max_clock =ulWorkCicleIn10us;
                  printf("timer = %i max_timer=%i\n",ulWorkCicleIn10us,mid_time/1000);
                  mid_time = 0;
                  printf("Memory %d\r\n",lua_gc(L1,LUA_GCCOUNT,0)*1024);
                }
#endif
                
                switch ( res)
                {
                    case LUA_OK:
                    case LUA_YIELD:
                         break;
                    default:
	   	   	              pcLuaErrorString =  (char *) lua_tostring( L1, LAST_ARGUMENT );
#ifdef DEBUG_PRINT
                        printf("Error = %s\r\n",pcLuaErrorString);
#endif
                        lua_state = LUA_ERROR;
                        break;  
                }
                lua_pop(L1,1);
                break;
              case LUA_ERROR:
                break;
        }
    }
}


/*
Функция отправки пакета по CAN. Вариативное кол-во агрументов, в завимости от DLC фрейма или передачи данных в таблице
*/
static int iCanSendData( lua_State *L )
{
    CAN_TX_FRAME_TYPE frame ={0};
    int parametr_count = lua_gettop(L);
    if ( parametr_count>= THREE_ARGUMENTS )
    {
        frame.rtr   = (uint32_t)lua_tointeger(L, THIRD_ARGUMENT);
        frame.extd  = (uint32_t)lua_tointeger(L, SECOND_ARGUMENT);
        frame.ident = (uint32_t)lua_tointeger(L, FIRST_ARGUMENT);
        if (parametr_count >= FOUR_ARGUMENTS )
        {
            if (lua_istable(L, LAST_ARGUMENT))   //Проверяем что в качестве аргумента передали таблицу
            { 	
			          frame.DLC 	 = luaL_len(L, LAST_ARGUMENT);  //Читаем рамер таблицы
                if (frame.DLC  > CAN_FRAME_SIZE) frame.DLC = 8;
				        for (uint8_t i = 0; i < frame.DLC ; i++)
				        {
					        lua_geti(L, LAST_ARGUMENT , i +1 );               //Получаем элемент таблицы
					        frame.data[i]=   lua_tointeger(L,LAST_ARGUMENT);  //Вытаскиваем данные из таблицы
                  lua_pop(L,1);                                     //Убираем значение из стека
                }
            }
            else 
            {
              frame.DLC =  parametr_count - THREE_ARGUMENTS;
              if (frame.DLC  > CAN_FRAME_SIZE) frame.DLC = 8;
              for (int i=0; i< (frame.DLC) ;i++)
		          {
			            frame.data[i]= (uint8_t) lua_tointeger(L,-( frame.DLC-i)); 
		          }
          }                        
				}
				APPCANSEND(&frame);
			}
	  return ( NO_RESULT );
}

/*
Инициализация контрллера CAN
*/
static int iCanSetConfig(lua_State *L)
{
  if (lua_gettop(L) == ONE_ARGUMENT)
  {
	  vCANBoudInit( (uint16_t)lua_tointeger( L, FIRST_ARGUMENT) );
  }
	return ( NO_RESULT );
}
/*
Инициализация контрллера CAN
*/
static int iSetCanNodeID(lua_State *L)
{
	if (lua_gettop(L) == ONE_ARGUMENT)
	{
    ConfigNodeID( (uint8_t) lua_tointeger( L, FIRST_ARGUMENT)); 
	}
	return ( NO_RESULT );
}

static int iGetKeyMask(lua_State *L)
{
    lua_pushinteger(L, getKeyData());
    return ( ONE_RESULT );
}






/*
Функция установки CAN фильторв
*/
static int iCanSetResiveFilter(lua_State *L )
{
  uint8_t ucResNumber = NO_RESULT;
  if (lua_gettop(L) == THREE_ARGUMENTS )  /*Проверяем, что при вызове нам передали нужное число аргументов*/
  {
	  lua_pushnumber(L, eMailboxFilterSet( ( uint32_t ) lua_tointeger(L,FIRST_ARGUMENT ),
                                                      lua_tointeger(L,SECOND_ARGUMENT ),
                                                      lua_tointeger(L,THIRD_ARGUMENT )
                                                      ) );
	  ucResNumber = ONE_RESULT;
  }
  return ( ucResNumber );
}

/*
Функция сброса фильтра CAN
*/
static int iCanResetResiveFilter(lua_State *L )
{
  if (lua_gettop(L) == ONE_ARGUMENT )  /*Проверяем, что при вызове нам передали нужное число аргументов*/
  {
    eMailboxFilterReset(  lua_tointeger(L,FIRST_ARGUMENT) );
  }
  return ( NO_RESULT );
}

/*
Функция проерки пришле ли нужный пакет
В качестве параметра передается CAN_ID для проверки. В случае если параметров не переадно,
проверяеться фильтр, установленный ранее функцией iCanSendRequest
*/
static int iCanCheckData(lua_State *L )
{
	uint32_t uiRes = 0U;
	if (lua_gettop(L) == ONE_ARGUMENT ) 
  {
		  uiRes =  uCheckMailBoxData( lua_tointeger(L, FIRST_ARGUMENT));                                   		 
	}
	lua_pushnumber(L, uiRes );
	return ( 1U );
}


/*
Функция получаения пакет CAN 
*/
static int iCanGetResivedData(lua_State *L )
{
	uint8_t n;
  uint8_t res = 0;
  int8_t mail_box_index= 0;
	CAN_FRAME_TYPE  RXPacket;
	if (lua_gettop(L) >= ONE_ARGUMENT )
	{
    mail_box_index = lua_tointeger(L,FIRST_ARGUMENT );
   
    if  ( GetMailBoxData( mail_box_index,&RXPacket ) == 1 )
    {
      if ((lua_gettop(L)==TWO_ARGUMENTS) && lua_istable(L, LAST_ARGUMENT))   //Проверяем что в качестве аргумента передали таблицу
      { 
	      n = luaL_len(L, -1);
	    	for (int i = 1; i<(n+1);i++)
	    	{
	    	  lua_pushnumber(L,RXPacket.data[i-1]);
	    		lua_seti(L, -2, i);
	      }
	    	res = 1U;
      }
      else 
      {	    
		    res = RXPacket.DLC;
		    for (int i = 0; i < res;i++)
		    {
			    lua_pushnumber(L,RXPacket.data[i]);
		    }
        return (res );
      }      
    }
	}
  lua_pushnumber(L,res );
	return ( 1U );
}

