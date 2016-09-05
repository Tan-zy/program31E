/*******************************************************************************

File_name:      RTCcalibration.c
Version:		   0.1
Revised:        $Date:2016-3-2  ; $
Description:    part of rtc.
Notes:          sd2068 & bl5372
Editor:		    Mr.Kong

*******************************************************************************/


/*==============================================================================
@ Include files
*/
#include "RTCcalibration.h"


/*==============================================================================
@ Global variable
*/




/*==============================================================================
@ All functions  as follow
*/



/*******************************************************************************
Function: F0_calculate
Description: if  f0 = 32768.8,this function will return 327688.

Input:   Freq1 is the rtc osc frequency in the temperature the Freq1 test it in.
Output:       None
Return:       None
Others:	   koing2010@2016-3-2.
*******************************************************************************/
u32 F0_calculate(u32 Freq1,u16 current_temp)
{
	 float PPM1 = (0.032 * (pow((current_temp / 10.0 - 25.0), 2)));	
	 return  (u32)Freq1/(1.0 - PPM1/1000000.0);
}
/*******************************************************************************
Function:    F0_calculate
Description: if f0 = 32768.8,this function will return 327688.

Input:     freq_cal: frequency calculated in last function.
Output:    if it return 100,this means  10.0 PPM, 
            ppm_current  > 0, freq_cal > 32768
			ppm_current  < 0, freq_cal < 32768
			
			write return value in RTC IC register 
			
Return:    None
Others:	   koing2010@2016-3-2.
*******************************************************************************/
u8 RTCcalibration(u32 freq_cal,u16 current_temp)
{
	u32 freq1_current;s16 different_freq ;
	float PPM1 = (0.032 * (pow((current_temp / 10 - 25), 2)));	
  freq1_current = freq_cal*(1.0 - PPM1/1000000.0);
	different_freq = (s16)(freq1_current - 327680);	 
	
	if(different_freq  >= 0)
	{
	  return different_freq + 1;//PPM_temp *0.32768 + 0.5) + 1
    }
	else
	{
   different_freq = 0 - different_freq;
	 return (0x80 - (u8)(0 - different_freq)) | 0x40; 		
	}
	
}























/*@*****************************end of file**********************************@*/
