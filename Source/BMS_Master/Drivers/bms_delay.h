/*!
********************************************************************************
* @file         :   bms_delay.h
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   Prototypes for Delay logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_DELAY_H
#define BMS_DELAY_H

#include <stdbool.h>

#include "tmr_utils.h"
#include "bms_debug_log.h"

#include "FreeRTOS.h"
#include "task.h"


/*
 *@brief: To set delay in micro seconds
 *@param1 micro: Number of microseconds
*/
void Delay_u(uint16_t micro);

/*
 *@brief: To set delay in milli seconds
 *@param1 micro: Number of milliseconds
*/
void Delay_m(uint16_t milli);

#endif /* BMS_DELAY_H */
