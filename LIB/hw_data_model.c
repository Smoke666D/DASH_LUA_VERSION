/*
 * hw_data_model.c
 *
 *  Created on: Dec 23, 2024
 *      Author: i.dymov
 */
#include "hw_data_model.h"

#if TOTAL_REGISTER_COUNT>0
static uint8_t DATA_MODEL_REGISTER[TOTAL_REGISTER_COUNT];




uint8_t * GetDataRegister()
{
    return DATA_MODEL_REGISTER;
}

uint8_t * GetRegisterAddr( uint16_t addr)
{
    return ( &DATA_MODEL_REGISTER[addr] );
}

void ClearDataModel()
{
   // for (uint32_t i =0;i < TOTAL_REGISTER_COUNT;i++)
  //      DATA_MODEL_REGISTER[i] = 0;
    memset(DATA_MODEL_REGISTER,0x00,TOTAL_REGISTER_COUNT);
}


void setReg32( u16 reg_adress, u32 data)
{
    DATA_MODEL_REGISTER[ reg_adress]     = (u8)( data & 0xFF);
    DATA_MODEL_REGISTER[ reg_adress + 1] = (u8)( data>>8 & 0xFF);
    DATA_MODEL_REGISTER[ reg_adress + 2] = (u8)( data>>16 & 0xFF);
    DATA_MODEL_REGISTER[ reg_adress + 3] = (u8)( data>>24 & 0xFF);
}

void setReg16( u16 reg_adress, u16 data)
{

   DATA_MODEL_REGISTER[ reg_adress] = (u8)( data & 0xFF);
   DATA_MODEL_REGISTER[ reg_adress + 1] =(u8)( data>>8 & 0xFF);
}

void setReg8( u16 reg_adress, u8 data)
{
   DATA_MODEL_REGISTER[ reg_adress] = (u8)( data );
}




u16 getReg16(u16 reg_adress )
{
    uint16_t  data =  (u16)DATA_MODEL_REGISTER[reg_adress] | (u16)(DATA_MODEL_REGISTER[reg_adress+1])<<8;
    return  data;
}

u32 getReg32(u16 reg_adress )
{
    u32 data = (u32)DATA_MODEL_REGISTER[reg_adress] | (u32)(DATA_MODEL_REGISTER[reg_adress+1])<<8 |
            (u32)DATA_MODEL_REGISTER[reg_adress+2]<<16 | (u32)(DATA_MODEL_REGISTER[reg_adress+3])<<24;
    return  (data);
}

u8 getReg8( u16 reg_adress)
{
    return DATA_MODEL_REGISTER[reg_adress];
}



float getRegFloat(u16 reg_adress )
{
    float * ptemp;
    u32 data = (u32)DATA_MODEL_REGISTER[reg_adress] | (u32)(DATA_MODEL_REGISTER[reg_adress+1])<<8 |
            (u32)DATA_MODEL_REGISTER[reg_adress+2]<<16 | (u32)(DATA_MODEL_REGISTER[reg_adress+3])<<24;
    ptemp = (float *)&data;
    return  (*ptemp);
}
void setRegFloat( u16 reg_adress, float data)
{
    float temp = data;
    u8 * pdata = (u8 *)&temp;
    DATA_MODEL_REGISTER[ reg_adress]     =  pdata[0];
    DATA_MODEL_REGISTER[ reg_adress + 1] =  pdata[1];
    DATA_MODEL_REGISTER[ reg_adress + 2] =  pdata[2];
    DATA_MODEL_REGISTER[ reg_adress + 3] =  pdata[3];
}
#endif