/*******************************************************************************

File_name:      RTCcalibration.h
Description:    the header file of  RTCcalibration.c ,.

*******************************************************************************/
#ifndef     __RTCCALIBRATION_H__
#define     __RTCCALIBRATION_H__

/*==============================================================================
@ Include files
*/
#include "includes.h"

/*==============================================================================
@ Typedefs
*/


/*==============================================================================
@ Constants and defines
*/

u32 F0_calculate(u32 Freq1,u16 current_temp);
u8  RTCcalibration(u32 freq_cal,u16 current_temp);






#endif
/*@*****************************end of file**********************************@*/


