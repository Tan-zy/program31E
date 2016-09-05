/************************************************************************************

File_name:       sd2068.h
Description:     the header file of  sd2068_c .

************************************************************************************/
#ifndef __SD2068_H
#define __SD2068_H

/*==================================================================================
@ Include files
*/
#include "includes.h"
#include "Object.h"
#include "BL5372.h"
/*==================================================================================
@ Typedefs
*/

/*==================================================================================
@ Constants and defines
*/
#define SCL_H         GPIOB->BSRR = GPIO_Pin_6
#define SCL_L         GPIOB->BRR  = GPIO_Pin_6 

#define SDA_H         GPIOB->BSRR = GPIO_Pin_7
#define SDA_L         GPIOB->BRR  = GPIO_Pin_7

#define SCL_read      GPIOB->IDR  & GPIO_Pin_6
#define SDA_read      GPIOB->IDR  & GPIO_Pin_7


bool SD2068_I2CReadDate(PTIME_S	psRTC);//读时间
bool SD2068_I2CWriteSingleReg( u8 value,u8 address);//写单个寄存器时间
bool SD2068_I2CWriteBCDDate(PTIME_S	psRTC);//can协议中用到的
bool Init_SD2068(PTIME_S	psRTC);


#endif 

/*@*****************************end of file**************************************@*/
