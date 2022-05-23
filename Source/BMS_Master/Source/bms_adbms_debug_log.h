/*!
********************************************************************************
* @file         :   bms_adbms_debug_log.h
* @author       :   Srikanth Reddy Ramidi
* @brief        :   Prototypes for Link between ADBMS lib and Debug console
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef _SERIAL_MAXIM_H
#define _SERIAL_MAXIM_H

/* **** Includes **** */

#ifdef __cplusplus
extern "C" {
#endif

#include "bms_debug_log.h"

/**
 * @brief      Class for wrappers with naming same as Aurduino's
 */

class Serial_wrapper{
public:
	void begin(long baudRate = MAIN_UART_BAUDRATE);
	void print(const char* str);
	void print(uint32_t data, display_data type = DEC);
	void println(const char* str);
	void println(uint32_t data, display_data type = DEC);
};

const char* F(const char*);

extern Serial_wrapper Serial;


#ifdef __cplusplus
}
#endif


#endif /* _SERIAL_MAXIM_H */
