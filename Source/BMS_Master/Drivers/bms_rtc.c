/*!
********************************************************************************
* @file         :   bms_rtc.c
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   RTC module logic
*******************************************************************************/

/***** Includes *****/
#include "bms_rtc.h"

static void RTC0_Handler_Compare( void );

bool RTC_Configure( void )
{
	int result;
	bool RetValue = true;

    rtc_cfg_t RTCconfig;

    //set RTC configuration
    RTCconfig.compareCount[0] = ALARM0_SEC; //alarm0 time in seconds
    RTCconfig.compareCount[1] = ALARM1_SEC; //alarm1 time in seconds
    RTCconfig.prescaler = RTC_PRESCALE_DIV_2_12; //1Hz clock
    RTCconfig.prescalerMask = RTC_PRESCALE_DIV_2_11;//0.5s prescaler compare
    RTCconfig.snoozeCount = SNOOZE_SEC;//snooze time in seconds
    RTCconfig.snoozeMode = RTC_SNOOZE_MODE_B;
    result = RTC_Init(&RTCconfig);

    //setup interrupt callbacks and enable
    NVIC_SetVector(RTC0_IRQn, RTC0_Handler_Compare);
    //NVIC_SetVector(RTC1_IRQn, RTC1_handler_Compare1);
    //NVIC_SetVector(RTC2_IRQn, RTC2_handler_PrescalerCMP);

    //RTC_EnableINT(MXC_F_RTC_INTEN_COMP0 | MXC_F_RTC_INTEN_COMP1 | MXC_F_RTC_INTEN_PRESCALE_COMP);
    RTC_EnableINT(MXC_F_RTC_INTEN_COMP0);

    if ( E_NO_ERROR != result )
    {
    	RetValue = false;
		//DebugDataLf( MAIN_USB, "I2C_Configure error:", result, DATA_DEC);
	}

    return RetValue;
}

void RTC_Timer_Start( void )
{
	RTC_Start();
}

void RTC_Timer_Stop( void )
{
	RTC_Stop();
}

static void RTC0_Handler_Compare( void )
{
    //DebugStrLf( MAIN_USB, "RTC0_Handler_Compare" );

    //disable interrupt
    RTC_DisableINT(MXC_F_RTC_INTEN_COMP0);

    //clear flag
    RTC_ClearFlags(MXC_F_RTC_FLAGS_COMP0);
}
