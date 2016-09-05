#ifndef DISPLAY_CHIP_H_ 
#define	DISPLAY_CHIP_H_
//#include "stm32f10x.h"
#include "includes.h"
#define CLK_H         GPIOB->BSRR = GPIO_Pin_8
#define CLK_L         GPIOB->BRR  = GPIO_Pin_8 
   
#define DIO_H         GPIOB->BSRR = GPIO_Pin_9
#define DIO_L         GPIOB->BRR  = GPIO_Pin_9

#define STB_H         GPIOB->BSRR = GPIO_Pin_5
#define STB_L         GPIOB->BRR  = GPIO_Pin_5
 
/*  STLED316S */
void Delay_STLED316S(void);
void  SendSPIData( u8 data );
void InitSTLED316( u8 Brightness );   //≥ı ºªØ
void SetSlaverLed(u16 slaverLed);
void Write_8LED( u8 sel,u8 oneSecondsFlag,u8 data1,u8 data2,u8 data3,u8	data4,u8 data5 ,u8 data6);
//void Led(u8	LED1_Data,u8 LED2_Data);
#endif
