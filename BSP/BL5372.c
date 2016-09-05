#include "BL5372.h"
#include "RTCcalibration.h"
/*************************************************
 �ļ����ƣ�BL5372ʱ��оƬ
 ���ߣ�
 �汾�ţ�
 ʱ�䣺
 ������	 ���ģ��I2C���߶�eeprom�Ļ���������
 ������д���ֽ����ݺͶ��ֽ�����
 ***************************************************/
u8 Temp_bai, Temp_shi, Temp_ge;

//u16 Temp_ad;   // �ڲ�ad�ɼ��ĵ�ѹֵ ���¶�ֵ
u16 Temp_inner = 250;//25.0���϶�
u8 Temp_fault; //ADоƬ�¶ȹ���

s8 PPM_temp;	  //��ǰppmֵ
u8 Temp_Actual;   //��ǰ�¶�
s8 PPM1,PPM2,PPM_Difference;   //ʵ��PPMֵ������ppmֵ��ʵ��ppmֵ������ppmֵ�Ĳ�²�
u16 Frequency; //,Frequency1		  ,Temp_Difference

TIME_S timestruct;
u8 const table_week[12] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 }; //���������ݱ�

extern volatile u16 AD_Value[3];
extern s8  Temp_Difference;   //�²�
u16 ADCFault;
u16 ADC_Count;
u8 rectify;

/*****************************************************************
*Function in this file.
*/
static void GetTprt_Frm_ADC(void);
static void Inside_Temp(u16 Temp_inner);//koing2010@2015/5/13
static u8 BCDtoHEX(u8 data);
static u8 HEXtoBCD(u8 data);



u8 Get_Week(u8 year, u8 month, u8 day)
{
    u8 temp = 0xff;
    u8 yearL = 0;

    yearL = year + 100;

    temp = yearL + yearL / 4; //��������
    temp = temp % 7;
    temp = temp + day + table_week[month - 1];
    if (yearL % 4 == 0 && month < 3)
    {
        temp--;
    }
    return (temp % 7);
}
static void I2C_BL5372_delay(void)
{
    u8 i = 60; //60*4 ��ʱ������
    while (i)
    {
        i--;
    }
}

static bool I2C_BL5372_Start(void)
{
    SDA_H;
    SCL_H;
    I2C_BL5372_delay();
    if (!SDA_read )
    {
        return FALSE;	//SDA��Ϊ�͵�ƽ������æ,�˳�
    }
    SDA_L;
    I2C_BL5372_delay();
    if (SDA_read)
    {
        return FALSE;	//SDA��Ϊ�ߵ�ƽ�����߳���,�˳�
    }
    SDA_L;
    I2C_BL5372_delay();
    return TRUE;
}

static void I2C_BL5372_Stop(void)
{
    SCL_L;
    I2C_BL5372_delay();
    SDA_L;
    I2C_BL5372_delay();
    SCL_H;
    I2C_BL5372_delay();
    SDA_H;
    I2C_BL5372_delay();
}

static void I2C_BL5372_Ack(void)
{
    SCL_L;
    I2C_BL5372_delay();
    SDA_L;
    I2C_BL5372_delay();
    SCL_H;
    I2C_BL5372_delay();
    SCL_L;
    I2C_BL5372_delay();
}

static void I2C_BL5372_NoAck(void)
{
    SCL_L;
    I2C_BL5372_delay();
    SDA_H;
    I2C_BL5372_delay();
    SCL_H;
    I2C_BL5372_delay();
    SCL_L;
    I2C_BL5372_delay();
}

static bool I2C_BL5372_WaitAck(void) 	 //����Ϊ:=1��ACK,=0��ACK
{
    SCL_L;
    I2C_BL5372_delay();
    SDA_H;
    I2C_BL5372_delay();
    SCL_H;
    I2C_BL5372_delay();
    if (SDA_read) //ע:����߾�û��Ӧ��
    {
        SCL_L;
        return FALSE;
    }
    SCL_L;
    return TRUE;
}

static void I2C_BL5372_SendByte(u8 SendByte) //���ݴӸ�λ����λ//
{
    u8 i = 8;
    while (i)
    {
        i--;
        SCL_L;  //����ʱ�ӿ�ʼ���ݴ���
        I2C_BL5372_delay();
        if (SendByte & 0x80)
        {
            SDA_H;
        }
        else
        {
            SDA_L;
        }
        SendByte <<= 1;

        I2C_BL5372_delay();
        SCL_H;
        I2C_BL5372_delay();
        SCL_L;
    }
    SCL_L;
}

static u8 I2C_BL5372_ReceiveByte(void)  //���ݴӸ�λ����λ//
{
    u8 a = 8;
    u8 ReceiveByte = 0;
    SDA_H;
    for (a = 8; a > 0; a--)
    {
        ReceiveByte <<= 1;
        SCL_H;
        I2C_BL5372_delay();
        if (SDA_read)
        {
            ReceiveByte |= 0x01;
        }
        else
        {
            ReceiveByte &= 0xFE;
        }
        SCL_L;
        I2C_BL5372_delay();
    }
    return ReceiveByte;
}

//д��1�ֽ�����       ��д������    ��д���ַ
bool I2C_BL5372_WriteByte(u8 SendByte, u16 WriteAddress)
{
    u8 DeviceAddress = 0x64;	//��������(24c16��SD2403)
    bool ii_flag = FALSE;

    ii_flag = I2C_BL5372_Start();
    if (!ii_flag)
    {
        return FALSE;
    }
    //ע:0000 0000 0000 1110
    I2C_BL5372_SendByte(
        ((WriteAddress & 0x0700) >> 7) | (DeviceAddress & 0xFFFE));//���ø���ʼ��ַ+������ַ
    ii_flag = FALSE;
    ii_flag = I2C_BL5372_WaitAck();
    if (!ii_flag)
    {
        I2C_BL5372_Stop();
        return FALSE;
    }

    I2C_BL5372_SendByte((u8) (WriteAddress & 0x00FF));   //���õ���ʼ��ַ
    ii_flag = FALSE;
    ii_flag = I2C_BL5372_WaitAck();
    if (!ii_flag)
    {
        I2C_BL5372_Stop();
        return FALSE;
    }

    I2C_BL5372_SendByte(SendByte);
    ii_flag = FALSE;
    ii_flag = I2C_BL5372_WaitAck();
    if (!ii_flag)
    {
        I2C_BL5372_Stop();
        return FALSE;
    }

    I2C_BL5372_Stop();
    //ע�⣺��Ϊ����Ҫ�ȴ�EEPROMд�꣬���Բ��ò�ѯ����ʱ��ʽ(10ms)
    Delay_ms(10);
    return TRUE;
}
//����1byte����         ��������ַ

bool I2C_BL5372_ReadByte(u8 *readbyte, u16 ReadAddress)
{
    u8 DeviceAddress = 0x64;	//��������(24c16��SD2403)
    bool ii_flag = FALSE;

    ii_flag = I2C_BL5372_Start();
    if (!ii_flag)
    {
        return FALSE;
    }

    I2C_BL5372_SendByte(DeviceAddress & 0x00ff);	//������ַ
    ii_flag = FALSE;
    ii_flag = I2C_BL5372_WaitAck();
    if (!ii_flag)
    {
        I2C_BL5372_Stop();
        return FALSE;
    }

    I2C_BL5372_SendByte((u8) (ReadAddress | 0x0004));   //������ʼ��ַ+����ģʽ
    ii_flag = FALSE;
    ii_flag = I2C_BL5372_WaitAck();
    if (!ii_flag)
    {
        I2C_BL5372_Stop();
        return FALSE;
    }

    *readbyte = I2C_BL5372_ReceiveByte();
    //I2C_BL5372_Ack(); //ע:֮ǰΪI2C_BL5372_NoAck();
    I2C_BL5372_NoAck();
    I2C_BL5372_Stop();
    return TRUE;
}

bool Init_BL5372(PTIME_S time)
{

    //���ϵ羧����ȴ�BL5372�ȶ����ʼ��
    extern SL863X_CONTROLLER sl863x;
    u8 test = 0;
    I2C_BL5372_ReadByte(&test, 0xf0);
    F_Difference(327000 + sl863x.sl863xStorage.frequency);
    if ((test & 0x10) == 0x10) //��⾧���Ƿ�ͣ��
    {



        if ( ! I2C_BL5372_WriteByte( HEXtoBCD(time->second), 0x00))  //se
        {
            return FALSE;
        }

        if ( ! I2C_BL5372_WriteByte( HEXtoBCD(time->minute), 0x10))  //min
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte( HEXtoBCD(time->hour), 0x20)) 	//hour
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte( HEXtoBCD(time->day), 0x40))  //day
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte( HEXtoBCD(time->month), 0x50)) //month
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte( HEXtoBCD(time->year), 0x60)) //year
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte(0X3f, 0x70))
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte(0X00, 0x80))
        {
            return FALSE;
        }


        if ( ! I2C_BL5372_WriteByte(0X00, 0x90))
        {
            return FALSE;
        }


        if ( ! I2C_BL5372_WriteByte(0X00, 0xA0))
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte(0X00, 0xB0))
        {
            return FALSE;
        }


        if ( ! I2C_BL5372_WriteByte(0X00, 0xC0))
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte(0X00, 0xD0))
        {
            return FALSE;
        }


        if ( ! I2C_BL5372_WriteByte(0X00, 0xE0))
        {
            return FALSE;
        }



        if ( ! I2C_BL5372_WriteByte(0X20, 0xF0))
        {
            return FALSE;
        }
    }

    return TRUE;
}
/*CANЭ����ʱ����������BCD�룬�����ú�������CANͨ���
 ��֮ǰ�Ȱ�ͨ��CAN���ܵ���bcd�븶���ṹ�������Ӧ������
 BL5372����ʱ��ֻ����BCD��*/
bool BL5372_SetBCD(PTIME_S timeBCD)
{
    bool Wr_flag = FALSE;

    Wr_flag = I2C_BL5372_WriteByte(timeBCD->second, 0x00);	//��
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->minute, 0x10);	//��
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->hour, 0x20);	//ʱ
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->day, 0x40);	//��
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->month, 0x50);	//��
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->year, 0x60);	//��
    if (!Wr_flag)
    {
        return FALSE;
    }

    return TRUE; //ע:�����ִ�е����
}

bool BL5372_Read(PTIME_S time)
{

    u8 temp_val = 0xff;

    extern SL863X_CONTROLLER sl863x; 
    static u8 calibration_last,calibration_current;

    GetTprt_Frm_ADC(); //ѭ���ɼ��¶�
   
    calibration_current = RTCcalibration(sl863x.sl863xStorage.frequency_0,Temp_inner);
    if(calibration_current != calibration_last)
    {
        I2C_BL5372_WriteByte(calibration_current, 0x70);
        calibration_last = calibration_current;
    }

    if (!I2C_BL5372_ReadByte(&temp_val, 0x00))  //��
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( temp_val >= 60)//�����60������һ�ε���ȷ��ֵ����д��
    {
        Init_BL5372(time);//����ȷֵ���³�ʼ��BL5372
    }
    else
    {
        time->second = temp_val;
    }




    if (!I2C_BL5372_ReadByte(&temp_val, 0x10))//��
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( temp_val >= 60)//���Ӵ���60������һ�ε���ȷ��ֵ����д��
    {
        Init_BL5372(time);//����ȷֵ���³�ʼ��BL5372
    }
    else
    {
        time->minute = temp_val;
    }




    if (!I2C_BL5372_ReadByte(&temp_val, 0x20))//ʱ
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( temp_val >= 24) //Сʱ����24������һ�ε���ȷ��ֵ����д��
    {
        Init_BL5372(time);//����ȷֵ���³�ʼ��BL5372
    }
    else
    {
        time->hour = temp_val;
    }


    if (!I2C_BL5372_ReadByte(&temp_val, 0x40))//��
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( temp_val >31 || (temp_val == 0) ) //����31��
    {
        Init_BL5372(time);//����ȷֵ���³�ʼ��BL5372
    }
    else
    {
        time->day = temp_val;
    }




    if (!I2C_BL5372_ReadByte(&temp_val, 0x50))
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( (temp_val > 12)  || (temp_val == 0) )//����12��
    {
        Init_BL5372(time);//����ȷֵ���³�ʼ��BL5372
    }
    else
    {
        time->month = temp_val;
    }




    if (!I2C_BL5372_ReadByte(&temp_val, 0x60))//��
    {
        return FALSE;
    }
    time->year = temp_val % 16 + (temp_val / 16) * 10;
    time->week = Get_Week(time->year, time->month, time->day);




    return TRUE;
}


/*****************************
BCD�뵽	hex ת��  ����λ��
******************************/
static u8 BCDtoHEX(u8 data)
{
    u8  target = 0;
    target = data % 16 + (data / 16) * 10;
    return target;
}

/******************************
hex�뵽	BC ת��  ����λ��
******************************/
static u8 HEXtoBCD(u8 data)
{
    u8  target = 0;
    target = ( data %10 ) + (( data / 10 )<< 4);
    return target;
}


/*��������bool BL5372_Read_BCD(PTIME_S time)
��ڲ�����ָ��ʱ��ṹ�������ָ��
���ܣ���BCD�����ʽ���ض�����ʱ�䣬
�����롢�֡�ʱ���ա��¡��꣬�ú�������ʱ�뷵��HEX���ݵĶ���������ʹ�ã�
���Զ�����ʱ��ṹ�������һ��������16�������ݣ�һ������BCD�������*/
bool BL5372_ReadBCD(PTIME_S timeBCD)
{


    bool Re_flag = FALSE;

    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->second), 0x00);   //��
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->minute), 0x10);  //��
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->hour), 0x20);	//ʱ
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->day), 0x40);	//��
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->month), 0x50);	//��
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->year), 0x60);	//��
    if (!Re_flag)
    {
        return FALSE;
    }

    //time->Week = Get_Week(time->Year,time->Month,time->Day);

    return TRUE;
}





/*********************************************************************
��ADCȡֵ���㵱ǰ�¶�Get current temperature from ADC.
u16 Temp_inner	��ǰʵ���¶ȵ�ʮ��

*********************************************************************/
static void GetTprt_Frm_ADC(void)
{
	u16 Temp_ad;
	float Temp_ADData, Temp_ADData_Dis;
	Temp_ad = AD_Value[0];						 //����ѹֵ
	Temp_ad = Temp_ad >> 2;								  //ȡ��ʮλ

	if (Temp_ad > 678)
	{
		Temp_ADData = (float) (0.1212 * Temp_ad);
		Temp_ADData_Dis = (Temp_ADData - 42.1682) * 10;
		Temp_inner = (u16) (Temp_ADData_Dis);
	}
	else
	{
		Temp_ADData = (float) (0.0926 * Temp_ad);
		Temp_ADData_Dis = (Temp_ADData - 23.231) * 10;
		Temp_inner = (u16) (Temp_ADData_Dis);
	}
}



//���ݵ�ǰ�¶ȼ���PPMֵ

static void Inside_Temp(u16 Temp_inner)
{
    static u8 Temp_old = 25;
    if ((Temp_inner / 10) != Temp_old)					//�¶ȱȽ�
    {
        Temp_old = Temp_inner / 10;					//
        if (Temp_old <= 0 || Temp_old > 50)
        {
            Temp_old = 25;
            Temp_fault = 1;					//оƬ�¶ȹ���
            return ;
        }
        else
        {
            Temp_fault = 0;
        }
        PPM_temp = (s8) (PPM_Difference - (pow(Temp_old - 25, 2) * 0.032));
        if (PPM_temp >= 0)
        {
            rectify = (u8) (PPM_temp / 3.05);
            I2C_BL5372_WriteByte(rectify, 0x70);
        }
        else
        {
            rectify = (u8) ((0 - PPM_temp) / 3.05);
            rectify = (0x80 - rectify) | 0x40;		  //ȡ7λ�����Ʋ��룬����b6��λ
            I2C_BL5372_WriteByte(rectify, 0x70);
        }
    }
}


//����ʵ��Ƶ��ֵ�����㵱ǰ�������׼����Ĳ���*/
void F_Difference(u32 frequency) //���뵱ǰʵ��Ƶ��ֵ
{
    PPM1 = (u8) (0.032 * (pow(((Temp_inner / 10) - 25), 2))); 	//���㵱ǰ�¶������۵�PPMֵ
    PPM2 = (s8) (((frequency - 327680) / 327680.0) * pow(10, 6)); //���������Ƶ��ֵ�����ʵ��PPmֵ
    PPM_Difference = PPM2 - PPM1;	 //���ϵ��=ʵ��ֵ-����ֵ
}

