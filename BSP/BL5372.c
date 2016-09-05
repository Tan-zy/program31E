#include "BL5372.h"
#include "RTCcalibration.h"
/*************************************************
 文件名称：BL5372时钟芯片
 作者：
 版本号：
 时间：
 描述：	 完成模拟I2C总线对eeprom的基本操作，
 包括读写单字节数据和多字节数据
 ***************************************************/
u8 Temp_bai, Temp_shi, Temp_ge;

//u16 Temp_ad;   // 内部ad采集的电压值 ，温度值
u16 Temp_inner = 250;//25.0摄氏度
u8 Temp_fault; //AD芯片温度故障

s8 PPM_temp;	  //当前ppm值
u8 Temp_Actual;   //当前温度
s8 PPM1,PPM2,PPM_Difference;   //实际PPM值、理论ppm值、实际ppm值与理论ppm值的差、温差
u16 Frequency; //,Frequency1		  ,Temp_Difference

TIME_S timestruct;
u8 const table_week[12] = { 0, 3, 3, 6, 1, 4, 6, 2, 5, 0, 3, 5 }; //月修正数据表

extern volatile u16 AD_Value[3];
extern s8  Temp_Difference;   //温差
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

    temp = yearL + yearL / 4; //算闰年数
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
    u8 i = 60; //60*4 个时钟周期
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
        return FALSE;	//SDA线为低电平则总线忙,退出
    }
    SDA_L;
    I2C_BL5372_delay();
    if (SDA_read)
    {
        return FALSE;	//SDA线为高电平则总线出错,退出
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

static bool I2C_BL5372_WaitAck(void) 	 //返回为:=1有ACK,=0无ACK
{
    SCL_L;
    I2C_BL5372_delay();
    SDA_H;
    I2C_BL5372_delay();
    SCL_H;
    I2C_BL5372_delay();
    if (SDA_read) //注:如果高就没有应答
    {
        SCL_L;
        return FALSE;
    }
    SCL_L;
    return TRUE;
}

static void I2C_BL5372_SendByte(u8 SendByte) //数据从高位到低位//
{
    u8 i = 8;
    while (i)
    {
        i--;
        SCL_L;  //拉低时钟开始数据传输
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

static u8 I2C_BL5372_ReceiveByte(void)  //数据从高位到低位//
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

//写入1字节数据       待写入数据    待写入地址
bool I2C_BL5372_WriteByte(u8 SendByte, u16 WriteAddress)
{
    u8 DeviceAddress = 0x64;	//器件类型(24c16或SD2403)
    bool ii_flag = FALSE;

    ii_flag = I2C_BL5372_Start();
    if (!ii_flag)
    {
        return FALSE;
    }
    //注:0000 0000 0000 1110
    I2C_BL5372_SendByte(
        ((WriteAddress & 0x0700) >> 7) | (DeviceAddress & 0xFFFE));//设置高起始地址+器件地址
    ii_flag = FALSE;
    ii_flag = I2C_BL5372_WaitAck();
    if (!ii_flag)
    {
        I2C_BL5372_Stop();
        return FALSE;
    }

    I2C_BL5372_SendByte((u8) (WriteAddress & 0x00FF));   //设置低起始地址
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
    //注意：因为这里要等待EEPROM写完，可以采用查询或延时方式(10ms)
    Delay_ms(10);
    return TRUE;
}
//读出1byte数据         待读出地址

bool I2C_BL5372_ReadByte(u8 *readbyte, u16 ReadAddress)
{
    u8 DeviceAddress = 0x64;	//器件类型(24c16或SD2403)
    bool ii_flag = FALSE;

    ii_flag = I2C_BL5372_Start();
    if (!ii_flag)
    {
        return FALSE;
    }

    I2C_BL5372_SendByte(DeviceAddress & 0x00ff);	//器件地址
    ii_flag = FALSE;
    ii_flag = I2C_BL5372_WaitAck();
    if (!ii_flag)
    {
        I2C_BL5372_Stop();
        return FALSE;
    }

    I2C_BL5372_SendByte((u8) (ReadAddress | 0x0004));   //设置起始地址+传输模式
    ii_flag = FALSE;
    ii_flag = I2C_BL5372_WaitAck();
    if (!ii_flag)
    {
        I2C_BL5372_Stop();
        return FALSE;
    }

    *readbyte = I2C_BL5372_ReceiveByte();
    //I2C_BL5372_Ack(); //注:之前为I2C_BL5372_NoAck();
    I2C_BL5372_NoAck();
    I2C_BL5372_Stop();
    return TRUE;
}

bool Init_BL5372(PTIME_S time)
{

    //刚上电晶振需等待BL5372稳定后初始化
    extern SL863X_CONTROLLER sl863x;
    u8 test = 0;
    I2C_BL5372_ReadByte(&test, 0xf0);
    F_Difference(327000 + sl863x.sl863xStorage.frequency);
    if ((test & 0x10) == 0x10) //检测晶振是否停震
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
/*CAN协议里时间设置是用BCD码，该设置函数用在CAN通信里，
 用之前先把通过CAN接受到的bcd码付给结构体里的相应变量，
 BL5372设置时间只能用BCD码*/
bool BL5372_SetBCD(PTIME_S timeBCD)
{
    bool Wr_flag = FALSE;

    Wr_flag = I2C_BL5372_WriteByte(timeBCD->second, 0x00);	//秒
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->minute, 0x10);	//分
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->hour, 0x20);	//时
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->day, 0x40);	//日
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->month, 0x50);	//月
    if (!Wr_flag)
    {
        return FALSE;
    }

    Wr_flag = FALSE;
    Wr_flag = I2C_BL5372_WriteByte(timeBCD->year, 0x60);	//年
    if (!Wr_flag)
    {
        return FALSE;
    }

    return TRUE; //注:如果能执行到最后
}

bool BL5372_Read(PTIME_S time)
{

    u8 temp_val = 0xff;

    extern SL863X_CONTROLLER sl863x; 
    static u8 calibration_last,calibration_current;

    GetTprt_Frm_ADC(); //循环采集温度
   
    calibration_current = RTCcalibration(sl863x.sl863xStorage.frequency_0,Temp_inner);
    if(calibration_current != calibration_last)
    {
        I2C_BL5372_WriteByte(calibration_current, 0x70);
        calibration_last = calibration_current;
    }

    if (!I2C_BL5372_ReadByte(&temp_val, 0x00))  //秒
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( temp_val >= 60)//秒大于60，将上一次的正确的值重新写入
    {
        Init_BL5372(time);//按正确值重新初始化BL5372
    }
    else
    {
        time->second = temp_val;
    }




    if (!I2C_BL5372_ReadByte(&temp_val, 0x10))//分
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( temp_val >= 60)//分钟大于60，将上一次的正确的值重新写入
    {
        Init_BL5372(time);//按正确值重新初始化BL5372
    }
    else
    {
        time->minute = temp_val;
    }




    if (!I2C_BL5372_ReadByte(&temp_val, 0x20))//时
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( temp_val >= 24) //小时大于24，将上一次的正确的值重新写入
    {
        Init_BL5372(time);//按正确值重新初始化BL5372
    }
    else
    {
        time->hour = temp_val;
    }


    if (!I2C_BL5372_ReadByte(&temp_val, 0x40))//日
    {
        return FALSE;
    }
    temp_val = BCDtoHEX( temp_val );
    if( temp_val >31 || (temp_val == 0) ) //大于31天
    {
        Init_BL5372(time);//按正确值重新初始化BL5372
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
    if( (temp_val > 12)  || (temp_val == 0) )//大于12月
    {
        Init_BL5372(time);//按正确值重新初始化BL5372
    }
    else
    {
        time->month = temp_val;
    }




    if (!I2C_BL5372_ReadByte(&temp_val, 0x60))//年
    {
        return FALSE;
    }
    time->year = temp_val % 16 + (temp_val / 16) * 10;
    time->week = Get_Week(time->year, time->month, time->day);




    return TRUE;
}


/*****************************
BCD码到	hex 转换  （两位）
******************************/
static u8 BCDtoHEX(u8 data)
{
    u8  target = 0;
    target = data % 16 + (data / 16) * 10;
    return target;
}

/******************************
hex码到	BC 转换  （两位）
******************************/
static u8 HEXtoBCD(u8 data)
{
    u8  target = 0;
    target = ( data %10 ) + (( data / 10 )<< 4);
    return target;
}


/*函数名：bool BL5372_Read_BCD(PTIME_S time)
入口参数：指向时间结构体变量的指针
功能：以BCD码的形式返回读到的时间，
包括秒、分、时、日、月、年，该函数在用时与返回HEX数据的读函数搭配使用，
可以定义俩时间结构体变量，一个用来存16进制数据，一个来存BCD码的数据*/
bool BL5372_ReadBCD(PTIME_S timeBCD)
{


    bool Re_flag = FALSE;

    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->second), 0x00);   //秒
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->minute), 0x10);  //分
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->hour), 0x20);	//时
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->day), 0x40);	//日
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->month), 0x50);	//月
    if (!Re_flag)
    {
        return FALSE;
    }

    Re_flag = FALSE;
    Re_flag = I2C_BL5372_ReadByte(&(timeBCD->year), 0x60);	//年
    if (!Re_flag)
    {
        return FALSE;
    }

    //time->Week = Get_Week(time->Year,time->Month,time->Day);

    return TRUE;
}





/*********************************************************************
从ADC取值计算当前温度Get current temperature from ADC.
u16 Temp_inner	当前实际温度的十倍

*********************************************************************/
static void GetTprt_Frm_ADC(void)
{
	u16 Temp_ad;
	float Temp_ADData, Temp_ADData_Dis;
	Temp_ad = AD_Value[0];						 //读电压值
	Temp_ad = Temp_ad >> 2;								  //取高十位

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



//根据当前温度计算PPM值

static void Inside_Temp(u16 Temp_inner)
{
    static u8 Temp_old = 25;
    if ((Temp_inner / 10) != Temp_old)					//温度比较
    {
        Temp_old = Temp_inner / 10;					//
        if (Temp_old <= 0 || Temp_old > 50)
        {
            Temp_old = 25;
            Temp_fault = 1;					//芯片温度故障
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
            rectify = (0x80 - rectify) | 0x40;		  //取7位二进制补码，并对b6置位
            I2C_BL5372_WriteByte(rectify, 0x70);
        }
    }
}


//输入实测频率值，计算当前晶振与标准晶振的差异*/
void F_Difference(u32 frequency) //输入当前实测频率值
{
    PPM1 = (u8) (0.032 * (pow(((Temp_inner / 10) - 25), 2))); 	//计算当前温度下理论的PPM值
    PPM2 = (s8) (((frequency - 327680) / 327680.0) * pow(10, 6)); //根据输入的频率值计算得实际PPm值
    PPM_Difference = PPM2 - PPM1;	 //恒差系数=实际值-理论值
}

