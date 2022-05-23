/*!
********************************************************************************
* @file         :   bms_arduino_spi.c
* @author       :   Ashvin Ramani
* @brief        :   SPI module logic on Arduino connector
*******************************************************************************/

/***** Includes *****/
#include "bms_debug_log.h"
#include "bms_arduino_spi.h"

const sys_cfg_spim_t arduino_sys_cfg = {
        .clk_scale = CLKMAN_SCALE_AUTO,
		                    /* core I/O, ss0, ss1, ss2,  sr0,  sr1, quad, fast I/O */
		/* .io_cfg = IOMAN_SPIM2(1, 0, 0, 0, 0, 0, 0, 0)  without chip select */
         .io_cfg = IOMAN_SPIM2(1, 1, 0, 0, 0, 0, 0, 0)  /* with chip select */
};

const spim_cfg_t arduino_spim_cfg = {0, SPIM_SSEL0_LOW, Arduino_SPI_Speed};

bool SPI_Arduino_Configure( void )
{
	int result = 0;
	bool RetValue = true;

	result = SPIM_Init(ARDUINO_SPI, &arduino_spim_cfg, &arduino_sys_cfg);

	if ( E_NO_ERROR != result )
	{
		RetValue = false;
		//DebugDataLf( DEBUG_INTERFACE, "Error while initializing SPIM on Arduino:", result, DEC);
	}

	return RetValue;
}

bool SPI_Arduino_Deconfigure(void)
{
	int result = 0;
	bool RetValue = true;

	result = SPIM_Shutdown(ARDUINO_SPI);

	if( E_NO_ERROR != result)
	{
		RetValue = false;
		//DebugDataLf( DEBUG_INTERFACE, "Error while de-configuring SPIM on Arduino:", result, DEC);
	}

	return RetValue;
}

int spi_arduino_write_array(uint8_t tx_len, const uint8_t *tx_buf)
{
	spim_req_t req;

	req.ssel = 0U;
	req.rx_data = NULL;
	req.width = SPIM_WIDTH_1;
	req.len = tx_len;
	req.tx_data = tx_buf;

	if ( SPIM_Trans(ARDUINO_SPI, &req) != tx_len )
	{
		return E_COMM_ERR;
	}

	/* wait for transaction to complete */
	while( SPIM_Busy(ARDUINO_SPI) != E_NO_ERROR )
	{
		/* wait for transaction to complete */
	}

    return E_NO_ERROR;
}

/*
 * @brief : Reads data over SPI
 * @param1 : tx_buf Data to be transmitted
 * @param2 : tx_len len of the transmit data
 * @param3 : rx_buf Data to be received
 * @param2 : rx_len len of the receive data
 */
int spi_arduino_write_read(const uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len)
{
	spim_req_t req;

	if ( NULL != tx_buf )
	{
		/* Send the command or data */
		req.ssel     = 0U;
		req.len      = tx_len;
		req.rx_data  = NULL;
		req.width    = SPIM_WIDTH_1;
		req.tx_data  = tx_buf;

		if ( SPIM_Trans(ARDUINO_SPI, &req) != tx_len )
		{
			return E_COMM_ERR;
		}
	}

	/* Read data */
	req.ssel     = 0U;
	req.tx_data  = NULL;
	req.len      = rx_len;
	req.width    = SPIM_WIDTH_1;
	req.rx_data  = rx_buf;

	if ( SPIM_Trans(ARDUINO_SPI, &req) != rx_len )
	{
		return E_COMM_ERR;
	}

	return E_NO_ERROR;
}
