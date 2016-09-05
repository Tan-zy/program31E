#ifndef	__BL5372_H
#define	__BL5372_H
#include "Object.h"
#include "includes.h"



#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 

#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7



bool Init_BL5372(PTIME_S time);
bool I2C_BL5372_WriteByte(u8 SendByte, u16 WriteAddress);
bool BL5372_SetBCD(PTIME_S timeBCD);
bool BL5372_Read(PTIME_S time);
bool BL5372_ReadBCD(PTIME_S timeBCD);
//void Inside_Temp(void);
void F_Difference(u32 frequency);
u8 Get_Week(u8 year, u8 month, u8 day);
#endif

