#ifndef  __TM1637_H__
#define __TM1637_H__
#include "stm32f10x.h"
#include "stm32_Init.h"
#define CLK_H  TM1637_PORT->BSRR = TM1637_DCLK
#define CLK_L  TM1637_PORT->BRR = TM1637_DCLK
#define DO_H   TM1637_PORT->BSRR = TM1637_DIO
#define DO_L   TM1637_PORT->BRR = TM1637_DIO
#define DI     GPIO_ReadInputDataBit(TM1637_PORT, TM1637_DIO)


typedef   unsigned char  uchar;



void Write_8LED( uchar sel ,
                uchar  oneSecondsFlag,
                  uchar	data1,
                  uchar  data2,
                 uchar	data3,
                 uchar  data4,
                 uchar	data5 )    ;          //写显示寄存器

void Write_LED(uchar	data);//控制单个灯

void TM1637_Init(unsigned char  brightness);//亮度调节



#endif