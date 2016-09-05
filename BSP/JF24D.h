#ifndef _JF24D_H_ 
#define	_JF24D_H_

#include "includes.h"

/***************** 无线引脚 选择*******************/

#define SPI_CLK_PIN		GPIO_Pin_7
#define SPI_MOSI_PIN	GPIO_Pin_6
#define SPI_CSN_PIN		GPIO_Pin_5

#define CE_PIN    		GPIO_Pin_4   //Reset n
#define IRQ_PIN   		GPIO_Pin_4
#define SPI_MISO_PIN	GPIO_Pin_5


//////////////////////////////////////////////////////////////////////////////////

///SPI CLK
#define SPI_CLK_H		GPIOA->BSRR = SPI_CLK_PIN
#define SPI_CLK_L		GPIOA->BRR  = SPI_CLK_PIN 
//SPI MOSI
#define SPI_MOSI_H		GPIOA->BSRR = SPI_MOSI_PIN
#define SPI_MOSI_L		GPIOA->BRR  = SPI_MOSI_PIN 
//SPI SS
#define SPI_CSN_H		GPIOA->BSRR = SPI_CSN_PIN
#define SPI_CSN_L		GPIOA->BRR  = SPI_CSN_PIN  

// RESETn
#define CE_H			GPIOC->BSRR = CE_PIN
#define CE_L			GPIOC->BRR  = CE_PIN  

//PKTFLAG
#define IRQ				 GPIOA->IDR & IRQ_PIN
//SPI_MISO
#define SPI_MISO		GPIOC->IDR & SPI_MISO_PIN

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

//寄存器命令宏定义
//#define MAX_PACKET_LEN  10// 数据包最大长度，最大为255

/***********SPI命令***********************************/
#define MAX_PACKET_LEN  32// 数据包最大长度，最大为255
/***********SPI命令***********************************/
//#define READ_REG        0x00        // 读寄存器
#define SPI_WRITE_REG       0x20        // 写寄存器
#define RD_RX_PLOAD     0x61        // 读接收FIFO
#define WR_TX_PLOAD     0xA0        // 写发射FIFO
#define FLUSH_TX        0xE1        // 清空发射FIFO
#define FLUSH_RX        0xE2        // 清空接收FIFO
#define REUSE_TX_PL     0xE3        // 重复发射数据包
#define W_TX_PAYLOAD_NOACK_CMD	0xb0	 //写发射FIFO，无应答
#define W_ACK_PAYLOAD_CMD	0xa8	 //写应答FIFO
#define ACTIVATE_CMD		0x50	 //ACTIVATE命令
#define R_RX_PL_WID_CMD		0x60	 //读数据包长度
#define NOP             0xFF  // 无操作
/*********寄存器组0的寄存器声明***********************/
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define PAYLOAD_WIDTH   0x1f  // 'payload length of 256 bytes modes register address
/************STATUS寄存器状态********************/
#define STATUS_RX_DR 0x40	  
#define STATUS_TX_DS 0x20
#define STATUS_MAX_RT 0x10

#define STATUS_TX_FULL 0x01

/***********FIFO_STATUS寄存器状态**********************/
#define FIFO_STATUS_TX_REUSE 0x40
#define FIFO_STATUS_TX_FULL 0x20
#define FIFO_STATUS_TX_EMPTY 0x10

#define FIFO_STATUS_RX_FULL 0x02
#define FIFO_STATUS_RX_EMPTY 0x01

#define ENTER_TX_RX() CE_H	   //进入接收或发射
#define EXIT_TX_RX()  CE_L	   //退出接收或发射
/////////////////////////////////////////////////////////////////
//extern  u8 read_data[2];
extern u8 JTxBuf[40];
extern u8 JRxBuf[40];
extern u8 JTxCount, JRxCount;
extern bool RecFlag;

void Handle_Data1(u8 key);	//处理来自编程器的数据，数据太多拆成两部分
void Handle_Data2(u8 key);	//处理来自编程器的数据
void Send_Data(void);		//向编程器发送数据

u8 SPI_RW(u8 byte);
void SPI_Write_Reg(u8 reg, u8 value);
u8 SPI_Read_Reg(u8 reg);
void SPI_Read_Buf(u8 reg, u8 *pBuf, u8 bytes);
void SPI_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes);
void Enter_PRX(void);
void Enter_PTX(void);
void SwitchCFG(u8 cfg);
void JF24D_Init(void);
u8 IsInitWirelessModuleSuccess(void);
void Send_Packet(u8 type, u8* pbuf, u8 len);
u8 Receive_Packet(u8* rx_buf);
void InitWirelessModule(u8 add_set,u8 channel);
//u16	CRC_16(u8 *puchMsg,u8 count) ;
extern void CreatCmd(u8 Addr, u8 FunCode, u8 OrderData);		//从机地址，功能码，命令数据
//void  CreatCmd1(u8 Addr,u8 FunCode,u8 OutNum,u8 OrderData);	//从机地址，功能码，输出路，命令数据
void Wireless_RXData(void);

void ChangeRF24Dchannel(u8 channel);
extern bool FlagJf24dOk;
extern u8 Rx_Data;
#endif
