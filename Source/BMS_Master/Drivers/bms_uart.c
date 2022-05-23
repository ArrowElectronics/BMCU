/*!
********************************************************************************
* @file         :   bms_uart.c
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   UART module logic
*******************************************************************************/

/***** Includes *****/
#include "bms_uart.h"

/****************************** UART definitions ******************************/
static uart_cfg_t main_uart_cfg =
{
	.extra_stop = 0,
	.cts = 0,
	.rts = 0,
	.baud = MAIN_UART_BAUDRATE,
	.size = UART_DATA_SIZE_8_BITS,
	.parity = UART_PARITY_DISABLE,
};

static const sys_cfg_uart_t main_sys_cfg =
{
    .clk_scale = CLKMAN_SCALE_AUTO,
                        /* idx, io_map(TX/RX), cts_map, rts_map, io_en(Tx/Rx), cts_en, rts_en */
    .io_cfg = IOMAN_UART(MAIN_UART, IOMAN_MAP_A, IOMAN_MAP_UNUSED, IOMAN_MAP_UNUSED, 1, 0, 0),
};

bool UART_Configure(long baudRate)
{
    int result;
    bool RetValue = true;

    main_uart_cfg.baud = baudRate;

    result = UART_Init(MXC_UART_GET_UART(MAIN_UART), &main_uart_cfg, &main_sys_cfg);

	if ( E_NO_ERROR != result )
	{
		RetValue = false;
		//DebugDataLf( MAIN_USB, "UART_Configure error:", result, DEC);
	}

    return RetValue;
}

bool UART_Deconfigure(void)
{
    int result;
    bool RetValue = true;

    result = UART_Shutdown(MXC_UART_GET_UART(MAIN_UART));

	if ( E_NO_ERROR != result )
	{
		RetValue = false;
		//DebugDataLf( MAIN_USB, "UART_Configure error:", result, DEC);
	}

    return RetValue;
}

int UART_SendStr(uint8_t interface, const char * buffer, int len)
{
    return ( UART_Write(MXC_UART_GET_UART(interface), (uint8_t *)buffer, len) );
}

int UART_RecvStr(uint8_t interface, char * buffer, int len, int *num)
{
	return ( UART_Read(MXC_UART_GET_UART(interface), (uint8_t *)buffer, len, num) );
}

int UART_RecvStrAvl(uint8_t interface, char * buffer, int len, int num)
{
	return ( UART_Read_Avl(MXC_UART_GET_UART(interface), (uint8_t *)buffer, len, num) );
}
int UART_RecvStrAvailable(uint8_t interface, char * buffer, int len, int *num)
{
	return ( UART_Read_Available(MXC_UART_GET_UART(interface), (uint8_t *)buffer, len, num) );
}
