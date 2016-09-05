#ifndef __STM32_USART_H
#define __STM32_USART_H

#include "includes.h"
 
//485接受发送控制
#define MDE_H	  GPIOA->BSRR = GPIO_Pin_8
#define MDE_L	  GPIOA->BRR  = GPIO_Pin_8

void USART1_SandData(u8 *str,u8 length);
void USART3_SandData(u8 *str,u8 length);
u16 CRC_16(u8 *puchMsg,u8 count) ;

#endif
