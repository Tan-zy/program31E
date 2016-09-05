#include "Display_Chip.h"
//�������ʾ������
//0 1 2 3 4 5 6 7 8 9 10  11 12	13 14 15 16	17 18 19 20	 21	22 23  24 25  26  27
//0 1 2 3 4 5 6 7 8 9  A  B  C  D  E  F  -  �� N  t   L  H  r  o   P  U	  o	   J
#if (CURRENT_PRODUCT == 0x8632)
	static u8 slaverOutLed; //ͳ�ƴӻ�״̬
#elif (CURRENT_PRODUCT == 0x8630)
	static u16 slaverOutLed; //ͳ�ƴӻ�״̬
#endif

const u8 SegA[] = { 0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
		0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71, 0x40, 0x00, 0x37, 0x78, 0x38, 0x76,
		0x50, 0x5c, 0x73, 0x3e, 0x5c, 0x0e };    //����

void SetSlaverLed(u16 slaverLed)
{
	slaverOutLed = slaverLed;
}

static void Delay_STLED316S(void)  //��ʱ
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
 ����:		�������ݴ���
 ����: 	    ��stled316s��������
 ����:		data
 ����ֵ:		��
 ��ע:
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
/*ת���ӻ�״̬λ*/
static u8 convert(void)
{
	u8 sl_status = 0; //ע:�ӻ�״ָ̬ʾ
	u8 temp = 0;
	u8 i = 0;

	for (i = 0; i < 8; i++)
	{
		temp = slaverOutLed >> i;

		if (i % 2 == 0) //ע:��Ӧ�Ƶ�����λ
		{
			if ((temp & 0x01) == 0x01)
			{
				sl_status |= (0x01 << (i / 2));
			}
		}
		else	//ע:��Ӧ�Ƶ�ż��λ
		{
			if ((temp & 0x01) == 0x01)
			{
				sl_status |= (0x01 << (i / 2 + 4));
			}
		}
	}

	return sl_status;
}

#if  (VERSION == 0x52)  //TME1668�ı�����
static u8 Light_Dim = 0;
/****************************************************************************
 ����:		stled316s��ʼ��
 ����: 	    ������ܵĸ��������ȼ�led�����Ƚ��г�ʼ��
 ����:		Brightness
 ����ֵ:		��
 ��ע:
 ******************************************************************************/
void InitSTLED316( u8 Brightness )   //��ʼ��
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
 ����:		д�����
 ����: 	    �����������ʾ״̬
 ����:		data1,data2,data3,data4,data5,data6
 ����ֵ:		��
 ��ע:
 ******************************************************************************/
void Write_8LED(u8 sel,u8 oneSecondsFlag,u8 data1,u8 data2,u8 data3,u8 data4,u8 data5, u8 data6)
{
	//u8 n = 0;
	
#if (CURRENT_PRODUCT == 0x8632)
    u8 slave_sta = 0;
	slave_sta = convert(); //ת���ӻ�״̬
#elif (CURRENT_PRODUCT == 0x8630)
    u16 slave_sta = 0;
    slave_sta = slaverOutLed;
#endif
    
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x03);//��ʾģʽ 7wei 10 duan
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();

	STB_L;
	Delay_STLED316S();
	SendSPIData(0x40);//д���ݵ���ʾ�Ĵ�������ַ�Զ���һ
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();//__nop();

	STB_L;
	SendSPIData(0xC0);//ס����ʼ��ַ00H
	Delay_STLED316S();
	//���������
	switch(oneSecondsFlag)//onesecondflag��Ҫ�����ĸ�С���㼰�Ƿ����ð��
	{
		//Ҫ�����ĸ�����ܵ�С����ֻ��Ѹ�λ��һ����
		case 0x00://С����ȫ����
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
			SendSPIData((0xf0&slave_sta)>>4); //ע:�����ӻ�״̬	����
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//ע:�����ӻ�״̬	   ��
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //ע:�����ӻ�״̬	9-15·
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//ע:�����ӻ�״̬ 1-8·
			SendSPIData(0);
#endif
		}
		break;
		case 0x01:  //�����м�ð��
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
			SendSPIData(((0xf0&slave_sta)>>4)|0x80); //ע:�����ӻ�״̬and����ð��
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//ע:�����ӻ�״̬
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData(((0xff00&slave_sta)>>8) | 0x80); //ע:�����ӻ�״̬	9-15·
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//ע:�����ӻ�״̬ 1-8·
			SendSPIData(0);
#endif
		}
		break;
		case 0x02:  //�ڶ���С������
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
			SendSPIData((0xf0&slave_sta)>>4); //ע:�����ӻ�״̬  ����
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//ע:�����ӻ�״̬	   ��
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //ע:�����ӻ�״̬	9-15·
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//ע:�����ӻ�״̬ 1-8·
			SendSPIData(0);
#endif
		}
		break;
		case 0x03:  //data3С������
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
			SendSPIData((0xf0&slave_sta)>>4); //ע:�����ӻ�״̬	����
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//ע:�����ӻ�״̬	 ��
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //ע:�����ӻ�״̬	9-15·
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//ע:�����ӻ�״̬ 1-8·
			SendSPIData(0);
#endif
		}
		break;
		case 0x04:  //data4С������
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
			SendSPIData((0xf0&slave_sta)>>4); //ע:�����ӻ�״̬	����
			SendSPIData(0);

			SendSPIData(0x0f&slave_sta);//ע:�����ӻ�״̬	 ��
			SendSPIData(0);
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //ע:�����ӻ�״̬	9-15·
			SendSPIData(0);

			SendSPIData(0x00ff&slave_sta);//ע:�����ӻ�״̬ 1-8·
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
	SendSPIData(Light_Dim);  //����ʾ;
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
}

#elif (VERSION == 0x51)

/****************************************************************************
 ����:		stled316s��ʼ��
 ����: 	    ������ܵĸ��������ȼ�led�����Ƚ��г�ʼ��
 ����:		Brightness
 ����ֵ:		��
 ��ע:
 ******************************************************************************/
void InitSTLED316( u8 Brightness )   //��ʼ��
{
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x30);   //�������õĵ�ַ
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x30);//��ַ
	SendSPIData(0xE5);//��������
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
	SendSPIData(0x11);//��ַ
	SendSPIData(Brightness);//��������
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
	SendSPIData(0x18);//��ַ
	SendSPIData(0x00);
	SendSPIData(0x00);
	SendSPIData(0x00);//��������
	Delay_STLED316S();
	STB_H;
	Delay_STLED316S();
}

static void led(u8 LED1_Data)
{
	STB_L;		 //sel��ʾ�ĸ�316
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
 ����:		д�����
 ����: 	    �����������ʾ״̬
 ����:		��Ӧ�����ÿλ����ܵĶ����ݣ�data1,data2,data3,data4,data5,data6
 ����ֵ:		��
 ��ע:
 ******************************************************************************/
void Write_8LED(u8 sel,u8 oneSecondsFlag,u8 data1,u8 data2,u8 data3,u8 data4,u8 data5,u8 data6)
{
	u8 n = 0;
#if (CURRENT_PRODUCT == 0x8632)
    u8 slave_sta = 0;
	slave_sta = convert(); //ת���ӻ�״̬
#elif (CURRENT_PRODUCT == 0x8630)
    u16 slave_sta = 0;
    slave_sta = slaverOutLed;
#endif

	STB_L;//sel��ʾ�ĸ�316
	Delay_STLED316S();
	SendSPIData(0x00);
	Delay_STLED316S();
	STB_H;

	Delay_STLED316S();
	STB_L;
	Delay_STLED316S();
	SendSPIData(0x00);//��ַ
	//�˴�������޸�
	switch(oneSecondsFlag)//onesecondflag��Ҫ�����ĸ�С���㼰�Ƿ����ð��
	{
		case 0x00:	 //С����ȫ����
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
			SendSPIData((slave_sta>>4)&0x7f);	  //ע:�����״̬
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //ע:�����ӻ�״̬	9-15·
#endif
		}break;
		case 0x01:	  //�м�ʱ��ð�ŵ���
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
			SendSPIData((slave_sta>>4) | 0x80);	 //ע:����� ͬʱ��ð��
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData(((0xff00&slave_sta)>>8) | 0x80); //ע:�����ӻ�״̬	9-15·
#endif
		}break;
		case 0x02:	 //data1С������
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
			SendSPIData(slave_sta>>4);		//ע:�����״̬
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //ע:�����ӻ�״̬	9-15·
#endif
		}break;
		case 0x03:		//data2С������
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
			SendSPIData(slave_sta>>4);	 //ע:�����״̬
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //ע:�����ӻ�״̬	9-15·
#endif
		}break;
		case 0x04:	 //data4С������
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
			SendSPIData(slave_sta>>4);	 //ע:�����״̬
#elif (CURRENT_PRODUCT == 0x8630)
			SendSPIData((0xff00&slave_sta)>>8); //ע:�����ӻ�״̬	9-15·
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
	SendSPIData(0x0d);				//������ʾ����
	Delay_STLED316S();
	STB_H;
	Delay_ms(10);
#if (CURRENT_PRODUCT == 0x8632)
	led(slave_sta);//ע:���״̬
#elif (CURRENT_PRODUCT == 0x8630)
	led(slave_sta & 0x00ff);//ע:��ӻ�״̬��1-8·
#endif
}

#endif
