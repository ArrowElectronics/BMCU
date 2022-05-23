/*!
********************************************************************************
* @file         :   bms_delay.c
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   Delay logic
*******************************************************************************/

/***** Includes *****/
#include "bms_delay.h"

void Delay_u(uint16_t micro)
{
	TMR_Delay(MXC_TMR0, micro); // FreeRTOS delay cannot be used for micro seconds so using Hardware Time by default
}

void Delay_m(uint16_t milli)
{
	BaseType_t schedulerState = xTaskGetSchedulerState();

	if ( schedulerState == taskSCHEDULER_RUNNING )
	{
		vTaskDelay(pdMS_TO_TICKS(milli));
	}
	else
	{
		TMR_Delay(MXC_TMR0, MSEC(milli));
	}
}

