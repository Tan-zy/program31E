#ifndef STM32_INIT_H
#define STM32_INIT_H

#include "stm32f10x.h"
#include "includes.h"


	/**µçÔ´¿ØÖÆÒý½Å**/
	#define SLEEP_PROT	GPIOC
	#define SLEEP_PIN	GPIO_Pin_4
	/**ADCµçÑ¹²É¼¯Òý½Å**/
	#define ADC_PORT	GPIOC
	#define ADC_PIN		GPIO_Pin_5
  
	#define  TM1637_DIO    GPIO_Pin_10
	#define  TM1637_DCLK   GPIO_Pin_11
  #define  TM1637_PORT   GPIOC

	#define VT_Pin      GPIO_Pin_0
	#define D0_Pin      GPIO_Pin_13
	#define D1_Pin      GPIO_Pin_12
	#define D2_Pin      GPIO_Pin_11
	#define D3_Pin      GPIO_Pin_10
	#define D4_Pin      GPIO_Pin_1
	
	#define Remot_Port	GPIOB



/*****************************************/
#define VT      Remot_Port->IDR & VT_Pin
#define D0      Remot_Port->IDR & D0_Pin
#define D1      Remot_Port->IDR & D1_Pin
#define D2      Remot_Port->IDR & D2_Pin
#define D3      Remot_Port->IDR & D3_Pin
#define D4      Remot_Port->IDR & D4_Pin


//·äÃùÆ÷
#define BUZZ_PIN	GPIO_Pin_2
#define BUZZ_PIN_X	2
#define BUZZ_PORT	GPIOD


#define Buzz_H	GPIO_WriteBit(BUZZ_PORT, BUZZ_PIN, Bit_SET);
#define Buzz_L	GPIO_WriteBit(BUZZ_PORT, BUZZ_PIN, Bit_RESET);


//²âÊÔ×´Ì
#define TEST_IO    GPIO_Pin_15
#define TEST_PORT  GPIOB
#define TEST_MODEL GPIO_ReadInputDataBit(TEST_PORT, TEST_IO ) 



	#define P101_PORT		GPIOA
	#define P102_PORT		GPIOC

	#define P101_Pin		GPIO_Pin_8   //VCON
	#define P102_Pin		GPIO_Pin_9   //FAN



/*----------------------------------------------------------------------------------------*/
#define H		1
#define L		0

#define P101(x)			((x)==(1)) ? (P101_PORT->BSRR=P101_Pin) : (P101_PORT->BRR=P101_Pin)
#define P102(x)			((x)==(1)) ? (P102_PORT->BSRR=P102_Pin) : (P102_PORT->BRR=P102_Pin)

/******************************************************************************************/


#define Tim2_5ms_OFF	TIM_Cmd(TIM2,DISABLE);TIM2->CNT=0		 //¹Ø¶¨Ê±Æ÷2£»Çå¿Õ¼ÆÊýÆ÷
#define Tim2_5ms_ON		TIM_Cmd(TIM2,ENABLE)					 //¿ª¶¨Ê±Æ÷2£»

#define Tim3_100ms_OFF	TIM_Cmd(TIM3,DISABLE);TIM3->CNT=0		 //¹Ø¶¨Ê±Æ÷3£»Çå¿Õ¼ÆÊýÆ÷
#define Tim3_100ms_ON	TIM_Cmd(TIM3,ENABLE)					 //¿ª¶¨Ê±Æ÷3£»

#define Tim4_5ms_OFF	TIM_Cmd(TIM4,DISABLE);TIM4->CNT=0		 //¹Ø¶¨Ê±Æ÷2£»Çå¿Õ¼ÆÊýÆ÷
#define Tim4_5ms_ON		TIM_Cmd(TIM4,ENABLE)					 //¿ª¶¨Ê±Æ÷2£»

void Wireless_GPIO(void);
void Output_P203(void);
void Output_P204(void);

void Target_Init (void);
void Application_Init(void);
void Can_Config(u8 CAN_Speed,u16 Can_Addr);
void EXTI_Configuration(bool flag);
void TIM2_Configuration(void);
void TIM3_Configuration(void);
void TIM4_Configuration(void);
int fputc(int ch, FILE *f);

#endif

