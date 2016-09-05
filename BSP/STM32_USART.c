/*****************************************************************************
 *文件名：Usart.c 485发送处理模块
 *描述：	 RS485 1和3的发送处理操作；
 注意RS485 1与传感器通信， 3与选控开关通信，并且他们的接收方式有区别：
 1、RS485 1采用超时接收，每接收一个字节开启定时器2，当计时结束进入定时
 器2中断时认为接收完成进入数据处理函数；
 2、RS485 3采用字节计数接收，当接收N字节时进入数据处理函数
 （为防止接收紊乱发送时需将计数清零）；
 有多余定时器时推荐使用方法1；
 文件历史
 版本号		日期		作者		说明
 V4.1		2012.05.03  王伟
 --------------------------修改说明-----------------------------------
 描述：RS485  USART1与传感器通信
 RS232  USART3与计量模块通信
 文件历史
 版本号		日期		作者		说明
 V5.2		2014.02.19  张豪杰
 *****************************************************************************/
#include "STM32_USART.h"
//USART1 接收、发送数组
/*u8 USART1_RxBuf[10];
 u8 USART1_TxBuf[10]; */
//USART3 接收、发送数组
u8 USART3_RxBuf[10];
u8 USART3_TxBuf[10];
/********************************************************************************************
 *函数名称：void UART1_SandData(u8 *str,u8 length)
 *入口参数：*str 数组的首地址；length 要发送的字节个数
 *出口参数：无
 *功能说明：USART1送多字节函数；
 *******************************************************************************************/
void USART1_SandData(u8 *str, u8 length)
{
	u8 i;

	MDE_H;				   //置位发送引脚口
	Delay_ms(1);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE); 	//取消中断接收
	for (i = 0; i < length; i++)
	{
		USART_SendData(USART1, str[i]);  		     //向USART1发送一字节数据  
		while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET)
		{
		} 	//通过检查发送寄存器标识位来判断数据是否发送完毕
	}
	Delay_ms(1);
	MDE_L;		   //复位发送引脚口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);  //发送完数据开启接收中断
}
/********************************************************************************************
 *函数名称：void UART3_SandData(u8 *str,u8 length)
 *入口参数：*str 数组的首地址；length 要发送的字节个数
 *出口参数：无
 *功能说明：USART3送多字节函数；
 *******************************************************************************************/
void USART3_SandData(u8 *str, u8 length)
{
	u8 i;

	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE); 	//取消中断接收
	for (i = 0; i < length; i++)
	{
		USART_SendData(USART3, str[i]);  		     //向USART1发送一字节数据  
		while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET)
		{
		} 	//通过检查发送寄存器标识位来判断数据是否发送完毕
	}

	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);  //发送完数据开启接收中断
}
/********************************************************************************************
 *函数名称：u16 CRC_16(u8 *puchMsg,u8 count)
 *入口参数：*puchMsg 数组的首地址；count 要计算的字节个数
 *出口参数：Verify_Data： 返回的CRC校验值
 *功能说明：CRC计算函数；
 *******************************************************************************************/
u16 CRC_16(u8 *puchMsg, u8 count)  //计算crc校验码
{
	u8 i, j;
	u8 XORResult;
	u16 xorCRC = 0xA001; //设置CRC的多项式 //
	u16 Verify_Data = 0xffff;
	for (i = 0; i < count; i++)
	{
		Verify_Data ^= puchMsg[i]; 	   //将0xffff和8位数据相异或
		for (j = 0; j < 8; j++)
		{
			XORResult = Verify_Data & 1; 	 //判断将要移除的一位是否为一
			Verify_Data >>= 1;
			if (XORResult) 	   //移除位为1则将crc与多项式相与
			{
				Verify_Data ^= xorCRC;
			}
		}
	}
	return Verify_Data;			//返回16位的crc校验码
}
