#ifndef __APP_CFG_H
#define __APP_CFG_H

//任务优先级0-3系统使用
#define 	OS_TASK_TMR_PRIO			4u   //软件定时器的优先级,设置为4

#define 	STARTUP_TASK_PRIO			13u //开始任务优先级最低，运行过程中关中断保证其运行结束
#define 	TASK_AutoSetting_PRIO		12u //开始任务优先级最低，运行过程中关中断保证其运行结束
#define 	TASK_Display_PRIO			11u
#define 	TASK_Wireless_PRIO			10u
#define 	TASK_Execute_PRIO 			9u		//不能有相同优先级任务
#define 	TASK_DataCollection_PRIO 	8u //
#define 	TASK_CAN_PRIO 				7u 		//can任务会修改全局变量，没有加锁的情况下，他要比访问这些变量任务的优先级高
#define   TASK_INDUCTION_PRIO   6u
#define 	TASK_DOG_PRIO 				5u 		//喂狗优先级最高
//栈深度
#define 	STARTUP_TASK_STK_SIZE				        80
#define 	TASK_DOG_STK_SIZE					          80
#define 	TASK_Display_STK_SIZE  			     	  256
#define 	TASK_CtrlOutput_STK_SIZE  		      128
#define 	TASK_Execute_STK_SIZE  				      128
#define 	TASK_DataCollection_STK_SIZE  		  128
#define 	TASK_CAN_STK_SIZE  					        128
#define 	TASK_Energy_STK_SIZE  				      128
#define		TASK_Wireless_STK_SIZE				      128
#define		TASK_AutoSetting_STK_SIZE			      80
#define   TASK_INDUCTION_STK_SIZE             100
#endif
