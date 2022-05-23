/*!
********************************************************************************
* @file         :   bms_adbms_debug_log.cpp
* @author       :   Srikanth Reddy Ramidi
* @brief        :   Link between ADBMS lib and Debug console
*******************************************************************************/

/***** Includes *****/
#include "bms_adbms_debug_log.h"

/**
 * @brief : Configure Serial interface for communication
 * @baudRate : The clock rate for UART
 */
void Serial_wrapper::begin(long baudRate)
{
	bool RetValue;

	RetValue = UART_Configure(baudRate);

	if ( true != RetValue )
	{
		//DebugDataLf( MAIN_USB, "Serial.begin : UART_Configure error:", error, DEC);
	}

}

/**
 * @brief : Prints string on the serial interface
 * @buffer : The string to print
 */
void Serial_wrapper::print(const char* buffer)
{
	DebugStr(DEBUG_INTERFACE, buffer);
}

/**
 * @brief : Prints the given data in given format on the serial interface
 * @data : Data to print
 * @type : Format of the data
 */
void Serial_wrapper::print(uint32_t data, display_data type)
{
	DebugData(DEBUG_INTERFACE, "", data,  type);
}

/**
 * @brief : Prints string on the serial interface with Line feed
 * @buffer : The string to print
 */
void Serial_wrapper::println(const char* buffer)
{
	DebugStrLf(DEBUG_INTERFACE, buffer);
}

/**
 * @brief : Prints the given data in given format on the serial interface with Line feed
 * @data : Data to print
 * @type : Format of the data
 */
void Serial_wrapper::println(uint32_t data, display_data type)
{
	DebugDataLf(DEBUG_INTERFACE, "", data,  type);
}

/**
 * @brief : This is a dummy function returns the provided string(argument)
 * @str : String data
 */
const char* F(const char *str)
{
	return str;
}


