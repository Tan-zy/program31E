#ifndef __APP_CFG_H
#define __APP_CFG_H

//�������ȼ�0-3ϵͳʹ��
#define 	OS_TASK_TMR_PRIO			4u   //�����ʱ�������ȼ�,����Ϊ4

#define 	STARTUP_TASK_PRIO			13u //��ʼ�������ȼ���ͣ����й����й��жϱ�֤�����н���
#define 	TASK_AutoSetting_PRIO		12u //��ʼ�������ȼ���ͣ����й����й��жϱ�֤�����н���
#define 	TASK_Display_PRIO			11u
#define 	TASK_Wireless_PRIO			10u
#define 	TASK_Execute_PRIO 			9u		//��������ͬ���ȼ�����
#define 	TASK_DataCollection_PRIO 	8u //
#define 	TASK_CAN_PRIO 				7u 		//can������޸�ȫ�ֱ�����û�м���������£���Ҫ�ȷ�����Щ������������ȼ���
#define   TASK_INDUCTION_PRIO   6u
#define 	TASK_DOG_PRIO 				5u 		//ι�����ȼ����
//ջ���
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
