/**********************************************************************************

File_name:      sd2068
Version:		    0.0
Revised:        $Date:2015-9-17 to   ; $
Description:    the drver of RTC  component.
Notes:          This version targets the stm32
Editor:		      Mr.Kong

**********************************************************************************/

/*================================================================================
@ Include files
*/
#include "sd2068.h"
#include "RTCcalibration.h"
#ifdef    RTC_SD2068
#define		RTC_Address			0x64		//RTC器件地址

/*================================================================================
@ Global variable
*/


/*================================================================================
@ All functions  as follow
*/
u8   I2CReadOneByte(u8 DeviceAddress,u8 add);//write
bool I2CWriteOneByte(u8 DeviceAddress,u8 add, u8 date);//写一字节
bool WriteTimeOn(void);//write enable
bool WriteTimeOff(void);//write disable
void Delay(u16 nn);
static void GetTprt_Frm_ADC(u16 AD_value);
static void Inside_Temp(u16 Temp_inner);
static bool SD2068_I2CWriteDate(PTIME_S	psRTC);



/*********************************************************************************
Function:     I2CWait
Description:  realize four seconds delay.
Input:        None
Output:       None
Return:       don't care.
Others:
*********************************************************************************/
void I2CWait(void)//4us
{
    u8 i = 72;
    while(i--);
}

/*********************************************************************************
Function:     I2CStart (locally)
Description:  start IIC bus transmission.
Input:        None
Output:       None
Return:       true or false :bus start sucssesfully or not.
Others:
*********************************************************************************/
static bool I2CStart(void)
{
    SDA_H ;
    SCL_H;
    I2CWait();
    if(!SDA_read)return FALSE;	//SDA线为低电平则总线忙,退出
    SDA_L;
    I2CWait();
    while(SDA_read)return FALSE;	//SDA线为高电平则总线出错,退出
    SCL_L;
    I2CWait();
    return TRUE;
}

/*********************************************************************************
Function:     I2CStop(locally)
Description:  close SD2068 IIC bus.
Input:        None
Output:       None
Return:       don't care.
Others:
*********************************************************************************/
static void I2CStop(void)
{
    SDA_L;
    SCL_L;
    I2CWait();
    SCL_H;
    I2CWait();
    SDA_H ;
}

/*********************************************************************************
Function:     I2CAck
Description:  transmit ack message.
Input:        None
Output:       None
Return:       don't care.
Others:
*********************************************************************************/
static void I2CAck(void)
{
    SDA_L;
    SCL_L;
    I2CWait();
    SCL_H;
    I2CWait();
    SCL_L;
}

/*********************************************************************************
Function:     I2CNoAck
Description:  send no ack to device.
Input:        None
Output:       None
Return:       don't care.
Others:
*********************************************************************************/
static void I2CNoAck(void)
{
    SDA_H ;
    SCL_L;
    I2CWait();
    SCL_H;
    I2CWait();
    SCL_L;
}

/*********************************************************************************
Function:     I2CWaitAck
Description:  read ack signal.
Input:        None
Output:       None
Return:       true : there has response. false : without.
Others:
*********************************************************************************/
static bool I2CWaitAck(void)
{
    SCL_L;
    SDA_H ;
    I2CWait();
    SCL_H;
    I2CWait();
    while(SDA_read)
    {
        SCL_L;
        return FALSE;
    }
    SCL_L;
    return TRUE;
}

/*********************************************************************************
Function:     I2CSendByte
Description:  mcu send one byte data to device in MSB form.
Input:        None
Output:       None
Return:       don't care.
Others:
*********************************************************************************/
static void I2CSendByte(u8 demand)
{
    u8 i=8;
    while(i--)
    {
        SCL_L;
        if(demand&0x80)
        {
            SDA_H ;
        }
        else
        {
            SDA_L;
        }
        demand <<= 1;
        I2CWait();
        SCL_H;
        I2CWait();
    }
    SCL_L;
}

/*********************************************************************************
Function:     I2CReceiveByte
Description:  get one byte data frome device,it is also in MSB form.
Input:        None
Output:       None
Return:       data: which read from device.
Others:
*********************************************************************************/
static u8 I2CReceiveByte(void)
{
    u8 i=8;
    u8 ddata=0;
    SDA_H ;			//设置SDA为输入（其它类型的单片机需要配置IO输入输出寄存器）
    while(i--)
    {
        ddata<<=1;      //数据从高位开始读取
        SCL_L;
        I2CWait();
        SCL_H;
        I2CWait();	//MSB
        if(SDA_read)
        {
            ddata|=0x01;
        }
    }
    SCL_L;
    return ddata;
}
/*********************************************************************************
Function:     I2CWriteOneByte
Description:  write one byte data to target device.
Input:        None
Output:       None
Return:       true: write sucssesfully , false: write failed.
Others:
*********************************************************************************/
bool I2CWriteOneByte(u8 DeviceAddress,u8 add, u8 date)
{
    if(!I2CStart())
        return FALSE;
    I2CSendByte(DeviceAddress);
    I2CWaitAck();
    I2CSendByte(add);		//write reg address
    I2CWaitAck();
    I2CSendByte(date);		//write target data
    I2CWaitAck();
    I2CStop();
    return	TRUE;
}
/*********************************************************************************
Function:     I2CReadOneByte
Description:  First function called after startup.
Input:        None
Output:       None
Return:       if muc read one byte from ter target sucssesfully,it will return the
               data. if not ,return false (0);
Others:
*********************************************************************************/
u8 I2CReadOneByte(u8 DeviceAddress,u8 add)
{
    u8 dat;
    if(!I2CStart())
        return FALSE;
    I2CSendByte(DeviceAddress);
    if(!I2CWaitAck())
    {
        I2CStop();
        return FALSE;
    }
    I2CSendByte( add );		//send read reg address
    I2CWaitAck();
    I2CStart();
    I2CSendByte(DeviceAddress+1);
    I2CWaitAck();
    dat = I2CReceiveByte(); // get variable data
    I2CNoAck();
    I2CStop();
    return  dat;
}
/*********************************************************************************
Function:    WriteTimeOn
Description:  unlock SD2068 writing.
Input:        None
Output:       None
Return:       true or false
Others:
*********************************************************************************/
bool WriteTimeOn(void)
{
    if(!I2CWriteOneByte(RTC_Address,0x10,0x80))
        return FALSE;
    I2CWriteOneByte(RTC_Address,0x0f,0x84);
    return	TRUE;
}

/*********************************************************************************
Function:    WriteTimeOff
Description:  lock SD2068 writing.
Input:        None
Output:       None
Return:       true or false
Others:
*********************************************************************************/
bool WriteTimeOff(void)
{
    if(!I2CWriteOneByte(RTC_Address,0x0f,0))
        return FALSE;
    I2CWriteOneByte(RTC_Address,0x10,0);
    return	TRUE;
}

/*********************************************************************************
Function:     BCDtoHEX
Description:  convert BCD code to hex code.
Input:        BCD data
Output:       hex data
Return:       don't care.
Others:
*********************************************************************************/
static u8 BCDtoHEX(u8 data)
{
    u8 target = 0;
    target = data % 16 + (data / 16) * 10;
    return target;
}

/*********************************************************************************
Function:     HEXtoBCD
Description:  convert hex code to BCD code.
Input:        hex data
Output:       BCD data
Return:       don't care.
Others:
*********************************************************************************/
static u8 HEXtoBCD(u8 data)
{
    u8 target = 0;
    target = ((u8)(data % 10) & 0x0F) | ((u8)((data / 10) << 4 ) & 0xF0);
    return target;
}

/*********************************************************************************
Function:     SD2068_I2CReadDate
Description:  read the SD2068 realy time data.
Input:        PTIME_S: realy time structure.
Output:       None
Return:       sucssesfully or failed.
Others:
*********************************************************************************/
extern u16  Temp_inner;
bool SD2068_I2CReadDate(PTIME_S	time)
{
    u8 tempdata = 0;
    static u8 calibration_last,calibration_current;
    TIME_S sRTC;
    if(!I2CStart())return FALSE;
    I2CSendByte(RTC_Address+1);
    if(!I2CWaitAck())
    {
        I2CStop();
        return FALSE;
    }

    sRTC.second = BCDtoHEX(I2CReceiveByte());
    I2CAck();
    sRTC.minute = BCDtoHEX(I2CReceiveByte());
    I2CAck();
    sRTC.hour  = BCDtoHEX(I2CReceiveByte() & 0x7F);//bit7   12/24
    I2CAck();
    sRTC.week  = BCDtoHEX(I2CReceiveByte());
    I2CAck();
    sRTC.day   = BCDtoHEX(I2CReceiveByte());
    I2CAck();
    sRTC.month = BCDtoHEX(I2CReceiveByte());
    I2CAck();
    sRTC.year  = BCDtoHEX(I2CReceiveByte());
    I2CNoAck();		//读时间完成，发送NoAck
    I2CStop();

    if( sRTC.second > 60 || sRTC.minute > 60)
    {
        SD2068_I2CWriteDate(time);
        return FALSE;
    }
    if(sRTC.hour > 24)
    {
        SD2068_I2CWriteDate(time);
        return FALSE;
    }
    if(sRTC.week > 6)
    {
        SD2068_I2CWriteDate(time);
        return FALSE;
    }
    if(sRTC.day >31 || sRTC.day == 0 )
    {
        SD2068_I2CWriteDate(time);
        return FALSE;
    }
    if(sRTC.month > 12 || sRTC.month == 0)
    {
        SD2068_I2CWriteDate(time);
        return FALSE;
    }
    time->second = sRTC.second ;
    time->minute = sRTC.minute;
    time->hour   = sRTC.hour ;
    time->week   = sRTC.week ;
    time->day    = sRTC.day;
    time->month  = sRTC.month;
    time->year   = sRTC.year;

    extern volatile u16 AD_Value[3];
    extern SL863X_CONTROLLER sl863x;

    GetTprt_Frm_ADC( AD_Value[0] );
    // Inside_Temp( Temp_inner );
    calibration_current = RTCcalibration(sl863x.sl863xStorage.frequency_0,Temp_inner);
    if(calibration_current != calibration_last)
    {
        SD2068_I2CWriteSingleReg( calibration_current,0x12);
        calibration_last = calibration_current;
    }
    return	TRUE;
}

/*********************************************************************************
Function:     SD2068_I2CWriteDate
Description:  init the RTC component, adjusted by hex code.
Input:        PTIME_S: realy time structure point
Output:       None
Return:       don't care.
Others:
*********************************************************************************/
static bool SD2068_I2CWriteDate(PTIME_S	psRTC)	//写时间操作要求一次对实时时间寄存器(00H~06H)依次写入，
{   //不可以单独对7个时间数据中的某一位进行写操作,否则可能会引起时间数据的错误进位.
    //要修改其中某一个数据 , 应一次性写入全部 7 个实时时钟数据.
    WriteTimeOn();				//unlock
    if(!I2CStart())return FALSE;
    I2CSendByte(RTC_Address);
    if(!I2CWaitAck())
    {
        I2CStop();
        return FALSE;
    }
    I2CSendByte(0x00);			//write start address
    I2CWaitAck();
    I2CSendByte(HEXtoBCD(psRTC -> second));		//second
    I2CWaitAck();
    I2CSendByte(HEXtoBCD(psRTC->minute));		//minute
    I2CWaitAck();
    I2CSendByte(HEXtoBCD(psRTC->hour)|0x80);		//hour ,同时设置小时寄存器最高位（0：为12小时制，1：为24小时制）
    I2CWaitAck();
    I2CSendByte(HEXtoBCD(psRTC->week));		//week
    I2CWaitAck();
    I2CSendByte(HEXtoBCD(psRTC->day));		//day
    I2CWaitAck();
    I2CSendByte(HEXtoBCD(psRTC->month));		//month
    I2CWaitAck();
    I2CSendByte(HEXtoBCD(psRTC->year));		//year
    I2CWaitAck();
    I2CStop();

    I2CWait();
    I2CWait();
    I2CWait();// delay 12us

    I2CWriteOneByte(RTC_Address,0x10,0xA1);
    I2CWriteOneByte(RTC_Address,0x11,0x01);
    WriteTimeOff();				//lock,write disable.
    return	TRUE;
}
/*********************************************************************************
Function:     Init_SD2068
Description:  check that SD2068 has lost power or not.
Input:        PTIME_S
Output:       None
Return:       true or false :
Others:
*********************************************************************************/
bool Init_SD2068(PTIME_S	psRTC)
{
    u8 flag = 0;
    extern SL863X_CONTROLLER sl863x;

    F_Difference(327000 + sl863x.sl863xStorage.frequency);
    flag = I2CReadOneByte(RTC_Address,0x0F);
    if( (flag & 0x01) == 0) //RTC rest check
        return TRUE;

    //has lost power already.
    if(SD2068_I2CWriteDate(psRTC))
        return TRUE;
    else
        return FALSE;
}
/*********************************************************************************
Function:     SD2068_I2CWriteSingleReg
Description:  write single register.
Input:        None
Output:       None
Return:       ture or false
Others:
*********************************************************************************/
bool SD2068_I2CWriteSingleReg( u8 value,u8 address)
{
    bool status;
    WriteTimeOn();
    if(address == 0x02)
    {
        value |= 0x80; //24 hours model
    }

    status  = I2CWriteOneByte(RTC_Address,address,value);
    WriteTimeOff();
    return  status;
}

/*********************************************************************************
Function:     SD2068_I2CWriteBCDDate
Description:  adjust realy time using BCD code.
Input:        PTIME_S
Output:       None
Return:       true or false :
Others:
*********************************************************************************/
/******写SD2068 BCD实时数据寄存器******/
bool SD2068_I2CWriteBCDDate(PTIME_S	psRTC)
{

    psRTC->week = Get_Week(BCDtoHEX(psRTC->year),\
                           BCDtoHEX(psRTC->month),BCDtoHEX(psRTC->day));
    WriteTimeOn();				//write enable
    if(!I2CStart())return FALSE;
    I2CSendByte(RTC_Address);
    if(!I2CWaitAck())
    {
        I2CStop();
        return FALSE;
    }
    I2CSendByte(0x00);			//write start address
    I2CWaitAck();
    I2CSendByte(psRTC -> second);		//second
    I2CWaitAck();
    I2CSendByte(psRTC->minute);		//minute
    I2CWaitAck();
    I2CSendByte(psRTC->hour | 0x80);		//hour ,同时设置小时寄存器最高位（0：为12小时制，1：为24小时制）
    I2CWaitAck();
    I2CSendByte(psRTC->week);		//week
    I2CWaitAck();
    I2CSendByte(psRTC->day);		//day
    I2CWaitAck();
    I2CSendByte(psRTC->month);		//month
    I2CWaitAck();
    I2CSendByte(psRTC->year);		//year
    I2CWaitAck();
    I2CStop();

    WriteTimeOff();				//write disable.
    return	TRUE;
}
 /*********************************************************************************
Function:     SD2068_I2CReadBCD
Description:  read the SD2068 realy time data.
Input:        PTIME_S: realy time structure.
Output:       None
Return:       sucssesfully or failed.
Others:
*********************************************************************************/
bool SD2068_I2CReadBCD(PTIME_S	time)
{
    TIME_S sRTC;
    if(!I2CStart())return FALSE;
    I2CSendByte(RTC_Address+1);
    if(!I2CWaitAck())
    {
        I2CStop();
        return FALSE;
    }

    time->second = I2CReceiveByte();
    I2CAck();
    time->minute = I2CReceiveByte();
    I2CAck();
    time->hour  =  I2CReceiveByte() & 0x7F;//bit7   12/24
    I2CAck();
    time->week  = I2CReceiveByte();
    I2CAck();
    time->day  = I2CReceiveByte();
    I2CAck();
    time->month = I2CReceiveByte();
    I2CAck();
    time->year = I2CReceiveByte();
    I2CNoAck();		//读时间完成，发送NoAck
    I2CStop();
    return	TRUE;
}
/*********************************************************************
从ADC取值计算当前温度Get current temperature from ADC.
u16 Temp_inner	当前实际温度的十倍

*********************************************************************/

static void GetTprt_Frm_ADC(u16 AD_value)
{
    u16 Temp_ad;
    float Temp_ADData, Temp_ADData_Dis;
    Temp_ad = AD_value;						 //读电压值
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

#endif


/*@*****************************end of file*************************************@*/