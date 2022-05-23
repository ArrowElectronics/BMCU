/*!
********************************************************************************
* @file         :   bms_dura_frc_spi.c
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   SPI module logic on dura-frc connector
*******************************************************************************/

/***** Includes *****/
#include "bms_debug_log.h"
#include "bms_dura_frc_spi.h"

#include "FreeRTOS.h"
#include "task.h"

const sys_cfg_spim_t iso_spi0 =
{
	    .clk_scale = CLKMAN_SCALE_AUTO,
			        /* core I/O, ss0, ss1, ss2,  ss3,  ss4, quad, fast I/O */
	    .io_cfg = IOMAN_SPIM0(1, 0, 0, 0, 0, 0, 0, 0)
};

const spim_cfg_t iso_spi0_spim_cfg = {3, SPIM_SSEL0_LOW, ADBMS_SPI_Speed};

static gpio_cfg_t frc_buffer_enable;

static gpio_cfg_t spi_dura_frc_cs[2][2] = {0};
static dura_frc_arduino_cs spi_dura_frc_ssel[2][2] = {{ISOSPI_FW, ISOSPI_RW}, {FRC_CONNECTOR_FW, FRC_CONNECTOR_RW}};

static dura_frc_interface spi_interface = ISOSPI;
static dura_frc_direction spi_direction = FORWARD;

bool spi_fault = false;

void Update_Dura_FRC_SPI_Interface(uint32_t button_state)
{
	if ( GPIO_HIGH_LEVEL == button_state )
	{
		spi_interface = ISOSPI;

		GPIO_OutSet( &frc_buffer_enable );
	}

	if ( GPIO_LOW_LEVEL == button_state )
	{
		spi_interface = FRC;

		GPIO_OutClr( &frc_buffer_enable );
	}
}

void Update_Dura_FRC_SPI_Direction(uint32_t direction)
{
	spi_direction = direction;
}

bool SPI_Dura_FRC_Configure( void )
{
	int result = 0;
	bool RetValue = true;

	result = SPIM_Init(Iso_SPI, &iso_spi0_spim_cfg, &iso_spi0);

	if( E_NO_ERROR == result )
	{
		spi_dura_frc_cs[0][0].port = DURA_SPI0_FW_SS0_PORT;
		spi_dura_frc_cs[0][0].mask = DURA_SPI0_FW_SS0_PIN;
		spi_dura_frc_cs[0][0].func = GPIO_FUNC_GPIO;
		spi_dura_frc_cs[0][0].pad = GPIO_PAD_NORMAL;
		GPIO_Config( &spi_dura_frc_cs[0][0] );
		GPIO_OutSet( &spi_dura_frc_cs[0][0] );

		spi_dura_frc_cs[0][1].port = DURA_SPI0_RW_SS1_PORT;
		spi_dura_frc_cs[0][1].mask = DURA_SPI0_RW_SS1_PIN;
		spi_dura_frc_cs[0][1].func = GPIO_FUNC_GPIO;
		spi_dura_frc_cs[0][1].pad = GPIO_PAD_NORMAL;
		GPIO_Config( &spi_dura_frc_cs[0][1] );
		GPIO_OutSet( &spi_dura_frc_cs[0][1] );

		spi_dura_frc_cs[1][0].port = FRC_SPI0_FW_SS2_PORT;
		spi_dura_frc_cs[1][0].mask = FRC_SPI0_FW_SS2_PIN;
		spi_dura_frc_cs[1][0].func = GPIO_FUNC_GPIO;
		spi_dura_frc_cs[1][0].pad = GPIO_PAD_NORMAL;
		GPIO_Config( &spi_dura_frc_cs[1][0] );
		GPIO_OutSet( &spi_dura_frc_cs[1][0] );

		spi_dura_frc_cs[1][1].port = FRC_SPI0_RW_SS3_PORT;
		spi_dura_frc_cs[1][1].mask = FRC_SPI0_RW_SS3_PIN;
		spi_dura_frc_cs[1][1].func = GPIO_FUNC_GPIO;
		spi_dura_frc_cs[1][1].pad = GPIO_PAD_NORMAL;
		GPIO_Config( &spi_dura_frc_cs[1][1] );
		GPIO_OutSet( &spi_dura_frc_cs[1][1] );

		frc_buffer_enable.port = FRC_SPI_EN_PORT;
		frc_buffer_enable.mask = FRC_SPI_EN_PIN;
		frc_buffer_enable.func = GPIO_FUNC_GPIO;
		frc_buffer_enable.pad = GPIO_PAD_NORMAL;
		GPIO_Config( &frc_buffer_enable );
		GPIO_OutSet( &frc_buffer_enable );
	}

	if ( E_NO_ERROR != result )
	{
		RetValue = false;
		//DebugDataLf( DEBUG_INTERFACE, "Error initializing SPIM:", error,DEC);
	}

	return RetValue;
}

bool SPI_Dura_FRC_Deconfigure(void)
{
	int result = 0;
	bool RetValue = true;

	result = SPIM_Shutdown(Iso_SPI);

	if( E_NO_ERROR  != result)
	{
		RetValue = false;
		//DebugStrLf( DEBUG_INTERFACE, "Error in De-configuring SPIM...");
	}

	return RetValue;
}

int spi_write_array(uint8_t tx_len, const uint8_t *tx_buf)
{
	spim_req_t req;

	req.ssel = spi_dura_frc_ssel[spi_interface][spi_direction];
	req.rx_data = NULL;
	req.width = SPIM_WIDTH_1;
	req.len = tx_len;
	req.tx_data = tx_buf;

	if ( SPIM_Trans(Iso_SPI, &req) != tx_len )
	{
		return E_COMM_ERR;
	}

	/* Wait for transaction to complete */
	while( SPIM_Busy(Iso_SPI) != E_NO_ERROR )
	{
        /* Wait for transaction to complete */
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
int spi_write_read(const uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len)
{
	spim_req_t req;

	if ( NULL != tx_buf )
	{
		/* Send the command or data */
		req.ssel     = spi_dura_frc_ssel[spi_interface][spi_direction];
		req.len      = tx_len;
		req.rx_data  = NULL;
		req.width    = SPIM_WIDTH_1;
		req.tx_data  = tx_buf;

		if ( SPIM_Trans(Iso_SPI, &req) != tx_len )
		{
			return E_COMM_ERR;
		}
	}

	/* Read data */
	req.ssel     = spi_dura_frc_ssel[spi_interface][spi_direction];
	req.tx_data  = NULL;
	req.len      = rx_len;
	req.width    = SPIM_WIDTH_1;
	req.rx_data  = rx_buf;

	if ( SPIM_Trans(Iso_SPI, &req) != rx_len )
	{
		return E_COMM_ERR;
	}

	return E_NO_ERROR;
}

uint8_t spi_read_byte(uint8_t rx_data)
{
	uint8_t ReadByte = -1;

	(void)spi_write_read(NULL, 0, &ReadByte, 1);

    return ReadByte;
}

void cs_low(uint8_t pin)
{
	(void)pin; /* Pin not used but keeping here to satisfy ADBMS Lib API's*/
    GPIO_OutClr( &spi_dura_frc_cs[spi_interface][spi_direction] );
}

void cs_high(uint8_t pin)
{
	(void)pin; /* Pin not used but keeping here to satisfy ADBMS Lib API's*/
	GPIO_OutSet( &spi_dura_frc_cs[spi_interface][spi_direction] );
}
