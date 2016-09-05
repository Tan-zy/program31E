#ifndef DS18B20_H
#define DS18B20_H

#include "includes.h"

/**********DS18B20Ӳ����������************************
 ����Ķ�����ֻ���ڴ˴��޸�
 ****************************************************/
#define DS18B20_Pin		GPIO_Pin_7 
#define DS18B20_PORT	GPIOC 
/***********************************/
#define READ_DS18B20() GPIO_ReadInputDataBit(DS18B20_PORT, DS18B20_Pin) 

#define DIR_1WIRE_IN() {DS18B20_PORT->CRL &= 0x0FFFFFFF;DS18B20_PORT->CRL |= 0x800000000;}   //4 ������������ 
#define CLR_OP_1WIRE() GPIO_ResetBits(DS18B20_PORT, DS18B20_Pin) //	�����0��
#define SET_OP_1WIRE() GPIO_SetBits(DS18B20_PORT, DS18B20_Pin) 	 // �����1��
#define CHECK_IP_1WIRE() GPIO_ReadInputDataBit(DS18B20_PORT,DS18B20_Pin)//��ȡ���ŵ�ƽ
extern u16 TEMP_Get_Temp;

extern u16 Get_Temp(void);
extern void DS18B20_Convert(void);
#endif
