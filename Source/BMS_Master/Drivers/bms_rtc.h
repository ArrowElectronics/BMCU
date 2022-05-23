/*!
********************************************************************************
* @file         :   bms_rtc.h
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   Prototypes for RTC module logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_I2C_H
#define BMS_I2C_H

#include <stdbool.h>

#include "rtc.h"         /* To access RTC definitions */
#include "nvic_table.h"
#include "bms_debug_log.h"

/* **** Definitions **** */

#define ALARM0_SEC        3
#define ALARM1_SEC        5
#define SNOOZE_SEC        7

/* **** Function Prototypes **** */

/**
 * @brief      To configure RTC
 */
bool RTC_Configure(void);

/**
 * @brief      To start RTC
 */
void RTC_Timer_Start( void );

/**
 * @brief      To stop RTC
 */
void RTC_Timer_Stop( void );

#endif /* BMS_I2C_H */
