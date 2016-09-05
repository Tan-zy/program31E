/*************************************************
�ļ����ƣ�JF24D.c ����ģ��ײ�����������ģ���ͺ�JF24D-B��
����Ϊ����2014��4�� �޸ĺ�ĳ���
�޸ĺ�ĳ���Ƶ�� ���۶�֮ǰ��JF24Dģ�黹���µ�ģ��
����2400 + RF_CH���Ҿ�Ϊ 2400-2527	 ��MHz��

ģ���ͨ��ʱ��������һ��Ƶ���� &  ����ģ�鷢��ͨ���ĵ�ַ�����ģ�����ͨ���ĵ�ַ��ͬ

һ̨�������͸������ߴӻ�����ͬһ��Ƶ���ϣ��ӻ����Եĵ�ַ��Ӧ���Է��͵�ַ

 ***************************************************/
#include"JF24D.h"
#include"controller.h"
/**********All Function as Follow**************/
static void ChangeRx0Tx0Address(u8 address);

#if(JF24D_VSERSION04)
//�°�JF24D��2014.4��ģ���ϱ�ע��VERSION 04 ������ģ��
//�Ĵ�����1  0-13�żĴ����ĳ�ʼֵ

const u32 RegArrFSKAnalog[] = { 0xE2014B40, //Reg0
                                0x00004BC0, //Reg1
                                0x028CFCD0, //Reg2
                                0x41390099, //Reg3
                                0x1B8296D9, //Reg4 1B8296D9��Ӧ������1Mbps��2Mbps  1B8AB6D9��Ӧ������Ϊ250Kbps
                                0xA67F0224, //Reg5 A67F0224(����RSSI) A67F0624(�ر�RSSI)
                                0x00400000, //Reg6
                                0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00127300,	//Reg12
                                0x46B48000, //Reg13
                              };

//�°�JF24D
//�Ĵ�����1  14�żĴ����ĳ�ʼֵ
const u8 RegArrFSKAnalogReg14[] = { 0x41, 0x20, 0x08, 0x04, 0x81, 0x20, 0xCF,
                                    0xF7, 0xFE, 0xFF, 0xFF
                                  };

/**************************�Ĵ�����0��ʼֵ********************/
const u8 RegArrFSK[][2] = {
    { 0, 0x0F },
    { 1, 0x00 },
    { 2, 0x3F },
    { 3, 0x03 },
    { 4, 0xff },
    { 5, 0x17 },
    { 6, 0x07 },
    { 7, 0x07 },
    { 8, 0x00 },
    { 9, 0x00 },
    { 12, 0xc3 },
    { 13, 0xc4 },
    { 14, 0xc5 },
    { 15, 0xc6 },
    { 17, 0x20 },
    { 18, 0x20 },
    { 19, 0x20 },
    { 20, 0x20 },
    { 21, 0x20 },
    { 22, 0x20 },
    { 23, 0x00 },
    { 28, 0x3F },
    { 29, 0x07 }
};
 #else
 
//�ϰ������ģ�� ����ģ�鱳���ע VERSIN 03B
 //�Ĵ�����1  0-13�żĴ����ĳ�ʼֵ  
const u32 RegArrFSKAnalog[]={
0xE2014B40,
0x00004B40,
0x028CFCD0,
0x41390099,
0x0B869EC1,
0xA67F0224,
0x00400000,	  
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x00000000,
0x001A7300,
0x36B48000,
};

//�Ĵ�����1  14�żĴ����ĳ�ʼֵ  
const u8 RegArrFSKAnalogReg14[]=
{
0x41,0x20,0x08,0x04,0x81,0x20,0xCF,0xF7,0xFE,0xFF,0xFF
};

//�Ĵ�����0��ʼֵ
const u8 RegArrFSK[][2]={
{0,0x0F},		 //���üĴ���
{1,0x3F},
{2,0x3F},		 //ʹ�ܽ��յ�ַ
{3,0x03},		 //�ŵ������ݳ���
{4,0xff},
{5,0x17},        //Ƶ��ѡ��Ĵ���
{6,0x17}, 
{7,0x07},	     //״̬�Ĵ���
{8,0x00},		 //��Ƶ���üĴ���
{9,0x00},
{12,0xc3},
{13,0xc4},
{14,0xc5},
{15,0xc6},
{17,0x20},
{18,0x20},
{19,0x20},
{20,0x20},
{21,0x20},
{22,0x20},
{23,0x00},
{28,0x3F},
{29,0x07}
};
#endif
static u8 RX0_Address[] = { 0x12, 0x34, 0x56, 0x78, 0x01 };

u8 JTxCount, JRxCount;

u8 frequency;
void Delayus(u16 countor)
{
    do {
        u8 i= 30;
        while(i--);
    } while(countor--);

}
/**************************************************************
 **�������ƣ�SPI_RW()
 **����������дһ���ֽڵ�JF24D�������ض������ֽ�
 **��ڲ�����������ַ
 **���ڲ������������ֽ�
 **************************************************************/
u8 SPI_RW(u8 byte)
{
    u8 bit_ctr;
    for (bit_ctr = 0; bit_ctr < 8; bit_ctr++)
    {
        if ((byte & 0x80) != 0)
        {
            SPI_MOSI_H;
        }
        else
        {
            SPI_MOSI_L;
        }
        byte = (byte << 1);           // ��һλ���ֵ��λ����λ

        Delayus(1);

        SPI_CLK_H;                      // SCK��λ

        Delayus(1);

        if (SPI_MISO)			 //ע�⣺�˴������д����51��pic������ͬ������ֱ�ӽ�SPI_MISOֱֵ�Ӹ���byte
        {
            byte |= 1;      		  // ��MISO��ǰֵ
        }
        Delayus(1);

        SPI_CLK_L;

        Delayus(1);           		  // SCK����
    }
    return (byte);           		  // ���ض�����ֵ
}


/**************************************************************
 **�������ƣ�SPI_Write_Reg()
 **����������д�Ĵ�����ֵ
 **��ڲ������Ĵ�����ַ+����Ĵ�����ֵ
 **���ڲ�������
 **************************************************************/
void SPI_Write_Reg(u8 reg, u8 value)
{
    SPI_CSN_L;                   // ����CSN��ʹ��SPI
    SPI_RW(reg);      // д�Ĵ�����ַ+����
    SPI_RW(value);             // д��Ӧ��ֵ
    SPI_CSN_H;                   // ��λCSN����ֹSPI
}

/**************************************************************
 **�������ƣ�SPI_Read_Reg()
 **�������������Ĵ�����ֵ
 **��ڲ������Ĵ�����ַ+����
 **���ڲ������Ĵ�����ֵ
 **************************************************************/
u8 SPI_Read_Reg(u8 reg)
{
    u8 value;
    SPI_CSN_L;                // ����CSN��ʹ��SPI
    Delayus(1);
    SPI_RW(reg);            // д�Ĵ�����ַ+����
    value = SPI_RW(0);    // ���Ĵ�����ֵ
    Delayus(1);
    SPI_CSN_H;                // ��λCSN����ֹSPI

    return (value);        // ���ؼĴ�����ֵ
}

/**************************************************************
 **�������ƣ�SPI_Read_Buf()
 **���������������ֽڼĴ�����ֵ
 **��ڲ������Ĵ�����ַ+�������ֵ�ĵ�ַ���Ĵ���ֵ�ĳ���
 **���ڲ�������
 **************************************************************/
void SPI_Read_Buf(u8 reg, u8 *pBuf, u8 bytes)
{
    u8 byte_ctr;

    SPI_CSN_L;                    		// ����CSN��ʹ��SPI
    SPI_RW(reg);       		// д�Ĵ�����ַ+����

    for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++)
    {
        pBuf[byte_ctr] = SPI_RW(0);    // ���Ĵ�����ֵ
    }
    SPI_CSN_H;                           // ��λCSN����ֹSPI
}

/**************************************************************
 **�������ƣ�SPI_Write_Buf()
 **����������д���ֽڼĴ�����ֵ
 **��ڲ������Ĵ�����ַ+���д��ֵ�ĵ�ַ���Ĵ���ֵ�ĳ���
 **���ڲ�������
 ***************************************************************/
void SPI_Write_Buf(u8 reg, const u8 *pBuf, u8 bytes)
{
    u8 byte_ctr;

    SPI_CSN_L;                   // ����CSN��ʹ��SPI
    SPI_RW(reg);    // д�Ĵ�����ַ+����
    for (byte_ctr = 0; byte_ctr < bytes; byte_ctr++) // д�Ĵ�����ֵ
    {
        SPI_RW(pBuf[byte_ctr]);
    }
    SPI_CSN_H;                 // ��λCSN����ֹSPI
}

/**************************************************************
 **�������ƣ�Enter_PRX()
 **�����������л���PRXģʽ
 **��ڲ�������
 **���ڲ�������
 ***************************************************************/
void Enter_PRX(void)
{
    u8 value;

    SPI_Write_Reg(FLUSH_RX, 0);                 //��ս���FIFO

    value = SPI_Read_Reg(STATUS);	// ��STATUS�Ĵ���
    SPI_Write_Reg(SPI_WRITE_REG + STATUS, value);	// ����RX_DR��TX_DS��MAX_RT��־

    value = SPI_Read_Reg(CONFIG);	// ��CONFIG�Ĵ���

    value = value & 0xfd;	//λ1����
    SPI_Write_Reg(SPI_WRITE_REG + CONFIG, value); // ����PWR_UPλ������POWER_DOWNģʽ

    value = value | 0x03; //��λλ1��λ0
    SPI_Write_Reg(SPI_WRITE_REG + CONFIG, value); // ��λPWR_UP��PRIM_RX������PRXģʽ

    //EXTI_Configuration(TRUE);  //�������ж�
}

/**************************************************************
 **�������ƣ�Enter_PTX()
 **�����������л���PTXģʽ
 **��ڲ�������
 **���ڲ�������
 ***************************************************************/
void Enter_PTX(void)
{
    u8 value;

    SPI_Write_Reg(FLUSH_TX, 0); //��ս���FIFO

    value = SPI_Read_Reg(STATUS);	// ��STATUS�Ĵ���
    SPI_Write_Reg(SPI_WRITE_REG + STATUS, value);	// ����RX_DR��TX_DS��MAX_RT��־

    value = SPI_Read_Reg(CONFIG);	// ��CONFIG�Ĵ���
    value = value & 0xfd;	//λ1����
    SPI_Write_Reg(SPI_WRITE_REG + CONFIG, value); // ����PWR_UPλ������POWER_DOWNģʽ

    value = value | 0x02; //��λλ1
    value = value & 0xfe; //λ0����
    SPI_Write_Reg(SPI_WRITE_REG + CONFIG, value); // ��λPWR_UP������PRIM_RX������PTXģʽ
}

/**************************************************************
 **�������ƣ�SwitchCFG()
 **�����������л��Ĵ�����
 **��ڲ�������Ҫ�л����Ĵ����飬0��1
 **���ڲ�������
 ***************************************************************/
void SwitchCFG(u8 cfg)
{
    u8 Tmp;

    Tmp = SPI_Read_Reg(STATUS);		//��STATUS�Ĵ���
    Tmp = Tmp & 0x80;

    if (((Tmp) && (cfg == 0)) || ((Tmp == 0) && (cfg)))		//�жϵ�ǰ�Ĵ������Ƿ��ǽ�Ҫ�л���
    {
        SPI_Write_Reg(ACTIVATE_CMD, 0x53);		   //ִ���л�
    }
}

/**************************************************************
 **�������ƣ�JF24D_Init()
 **������������ʼ��JF24D
 **��ڲ�������
 **���ڲ�������
 ***************************************************************/
void JF24D_Init(void)
{
    u8 i, j;
    u8 WriteArr[4];

    /************************��ʼ���Ĵ�����1*********************************/
    SwitchCFG(1); 	  //�л����Ĵ�����1

    for (i = 0; i <= 8; i++) 			//дǰ0-8�żĴ���
    {
        for (j = 0; j < 4; j++)
        {
            WriteArr[j] = (RegArrFSKAnalog[i] >> (8 * (j))) & 0xff; //���Ĵ���ֵ��ŵ������У��ȸ��ֽ�
        }
        SPI_Write_Buf((SPI_WRITE_REG | i), &(WriteArr[0]), 4); 		//д�Ĵ���
    }
    for (i = 9; i <= 13; i++) 										//д9-13�żĴ���
    {
        for (j = 0; j < 4; j++)
        {
            WriteArr[j] = (RegArrFSKAnalog[i] >> (8 * (3 - j))) & 0xff; //���Ĵ���ֵ��ŵ������У��ȵ��ֽ�
        }
        SPI_Write_Buf((SPI_WRITE_REG | i), &(WriteArr[0]), 4); 			//д�Ĵ���
    }
    //SPI_Write_Buf((WRITE_REG|14),RegArrFSKAnalogReg14,11); 	//д14�żĴ���
    SPI_Write_Buf((SPI_WRITE_REG | 14), &(RegArrFSKAnalogReg14[0]), 11);

    /***************дREG4[0]=0��REG4[1]=0��REG4[3]=0��REG4[23]=0 **************/
    for (j = 0; j < 4; j++)
    {
        WriteArr[j] = (RegArrFSKAnalog[4] >> (8 * (j))) & 0xff;
    }
    WriteArr[3] = WriteArr[3] & 0xf4;
    WriteArr[1] = WriteArr[1] & 0x7F;
    SPI_Write_Buf((SPI_WRITE_REG | 4), &(WriteArr[0]), 4);

    /***********дREG4[25]=1��дREG4[26]=1 *********************************/
    WriteArr[0] = WriteArr[0] | 0x06;
    SPI_Write_Buf((SPI_WRITE_REG | 4), &(WriteArr[0]), 4);

    /*************дREG4[25]=0��дREG4[26]=0*****************************/
    WriteArr[0] = WriteArr[0] & 0xf9;
    SPI_Write_Buf((SPI_WRITE_REG | 4), &(WriteArr[0]), 4);

    /************дREG4[0]=1��REG4[1]=1��REG4[3]=1��REG4[23]=1***********/
    for (j = 0; j < 4; j++)
    {
        WriteArr[j] = (RegArrFSKAnalog[4] >> (8 * (j))) & 0xff;
    }
    SPI_Write_Buf((SPI_WRITE_REG | 4), &(WriteArr[0]), 4);

    /******************��ʼ���Ĵ�����0*********************************/
    SwitchCFG(0); 		//�л����Ĵ�����0

    for (i = 21; i > 0; i--)
    {
        SPI_Write_Reg((SPI_WRITE_REG | RegArrFSK[i - 1][0]),
                      RegArrFSK[i - 1][1]);
    }


    //	WirteAddrToJf24D(1);�����ͺ��沿���ظ�
    i = SPI_Read_Reg(29); 		//���Ĵ���29
    if (i == 0) 			//�Ƿ���Ҫ����
    {
        SPI_Write_Reg(ACTIVATE_CMD, 0x73);  //����Ĵ���29

        SPI_Write_Reg((SPI_WRITE_REG | RegArrFSK[22][0]), RegArrFSK[22][1]); //д�Ĵ���28��29
        SPI_Write_Reg((SPI_WRITE_REG | RegArrFSK[21][0]), RegArrFSK[21][1]); //д�Ĵ���28��29

    }

}

/***********************************************
 *��������IsInitWirelessModuleSuccess(void)
 *���ã�  ���ôӼĴ�������ֵ�жϳ�ʼ���Ƿ�ɹ���
 *����ֵ������0��ʾ��ʼ���ɹ���
 *ʱ�䣺 2009��12��19��
 *���ߣ� �ﻳ��
 *************************************************/
//����ӼĴ���29����ֵΪ0x07����д���ֵ��ͬ��˵����ʼ���ɹ���
//�������ֵΪ0�򲻳ɹ�
u8 IsInitWirelessModuleSuccess(void)
{
    u8 rdata;
    rdata = SPI_Read_Reg(29); 		//���Ĵ���29
    if (rdata == 0x07)
    {
        return 1;	  //��ʼ���ɹ�
    }
    else
    {
        return 0;	 //��ʼ��ʧ��
    }
}

/**************************************************************
 **�������ƣ�Send_Packet()
 **�����������������ݰ�
 **��ڲ�����д����FIFO���д��ֵ�ĵ�ַ���Ĵ���ֵ�ĳ���
 **���ڲ�������
 ***************************************************************/
void Send_Packet(u8 type, u8* pbuf, u8 len)
{
    u8 fifo_sta;

    fifo_sta = SPI_Read_Reg(FIFO_STATUS);	// ���Ĵ���FIFO_STATUS

    if ((fifo_sta & FIFO_STATUS_TX_FULL) == 0)	//�жϷ���FIFO�Ƿ���
    {
        SPI_Write_Buf(type, pbuf, len); // д����FIFO
    }
}

/**************************************************************
 **�������ƣ�Receive_Packet()
 **�����������������ݰ�
 **��ڲ�����������FIFO�������ֵ�ĵ�ַ
 **���ڲ��������ݰ��ĳ���
 ***************************************************************/
u8 Receive_Packet(u8* rx_buf)
{
    u8 len, fifo_sta;

    do
    {
        len = SPI_Read_Reg(R_RX_PL_WID_CMD);	// �����ݰ�����

        if (len <= MAX_PACKET_LEN)
        {
            SPI_Read_Buf(RD_RX_PLOAD, rx_buf, len);	// ������FIFO
        }
        else
        {
            SPI_Write_Reg(FLUSH_RX, 0);	//���ݰ����ȴ�����󳤶ȣ����FIFO
        }

        fifo_sta = SPI_Read_Reg(FIFO_STATUS);	//���Ĵ���FIFO_STATUS

    } while ((fifo_sta & FIFO_STATUS_RX_EMPTY) == 0); //�����Ϊ�գ�������

    return (len);		  //�������ݰ�����
}

/**************************************************************
 **�������ƣ�ChangeRx0Tx0Address
 **�������������Ľ��պͷ���ͨ����ַ�������ַ��0~255�޸��ò��֣�
 **��ڲ�������
 **���ڲ�������

 ***************************************************************/
void ChangeRx0Tx0Address(u8 address)
{

    RX0_Address[0] = address;
    SPI_Write_Buf((SPI_WRITE_REG + RX_ADDR_P0), RX0_Address, 5); //д�Ĵ���10��ͨ��0��ַ

    //SPI_Write_Buf((WRITE_REG+RX_ADDR_P1),RX1_Address,5); 		//д�Ĵ���11��ͨ��1��ַ��������ͨ����λ��ַ
    SPI_Write_Buf((SPI_WRITE_REG + TX_ADDR), RX0_Address, 5); 	//д�Ĵ���16������ͨ����ַ
}
/****************************************************************************
 ����:	����Ƶ��
 ����:
 ����:		channel �� 1  to 254 ( channel��127ȡ�� ����127����ǰ��ĸ��� )

 ����ֵ:	��
 ��ע:
 ******************************************************************************/
void ChangeRF24Dchannel(u8 channel)
{
    //RX0_Address[0] =  channel;
    frequency = channel % 127;
    SPI_Write_Reg((SPI_WRITE_REG | 5), frequency);
}

/***********************************************
 *��������InitWirelessModule
 *���ã�  ��ʼ��JF24Dģ��
 *������  add_set ���պͷ���ͨ����ַ��channel ͨ��Ƶ��
 *ʱ�䣺
 *���ߣ�
 *************************************************/
void InitWirelessModule(u8 add_set,u8 channel)
{

    Delay_ms(10);
    JF24D_Init();		//��ʼ��JF24D
    Delay_ms(5);
    ChangeRx0Tx0Address(add_set);
    ChangeRF24Dchannel(channel);
    EXTI_Configuration(TRUE);
    Delay_ms(5);

}

/***********************************************
 *��������InitWirelessModule
 *���ã�  ��ʼ��jf24d
 *������  add_set
 *ʱ�䣺  koing2010@2015/6/5
 *���ߣ�  �Ұ�JF24D�ĵײ�������˽ϴ�ĸĶ�
 *************************************************/
//��SL8410��SL8411��SL8412���Ϳ�������
void CreatCmd(u8 Addr, u8 FunCode, u8 OrderData)	//�ӻ���ַ�������룬��������
{
#if OS_CRITICAL_METHOD == 3
    OS_CPU_SR cpu_sr;
#endif
    u8 JTxBuf[32] = { 0 }; //ע:������¥
    u8 JTxCount = 0;
    u16 CrcData = 0;
    u16 i = 0;
    JTxBuf[0] = Addr;
    JTxBuf[1] = FunCode;
    JTxBuf[2] = OrderData;
    JTxCount = 5;

    CrcData = CRC_16(JTxBuf, JTxCount - 2);	//�õ�crcУ����
    JTxBuf[JTxCount - 2] = CrcData & 0xff;
    JTxBuf[JTxCount - 1] = CrcData >> 8;  //��crcУ������뷢�ͻ�������

    OS_ENTER_CRITICAL()
    ChangeRx0Tx0Address(JTxBuf[0]);//�����͵�ַ�л�
    EXIT_TX_RX();
    Delay_us(10);
    Enter_PTX();		//�л���PTXģʽ
    ENTER_TX_RX();			//����TX

    Delay_us(1000);//��ʱ�ȴ�PA����

    Send_Packet(W_TX_PAYLOAD_NOACK_CMD, JTxBuf, JTxCount);

    OS_EXIT_CRITICAL()

    extern SL863X_CONTROLLER sl863x;
    u8 err= OS_ERR_NONE;
    OSSemPend(sl863x.semWirelessTx,80ul,&err);
    if(err== OS_ERR_TIMEOUT)
    {
        OSSemSet(sl863x.semWirelessTx,0,&err);
    }
    EXIT_TX_RX();
    Delay_us(10);
    Enter_PRX();		//�л���PRXģʽ
    ENTER_TX_RX();		//����RX



}
