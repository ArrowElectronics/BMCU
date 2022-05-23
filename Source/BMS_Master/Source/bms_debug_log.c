/*!
********************************************************************************
* @file         :   bms_debug_log.c
* @author       :   Ashvin Ramani
* @brief        :   Debug console logic
*******************************************************************************/

/***** Includes *****/
#include "bms_debug_log.h"


//************************************************************************************************************
//
// Simple string print.
//
//************************************************************************************************************
int DebugStr(const uint8_t interface, const char * buffer)
{
	if( interface == MAIN_UART )
	{
		return ( UART_SendStr(interface, buffer, strlen(buffer)) );
	}
	else if( interface == MAIN_USB )
	{
		return ( USB_SendStr((uint8_t *)buffer, strlen(buffer)) );
	}
	else
	{
		return E_INVALID;
	}
}

int DebugStrLf(const uint8_t interface, const char * buffer)
{
	int result;

	result = DebugStr(interface, buffer);
	result &= DebugStr(interface, "\n");

	return result;
}

int DebugStrCr(const uint8_t interface, const char * buffer)
{
	int result;

    result = DebugStr(interface, buffer);
    result &= DebugStr(interface, "\r");

    return result;
}

//************************************************************************************************************
//
// Simple data print.
//
//************************************************************************************************************
int DebugData(const uint8_t interface, const char * buffer, const int32_t data, const display_data type)
{
	int result;

    result = DebugStr(interface, buffer);

    char txBuffer[16];

    /* TODO: Test and change with usnprintf() if required */

    switch (type)
    {
        case DEC:
            snprintf(txBuffer, 14, " %ld", data);
            break;
        case HEX:
        	snprintf(txBuffer, 14, " %02lx", data);
            break;
        case HEX16:
            snprintf(txBuffer, 14, " %04lx", data);
            break;
        case HEX32:
            snprintf(txBuffer, 14, " %08lx", data);
            break;
    }

    result &= DebugStr(interface, txBuffer);     // may NOT want \n here ...

    return result;
}


int DebugDataCr(const uint8_t interface, const char * buffer, const int32_t data, const display_data type)
{
	int result;

	result = DebugData(interface, buffer, data, type);
    result &= DebugStr(interface, "\r");

    return result;
}


int DebugDataLf(const uint8_t interface, const char * buffer, const int32_t data, const display_data type)
{
	int result;

	result = DebugData(interface, buffer, data, type);
    result &= DebugStrLf(interface, "");

    return result;
}

int DebugFloatData(const uint8_t interface, const char * buffer, const float data)
{
	int result;

    result = DebugStr(interface, buffer);

    char txBuffer[16];
    snprintf(txBuffer, 14, " %.4f", data);

    /* TODO: Test and change with usnprintf() if required */
    result &= DebugStr(interface, txBuffer);     // may NOT want \n here ...

    return result;
}


int DebugFloatDataCr(const uint8_t interface, const char * buffer, const float data)
{
	int result;

	result = DebugFloatData(interface, buffer, data);
    result &= DebugStr(interface, "\r");

    return result;
}


int DebugFloatDataLf(const uint8_t interface, const char * buffer, const float data)
{
	int result;

	result = DebugFloatData(interface, buffer, data);
    result &= DebugStrLf(interface, "");

    return result;
}
