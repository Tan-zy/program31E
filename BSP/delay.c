/********************************************
 **--------------�ļ���Ϣ---------------------
 ** �� �� ��: Delay.c
 ** �� �� ��: �ź���
 ** ��������: 2014��02��15��
 ** ��    ��: ����ϵͳ��ʹ�õ���ʱ����
 **
 *******************************************/

#include "delay.h"	

static u8 fac_us = 0; //us��ʱ������
static u16 fac_ms = 0; //ms��ʱ������,��ucos��,����ÿ�����ĵ�ms��

//��ʼ���ӳٺ���
//��ʹ��ucos��ʱ��,�˺������ʼ��ucos��ʱ�ӽ���
//SYSTICK��ʱ�ӹ̶�ΪHCLKʱ�ӵ�1/8
//SYSCLK:ϵͳʱ��
void delay_init(u8 SYSCLK)
{
	u32 reload;

	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK_Div8);	//ѡ���ⲿʱ��  HCLK/8
	fac_us = SYSCLK / 8;		//�����Ƿ�ʹ��ucos,fac_us����Ҫʹ��

	reload = SYSCLK / 8;		//ÿ���ӵļ������� ��λΪK
	reload *= 1000000 / OS_TICKS_PER_SEC;		//����OS_TICKS_PER_SEC�趨���ʱ��
	//reloadΪ24λ�Ĵ���,���ֵ:16777216,��72M��,Լ��1.86s����
	fac_ms = 1000 / OS_TICKS_PER_SEC;						//����ucos������ʱ�����ٵ�λ
	SysTick->CTRL |= (1ul << 1);//SysTick_CTRL_TICKINT_Msk;   	//����SYSTICK�ж�
	SysTick->LOAD = reload; 	//ÿ1/OS_TICKS_PER_SEC���ж�һ��
	SysTick->CTRL |= (1ul << 0);   	//����SYSTICK
}

//��ʱnus
//nusΪҪ��ʱ��us��.
void Delay_us(u32 nus)
{
	u32 ticks;
	u32 told, tnow, tcnt = 0;
	u32 reload = SysTick->LOAD;	//LOAD��ֵ
	ticks = nus * fac_us; 			//��Ҫ�Ľ�����
	tcnt = 0;
	OSSchedLock();				//��ֹucos���ȣ���ֹ���us��ʱ
	told = SysTick->VAL;        	//�ս���ʱ�ļ�����ֵ
	while (1)
	{
		tnow = SysTick->VAL;
		if (tnow != told)
		{
			if (tnow < told)
				tcnt += told - tnow;        	//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else
				tcnt += reload - tnow + told;
			told = tnow;
			if (tcnt >= ticks)
				break;        	//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}
	};
	OSSchedUnlock();			//����ucos����
}
//��ʱnms
//nms:Ҫ��ʱ��ms��
void Delay_ms(u16 nms)
{
	if (OSRunning == TRUE)			//���os�Ѿ�������
	{
		if (nms >= fac_ms)			//��ʱ��ʱ�����ucos������ʱ������
		{
			OSTimeDly(nms / fac_ms);			//ucos��ʱ
		}
		nms %= fac_ms;			//ucos�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ
	}
	Delay_us((u32)(nms * 1000));	//��ͨ��ʽ��ʱ
}

