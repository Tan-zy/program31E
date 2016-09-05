#ifndef DELAY_H
#define DELAY_H 			   

//#include "stm32f10x.h"
#include "includes.h"	 	 
void delay_init(u8 SYSCLK);
void Delay_us(u32 nus);
void Delay_ms(u16 nms);

#endif

