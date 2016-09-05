#include "DS18B20.h"

#include <math.h>

void DIR_1WIRE_OUT(void)	  //����DQ����Ϊ��©���ģʽ 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DS18B20_Pin;		  //GPIO_pin_4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //��©���
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);	//GPIOC
}

void init_1820()   		//��ʼ��
{
	DIR_1WIRE_OUT();    //�л������ģʽ
	SET_OP_1WIRE();     // �������߼���λ����
	CLR_OP_1WIRE();     // 	��������
	Delay_us(650);      //  ����550us����С480us���960us
	SET_OP_1WIRE();     // �������ߣ���15��60us��18b20�������߱�ʾ����
	Delay_us(70);      // ��ʱ�����߱�18b20���ͣ��˴�������Ϊ18b20����һ���������ټ���������
	SET_OP_1WIRE();   	//��λ���ߣ���ʼ�����
	Delay_us(160);   //
}

void write_1820(u8 Write_Byte)
{
	u16 i;
	for (i = 0; i < 8; i++)
	{
		DIR_1WIRE_OUT();
		CLR_OP_1WIRE();   //дʱ�Ƚ���������������
		Delay_us(15);
		if (Write_Byte & 0x01)    //д�����ˣ���д��λ�ģ�
		{
			SET_OP_1WIRE();
		}
		else
		{
			CLR_OP_1WIRE();
		}
		Write_Byte >>= 1;
		Delay_us(45);   //����дʱ���϶���60us
		SET_OP_1WIRE();
	}
	SET_OP_1WIRE();
}

u8 read_1820(void)
{
	u8 i;
	u8 Read_Byte = 0;

	DIR_1WIRE_OUT();
	for (i = 0; i < 8; i++)
	{
		CLR_OP_1WIRE();  //��ʱ�Ƚ���������������
		Read_Byte = Read_Byte >> 1;
		SET_OP_1WIRE();

		DIR_1WIRE_IN()
		;
		Delay_us(3);
		if (CHECK_IP_1WIRE() == 1) //������,�ӵ�λ��ʼ
		{
			Read_Byte |= 0x80;
		}
		Delay_us(60); //������ʱ���϶��С60us
		DIR_1WIRE_OUT();
	}
	return (Read_Byte);
}

void DS18B20_Convert(void) //�¶�ת�� 
{
	init_1820();        //��λ18b20
	write_1820(0xcc);   // ����ת������
	write_1820(0x44);
}

extern u16 Get_Temp(void)			//��DS18B20��ȡ�¶Ⱥ���
{
	u8 TLV = 0;			// ,j
	u8 THV = 0;
	s16 tmp = 0;
	s16 temp_data = 0;

//    T[0]=0;//
	DS18B20_Convert();

	init_1820();

	write_1820(0xcc);  					//����������
	write_1820(0xbe);

	TLV = read_1820();  					//������
	THV = read_1820();

	tmp = (THV << 8) | TLV;					//temp�����λ����λ�������λС��λ���м�8λ����λ

	/*if((tmp&0xf000)!=0)				//����λΪ��
	 {
	 tmp = ~tmp+1 ;
	 TempNow_Sign=1;				//��ǰ�¶�Ϊ��
	 }else
	 {
	 TempNow_Sign=0;				//��ǰ�¶�Ϊ��
	 }*/

	temp_data = ((tmp >> 4) & 0x00ff) * 10;
	temp_data = temp_data + ((tmp & 0x000f) * 0.0625) * 10;

	return temp_data;
}

