#include "led_task.h"
#include "hal_dma.h"
#include "hal_spi.h"
#include "init.h"
#include "string.h"
#include "hal_gpio.h"
#include "hal_timers.h"
#include "math.h"


static uint8_t  LED_ON[SPI_PACKET_SIZE] 		= { 0x00 , 0x00 , 0x00 };
static uint16_t backligch_brigth		        = 0x1F;
static uint16_t led_brigth 				        = 0x3F;
static uint8_t  color_div 				        = 1U;
static uint8_t  brigth_color[SPI_PACKET_SIZE];
static uint16_t led_brigth_counter 		        = 0;
static uint8_t  data[SPI_PACKET_SIZE];


static void vLedProcess( void );


void LC()
{
   HAL_DMA_Disable(DMA1_CH5);
   HAL_SPI_RXOveleyClear(HAL_SPI2 );
   while (HAL_SPI_GetBusy(HAL_SPI2) == HAL_SET);
   HAL_SetBit(CS_Port, CSPin);
}

/*
 *
 */
static uint16_t calcBrigt(uint8_t pbr)
{
  return ( ( pbr > MAX_BRIGTH )?  MAX_BRIGTH_COUNTER :  (uint16_t) ( sin((double)pbr*(3.14/2.0)/MAX_BRIGTH )*(MAX_BRIGTH_COUNTER) ) );
}
/*
 *
 */

/*
 *
 */
INIT_FUNC_LOC void vLedDriverStart(void)
{
    DMA_INIT_t init;
    init.stream = DMA1_CH5;
    init.direction = MTOP;
    init.mode  = DMA_Mode_Normal;
    init.paddr = (u32)&SPI2->DATAR;
    init.memadr = (u32)data;
    init.dma_size = DMA_BYTE;
    init.bufsize = SPI_PACKET_SIZE;
    init.prioroty = dma_VeryHigh;
    HAL_DMAInitIT(init, DMA1_CH5_PRIOR  , DMA1_CH5_SUBPRIOR , &LC  );
	HAL_TIMER_InitIt( TIMER3, 10000000, 900, &vLedProcess,TIMER3_PRIOR,TIMER3_SUBPRIOR );
    HAL_TiemrEneblae(TIMER3);
    return;
}
/*
 * Функция включения светодиодоы.
 * Корректность аргументов проверятся при вызове
 */
void vSetLedOn(uint8_t Color,uint8_t State)
{

	if (LED_ON[Color-1] != State) LED_ON[Color-1] = State  ;
	return;
}

uint8_t uGetLedState( uint8_t Color )
{
    return (LED_ON[Color-1]);
}

/*
 *
 */
void vSetLedBrigth(uint8_t brigth)
{
	
	led_brigth = calcBrigt(brigth);
	return;
}

void vSetBackLigthColor(uint8_t color)
{
	brigth_color[0]=MAX_DATA;
	brigth_color[1]=MAX_DATA;
	brigth_color[2]=MAX_DATA;
	color_div =2;
	switch (color)
	{
		case  RED:
			brigth_color[1]=0x00;
			brigth_color[2]=0x00;
			color_div =1;
			break;
		case GREEN:
			brigth_color[0]=0x00;
			brigth_color[2]=0x00;
			color_div =1;
			break;
		case BLUE:
			brigth_color[0]=0x00;
			brigth_color[1]=0x00;
			color_div =1;
			break;
		case YELLOW:
			brigth_color[2]=0x00;
			break;
		case YELLOW_GREEN:
			brigth_color[2]=0x00;
			break;
		case  AMBER:
			brigth_color[2]=0x00;
			break;
		case VIOLET:
			brigth_color[1]=0x00;
			break;
		case CYAN:
			brigth_color[0]=0x00;
			break;
		case WHITE:
		default:
			break;
	}
	return;
}
/*
 *
 */
void vSetBackLigth(uint8_t brigth)
{
	
	if (calcBrigt( brigth) != backligch_brigth)
	{
		printf("set br %x  %x\r\n",brigth,backligch_brigth);
		backligch_brigth =calcBrigt( brigth);
	}
}

/*
 *  Функция вывода данных в SPI, вызывается по прерыванию таймра №4
 */
static void vLedProcess( void )
{
	/*Cбравысваем флаг тамера 4*/
	uint8_t temp_led;
    if (++led_brigth_counter>(MAX_BRIGTH_COUNTER))
    {
    	led_brigth_counter = 0;
    }
    memset(data,0, SPI_PACKET_SIZE );
    temp_led = ~(LED_ON[0]  | LED_ON[1] | LED_ON[2] );
	if (led_brigth_counter < backligch_brigth)
	{
		 data[2]=brigth_color[0] & temp_led;
	 	 data[1]=brigth_color[1] & temp_led;
	 	 data[0]=brigth_color[2] & temp_led;
 	}
	if (led_brigth_counter < led_brigth)
	{
		data[2]|=LED_ON[0];
	 	data[1]|=LED_ON[1];
	 	data[0]|=LED_ON[2];
    }
	HAL_ResetBit(CS_Port, CSPin);
	HAL_DMA_SetCouterAndEnable(DMA1_CH5, SPI_PACKET_SIZE );
    return;
}

