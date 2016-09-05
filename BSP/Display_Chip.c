#include "Display_Chip.h"
//数码管显示字数组
//0 1 2 3 4 5 6 7 8 9 10  11 12	13 14 15 16	17 18 19 20	 21	22 23  24 25  26  27
//0 1 2 3 4 5 6 7 8 9  A  B  C  D  E  F  -  灭 N  t   L  H  r  o   P  U	  o	   J
#if (CURRENT_PRODUCT == 0x8632)
	static u8 slaverOutLed; //统计从机状态
#elif (CURRENT_PRODUCT == 0x8630)
	static u16 slaverOutLed; //统计从机状态
#endif

const u8 SegA[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
		0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x40, 0x00, 0x37, 0x78, 0x38, 0x76,
		0x50, 0x5c, 0x73, 0x3e, 0x5c, 0x0e };    //段码

void SetSlaverLed(u16 slaverLed)
{
	slaverOutLed = slaverLed;
}

static void Delay_STLED316S(void)  //延时
{
	u16 i = 0;
	u8 n = 0;
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr;
#endif
	OS_ENTER_CRITICAL()
	for (i = 0; i < 200; i++)
	{
		n++;
	}
	OS_EXIT_CRITICAL()
}

/************************************************************
 功能:		串口数据传送
 描述: 	    给stled316s发送数据
 参数:		data
 返回值:		无
 备注:
 ************************************************************/
static void SendSPIData(u8 data)
{
	u8 i = 0, l = 0;
	u8 n = 0;
#if OS_CRITICAL_METHOD == 3
	OS_CPU_SR cpu_sr;
#endif
	OS_ENTER_CRITICAL()
	for (i = 0; i < 8; i++)
	{
		CLK_L;
		for (l = 0; l < 100; l++)
		{
			n++;
		}
		if (data & 0x01 == 1)
		{
			DIO_H;
		}
		else
		{
			DIO_L;
		}
		data >>= 1;
		for (l = 0; l < 100; l++)
		{
			n++;
		}
		CLK_H;
		for (l = 0; l < 100; l++)
		{
			n++;
		}
	}
	OS_EXIT_CRITICAL()
}
/*转换从机状态位*/
static u8 convert(void)
{
	u8 sl_status = 0; //注:从机状态指示
	u8 temp = 0;
	u8 i = 0;

	for (i = 0; i < 8; i++)
	{
		temp = slaverOutLed >> i;

		if (i % 2 == 0) //注:对应灯的奇数位
		{
			if ((temp & 0x01) == 0x01)
			{
				sl_status |= (0x01 << (i / 2));
			}
		}
		else	//注:对应灯的偶数位
		{
			if ((temp & 0x01) == 0x01)
			{
				sl_status |= (0x01 << (i / 2 + 4));
			}
		}
	}

	return sl_status;
}

#if  (VERSION == 0x52)  //TME1668改变亮度
static u8 Light_Dim = 0;
/****************************************************************************
 功能:		stled316s初始化
 描述: 	    对数码管的个数、亮度及led的亮度进行初始化
 参数:		Brightness
 返回值:		无
 备注:
 ******************************************************************************/
void InitSTLED316( u8 Brightness )   //初始化
{
	if(Brightness == 0x11)
	{
		Light_Dim = 0x8A;
	}
	else
	{
		Light_Dim = 0x89;
	}
}
/****************************************************************************
 功能:		写数码管
 描述: 	    控制数码的显示状态
 参数:		data1,data2,data3,data4,data5,data6
 返回值:		无
 备注:
 ******************************************************************************/
void Write_8LED(u8 sel,u8 oneSecondsFlag,u8 data1,u8 data2,u8 data3,u8 data4,u8 data5, u8 data6)
{
	//u8 n = 0;
	
#if (CURRENT_PRODUCT == 0x8632)
    u8 slave_sta = 0;
	slave_sta = convert(); //转换从机状态
#elif (CURRENT_PRODUCT == 0x8630)
    u16 slave_sta = 0;
    slave_sta = slaverOutLed;
#endif
    
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x03);//显示模式 7wei 10 duan
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();

	STB_L;
	Delay_STLED316S();
	SendSPIData(0x40);//写数据到显示寄存器，地址自动加一
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();//__nop();

	STB_L;
	SendSPIData(0xC0);//住：起始地址00H
	Delay_STLED316S();
	//点亮数码管
	switch(oneSecondsFlag)//onesecondflag表要点亮哪个小数点及是否点亮冒号
	{
		//要点亮哪个数码管的小数点只需把高位置一即可
		case 0x00://小数点全不亮
		{
			if(sel == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
				SendSPIData(0);
			}
			else if(sel == 0x08)
			{
				SendSPIData(SegA[data1]);
				SendSPIData(0x01);
			}
			else if(sel == 0x18)
			{
				SendSPIData(SegA[data1]|0x80);
				SendSPIData(0x01);
			}
			else
			{
				SendSPIData(SegA[data1]);
				SendSPIData(0);

			}

			SendSPIData(SegA[data2]);
			SendSPIData(0);

			SendSPIData(SegA[data3]);
			SendSPIData(0);

			SendSPIData(SegA[data4]);
			SendSPIData(0);

			SendSPIData(SegA[data5]);
			SendSPIData(0);

#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData((0xf0&slave_sta)>>4); //注:点亮从机状态	风扇
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//注:点亮从机状态	   灯
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //注:点亮从机状态	9-15路
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//注:点亮从机状态 1-8路
			SendSPIData(0);
#endif
		}
		break;
		case 0x01:  //点亮中间冒号
		{
			if(sel==0x10)
			{
				SendSPIData(SegA[data1]|0x80);
			}
			else if(sel==0x08)
			{
				SendSPIData(SegA[data1]);
			}
			else if(sel==0x18)
			{
				SendSPIData(SegA[data1]|0x80);
			}
			else
			{
				SendSPIData(SegA[data1]);
			}

			if(sel==0x10)
			{
				SendSPIData(0);
			}
			else if(sel==0x08)
			{
				SendSPIData(0x01);
			}
			else if(sel==0x18)
			{
				SendSPIData(0x01);
			}
			else
			{
				SendSPIData(0);
			}
			SendSPIData(SegA[data2]);
			SendSPIData(0);

			SendSPIData(SegA[data3]);
			SendSPIData(0);

			SendSPIData(SegA[data4]);
			SendSPIData(0);

			SendSPIData(SegA[data5]);
			SendSPIData(0);

#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData(((0xf0&slave_sta)>>4)|0x80); //注:点亮从机状态and点亮冒号
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//注:点亮从机状态
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData(((0xff00&slave_sta)>>8) | 0x80); //注:点亮从机状态	9-15路
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//注:点亮从机状态 1-8路
			SendSPIData(0);
#endif
		}
		break;
		case 0x02:  //第二个小数点亮
		{
			if(sel == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
				SendSPIData(0);
			}
			else if(sel == 0x08)
			{
				SendSPIData(SegA[data1]);
				SendSPIData(0x01);
			}
			else if(sel == 0x18)
			{
				SendSPIData(SegA[data1]|0x80);
				SendSPIData(0x01);
			}
			else
			{
				SendSPIData(SegA[data1]);
				SendSPIData(0);
			}

			SendSPIData(SegA[data2]|0x80);
			SendSPIData(0);

			SendSPIData(SegA[data3]);
			SendSPIData(0);

			SendSPIData(SegA[data4]);
			SendSPIData(0);

			SendSPIData(SegA[data5]);
			SendSPIData(0);

#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData((0xf0&slave_sta)>>4); //注:点亮从机状态  风扇
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//注:点亮从机状态	   灯
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //注:点亮从机状态	9-15路
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//注:点亮从机状态 1-8路
			SendSPIData(0);
#endif
		}
		break;
		case 0x03:  //data3小数点亮
		{
			if(sel == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
				SendSPIData(0);
			}
			else if(sel == 0x08)
			{
				SendSPIData(SegA[data1]);
				SendSPIData(0x01);
			}
			else if(sel == 0x18)
			{
				SendSPIData(SegA[data1]|0x80);
				SendSPIData(0x01);
			}
			else
			{
				SendSPIData(SegA[data1]);
				SendSPIData(0);
			}

			SendSPIData(SegA[data2]);
			SendSPIData(0);

			SendSPIData(SegA[data3]|0x80);
			SendSPIData(0);

			SendSPIData(SegA[data4]);
			SendSPIData(0);

			SendSPIData(SegA[data5]);
			SendSPIData(0);

#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData((0xf0&slave_sta)>>4); //注:点亮从机状态	风扇
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//注:点亮从机状态	 灯
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //注:点亮从机状态	9-15路
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//注:点亮从机状态 1-8路
			SendSPIData(0);
#endif
		}
		break;
		case 0x04:  //data4小数点亮
		{
			if(sel == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
				SendSPIData(0);
			}
			else if(sel == 0x08)
			{
				SendSPIData(SegA[data1]);
				SendSPIData(0x01);
			}
			else if(sel == 0x18)
			{
				SendSPIData(SegA[data1]|0x80);
				SendSPIData(0x01);
			}
			else
			{
				SendSPIData(SegA[data1]);
				SendSPIData(0);
			}

			SendSPIData(SegA[data2]);
			SendSPIData(0);

			SendSPIData(SegA[data3]);
			SendSPIData(0);

			SendSPIData(SegA[data4]|0x80);
			SendSPIData(0);

			SendSPIData(SegA[data5]);
			SendSPIData(0);
#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData((0xf0&slave_sta)>>4); //注:点亮从机状态	风扇
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//注:点亮从机状态	 灯
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //注:点亮从机状态	9-15路
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//注:点亮从机状态 1-8路
			SendSPIData(0);
#endif
		}
		break;
		default:
		{
			//n++;
		}
		break;
	}
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(Light_Dim);  //开显示;
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
}

#elif (VERSION == 0x51)

/****************************************************************************
 功能:		stled316s初始化
 描述: 	    对数码管的个数、亮度及led的亮度进行初始化
 参数:		Brightness
 返回值:		无
 备注:
 ******************************************************************************/
void InitSTLED316( u8 Brightness )   //初始化
{
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x30);   //个数设置的地址
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x30);//地址
	SendSPIData(0xE5);//个数设置
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();

	STB_L;
	Delay_STLED316S();
	SendSPIData(0x11);
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x11);//地址
	SendSPIData(Brightness);//亮度设置
	SendSPIData(Brightness);
	SendSPIData(Brightness);
	STB_H;
	Delay_STLED316S();

	STB_L;
	Delay_STLED316S();
	SendSPIData(0x18);
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x18);//地址
	SendSPIData(0x00);
	SendSPIData(0x00);
	SendSPIData(0x00);//亮度设置
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
}

static void led(u8 LED1_Data)
{
	STB_L;		 //sel表示哪个316
	Delay_STLED316S();
	SendSPIData(0x28);
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();

	STB_L;
	Delay_STLED316S();

	SendSPIData(0x28);
	Delay_STLED316S();
	SendSPIData(LED1_Data);
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();

	STB_L;
	Delay_STLED316S();
	SendSPIData(0xD);//Display On
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
}
/****************************************************************************
 功能:		写数码管
 描述: 	    控制数码的显示状态
 参数:		对应数码管每位数码管的段数据：data1,data2,data3,data4,data5,data6
 返回值:		无
 备注:
 ******************************************************************************/
void Write_8LED(u8 sel,u8 oneSecondsFlag,u8 data1,u8 data2,u8 data3,u8 data4,u8 data5,u8 data6)
{
	u8 n = 0;
#if (CURRENT_PRODUCT == 0x8632)
    u8 slave_sta = 0;
	slave_sta = convert(); //转换从机状态
#elif (CURRENT_PRODUCT == 0x8630)
    u16 slave_sta = 0;
    slave_sta = slaverOutLed;
#endif

	STB_L;//sel表示哪个316
	Delay_STLED316S();
	SendSPIData(0x00);
	Delay_STLED316S();
	STB_H;

	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x00);//地址
	//此处代码可修改
	switch(oneSecondsFlag)//onesecondflag表要点亮哪个小数点及是否点亮冒号
	{
		case 0x00:	 //小数点全不亮
		{
			if((sel&0x10) == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
			}
			else
			{
				SendSPIData(SegA[data1]);
			}

			if((sel&0x08) == 0x08)
			{
				SendSPIData(SegA[data2]|0x80);
			}
			else
			{
				SendSPIData(SegA[data2]);
			}

			SendSPIData(SegA[data3]);
			SendSPIData(SegA[data4]);
			SendSPIData(SegA[data5]);
#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData((slave_sta>>4)&0x7f);	  //注:点风扇状态
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //注:点亮从机状态	9-15路
#endif
		}break;
		case 0x01:	  //中间时钟冒号点亮
		{
			if((sel&0x10) == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
			}
			else
			{
				SendSPIData(SegA[data1]);
			}

			if((sel&0x08) == 0x08)
			{
				SendSPIData(SegA[data2]|0x80);
			}
			else
			{
				SendSPIData(SegA[data2]);
			}

			SendSPIData(SegA[data3]);
			SendSPIData(SegA[data4]);
			SendSPIData(SegA[data5]);
#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData((slave_sta>>4) | 0x80);	 //注:点风扇 同时点冒号
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData(((0xff00&slave_sta)>>8) | 0x80); //注:点亮从机状态	9-15路
#endif
		}break;
		case 0x02:	 //data1小数点亮
		{
			if((sel&0x10) == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
			}
			else
			{
				SendSPIData(SegA[data1]);
			}

			if((sel&0x08) == 0x08)
			{
				SendSPIData(SegA[data2]|0x80);
			}
			else
			{
				SendSPIData(SegA[data2]);
			}

			SendSPIData(SegA[data3]);
			SendSPIData(SegA[data4]);
			SendSPIData(SegA[data5]);
#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData(slave_sta>>4);		//注:点风扇状态
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //注:点亮从机状态	9-15路
#endif
		}break;
		case 0x03:		//data2小数点亮
		{
			if((sel&0x10) == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
			}
			else
			{
				SendSPIData(SegA[data1]);
			}

			if((sel&0x08) == 0x08)
			{
				SendSPIData(SegA[data2]|0x80);
			}
			else
			{
				SendSPIData(SegA[data2]);
			}

			SendSPIData(SegA[data3]);
			SendSPIData(SegA[data4]);
			SendSPIData(SegA[data5]);
#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData(slave_sta>>4);	 //注:点风扇状态
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //注:点亮从机状态	9-15路
#endif
		}break;
		case 0x04:	 //data4小数点亮
		{
			if((sel&0x10) == 0x10)
			{
				SendSPIData(SegA[data1]|0x80);
			}
			else
			{
				SendSPIData(SegA[data1]);
			}

			if((sel&0x08) == 0x08)
			{
				SendSPIData(SegA[data2]|0x80);
			}
			else
			{
				SendSPIData(SegA[data2]);
			}

			SendSPIData(SegA[data3]);
			SendSPIData(SegA[data4]|0x80);
			SendSPIData(SegA[data5]);
#if (CURRENT_PRODUCT == 0x8632)
			SendSPIData(slave_sta>>4);	 //注:点风扇状态
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //注:点亮从机状态	9-15路
#endif
		}break;
		default:
		{
			//n++;
		}
		break;
	}

	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x0d);				//发送显示命令
	Delay_STLED316S();
	STB_H;
	Delay_ms(10);
#if (CURRENT_PRODUCT == 0x8632)
	led(slave_sta);//注:点灯状态
#elif (CURRENT_PRODUCT == 0x8630)
	led(slave_sta & 0x00ff);//注:点从机状态，1-8路
#endif
}

#endif
