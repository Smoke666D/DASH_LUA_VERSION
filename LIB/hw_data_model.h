/*
 * hw_data_model.h
 *
 *  Created on: Dec 23, 2024
 *      Author: i.dymov
 */

#ifndef HW_DATA_MODEL_H_
#define HW_DATA_MODEL_H_

#include "system_config.h"
#include "lib_config.h"

uint8_t * GetDataRegister();
void ClearDataModel();
void setReg32( u16 reg_adress, u32 data);
void setReg16( u16 reg_adress, u16 data);
void setReg8( u16 reg_adress, u8 data);
u32 getReg32(u16 reg_adress );
u16 getReg16(u16 reg_adress );
u8 getReg8( u16 reg_adress);

float getRegFloat(u16 reg_adress );
void setRegFloat( u16 reg_adress, float data);
uint8_t * GetRegisterAddr( uint16_t addr);

#endif /* HW_DATA_MODEL_H_ */
