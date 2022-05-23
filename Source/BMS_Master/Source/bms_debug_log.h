/*!
********************************************************************************
* @file         :   bms_debug_log.h
* @author       :   Ashvin Ramani
* @brief        :   Prototypes for Debug console logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef _DEBUG_LOG_H
#define _DEBUG_LOG_H

/* **** Includes **** */
#include <stdio.h>
#include <string.h>
#include "uart.h"
#include "bms_uart.h"
#include "bms_usb.h"


/* **** Definitions **** */

#define DEBUG_INTERFACE    MAIN_USB

typedef enum
{
	DEC,
	HEX,
	HEX16,
	HEX32,
} display_data;

/**
 * @brief      supported API's for debugging
 */
int DebugStr(const uint8_t interface, const char * buffer);
int DebugStrLf(const uint8_t interface, const char * buffer);
int DebugStrCr(const uint8_t interface, const char * buffer);
int DebugData(const uint8_t interface, const char * buffer, const int32_t data, const display_data type);
int DebugDataCr(const uint8_t interface, const char * buffer, const int32_t data, const display_data type);
int DebugDataLf(const uint8_t interface, const char * buffer, const int32_t data, const display_data type);
int DebugFloatData(const uint8_t interface, const char * buffer, const float data);
int DebugFloatDataCr(const uint8_t interface, const char * buffer, const float data);
int DebugFloatDataLf(const uint8_t interface, const char * buffer, const float data);

#endif /* _DEBUG_LOG_H */
