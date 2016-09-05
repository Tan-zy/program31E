#include "includes.h"

void BSP_Init(void)
{
	//SystemInit();
	//USART_ConfigInit();
	//LED_GPIO_Config();
	Target_Init();

}
/*
 void SysTick_init(void)
 {
 //SysTick_Config(SystemFrequency/OS_TICK_PER_SEC);
 SysTick_Config(SystemFrequency / OS_TICKS_PER_SEC);	//初始化并使能SysTick定时器
 }
 */
