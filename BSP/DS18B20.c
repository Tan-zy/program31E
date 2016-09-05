#include "DS18B20.h"

#include <math.h>

void DIR_1WIRE_OUT(void)	  //设置DQ引脚为开漏输出模式 
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = DS18B20_Pin;		  //GPIO_pin_4
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD; //开漏输出
	GPIO_Init(DS18B20_PORT, &GPIO_InitStructure);	//GPIOC
}

void init_1820()   		//初始化
{
	DIR_1WIRE_OUT();    //切换到输出模式
	SET_OP_1WIRE();     // 拉高总线即复位总线
	CLR_OP_1WIRE();     // 	拉低总线
	Delay_us(650);      //  保持550us，最小480us最大960us
	SET_OP_1WIRE();     // 拉高总线，在15到60us内18b20拉低总线表示存在
	Delay_us(70);      // 延时到总线被18b20拉低，此处我们认为18b20工作一定正常不再监测存在脉冲
	SET_OP_1WIRE();   	//复位总线，初始化完成
	Delay_us(160);   //
}

void write_1820(u8 Write_Byte)
{
	u16 i;
	for (i = 0; i < 8; i++)
	{
		DIR_1WIRE_OUT();
		CLR_OP_1WIRE();   //写时先将总线拉高再拉低
		Delay_us(15);
		if (Write_Byte & 0x01)    //写数据了，先写低位的！
		{
			SET_OP_1WIRE();
		}
		else
		{
			CLR_OP_1WIRE();
		}
		Write_Byte >>= 1;
		Delay_us(45);   //整个写时间间隙最低60us
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
		CLR_OP_1WIRE();  //读时先将总线拉低再拉高
		Read_Byte = Read_Byte >> 1;
		SET_OP_1WIRE();

		DIR_1WIRE_IN()
		;
		Delay_us(3);
		if (CHECK_IP_1WIRE() == 1) //读数据,从低位开始
		{
			Read_Byte |= 0x80;
		}
		Delay_us(60); //整个读时间间隙最小60us
		DIR_1WIRE_OUT();
	}
	return (Read_Byte);
}

void DS18B20_Convert(void) //温度转换 
{
	init_1820();        //复位18b20
	write_1820(0xcc);   // 发出转换命令
	write_1820(0x44);
}

extern u16 Get_Temp(void)			//从DS18B20获取温度函数
{
	u8 TLV = 0;			// ,j
	u8 THV = 0;
	s16 tmp = 0;
	s16 temp_data = 0;

//    T[0]=0;//
	DS18B20_Convert();

	init_1820();

	write_1820(0xcc);  					//发出读命令
	write_1820(0xbe);

	TLV = read_1820();  					//读数据
	THV = read_1820();

	tmp = (THV << 8) | TLV;					//temp最高四位符号位，最低四位小数位，中间8位整数位

	/*if((tmp&0xf000)!=0)				//符号位为负
	 {
	 tmp = ~tmp+1 ;
	 TempNow_Sign=1;				//当前温度为负
	 }else
	 {
	 TempNow_Sign=0;				//当前温度为正
	 }*/

	temp_data = ((tmp >> 4) & 0x00ff) * 10;
	temp_data = temp_data + ((tmp & 0x000f) * 0.0625) * 10;

	return temp_data;
}

