/*!
********************************************************************************
* @file         :   bms_can.c
* @author       :   Ashvin Ramani
* @brief        :   CAN-SPI module logic
*******************************************************************************/

/* Most of this code are derived from Microchip MCP2518FD SDK */
#include "bms_can.h"

/*************** CAN Test functions **********/
#if CAN_TEST_ENABLED
static int8_t can_send_message(void);
static int8_t can_receive_message(void);
static bool can_compare_messages(void);
#endif

/************ CAN Test variables ***********/

const unsigned char send_msg[8] = {0, 1, 2, 3, 4, 5, 6, 7};
unsigned char recv_msg[8];

/******************************************/
gpio_cfg_t CANSPI_CS_gpio;
mxc_spim_regs_t *CAN_SPI = MXC_SPIM1;
/*********************************************************************************************************
** Function name:           init_CS
** Descriptions:            init CS pin and set UNSELECTED
*********************************************************************************************************/
void init_CAN_SPI( void )
{
	int error = 0;

	// Initialize the SPIM
	spim_cfg_t cfg;
	cfg.mode = 0;
	cfg.ssel_pol = SPIM_SSEL0_LOW;
	cfg.baud = SPI_CLOCK;

	sys_cfg_spim_t sys_cfg;

	// MX25 IO Config                  core I/O, ss0, ss1, ss2, quad, fast I/O
	sys_cfg.io_cfg = (ioman_cfg_t )IOMAN_SPIM1(1,   0,  0,    0,    0,        0);
	sys_cfg.clk_scale = CLKMAN_SCALE_AUTO;

	if((error = SPIM_Init(CAN_SPI, &cfg, &sys_cfg)) != E_NO_ERROR)
	{
		DebugDataLf( MAIN_USB, "Error initializing SPIM ", error, HEX);
	}
	else
	{
		DebugStrLf( MAIN_USB, "SPIM Initialized");
	}

	CANSPI_CS_gpio.port = CAN_SPI1_SS0_PORT;
	CANSPI_CS_gpio.mask = CAN_SPI1_SS0_PIN;
	CANSPI_CS_gpio.func = GPIO_FUNC_GPIO;
	CANSPI_CS_gpio.pad = GPIO_PAD_NORMAL;

	GPIO_Config(&CANSPI_CS_gpio);

	GPIO_OutSet(&CANSPI_CS_gpio);
}

void CAN_SPI_End( void )
{
	(void)SPIM_Shutdown(CAN_SPI);
}

int CAN_SPI_Array_Write(uint8_t tx_len, const uint8_t *tx_buf)
{
	spim_req_t req;

	req.ssel = CAN_SPI_SSEL;
	req.rx_data = NULL;
	req.width = SPIM_WIDTH_1;
	req.len = tx_len;
	req.tx_data = tx_buf;
	//req.deass = 1;

	if ( SPIM_Trans(CAN_SPI, &req) != tx_len ) //SPI_TX_RX_SIZE
	{
		return E_COMM_ERR;
	}

	// Wait for transaction to complete
	while( SPIM_Busy(CAN_SPI) != E_NO_ERROR )
	{
		//wait for transaction to complete
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
int CAN_SPI_Read(const uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len)
{
	spim_req_t req;

	if ( NULL != tx_buf )
	{
		// Send the command or data
		req.ssel     = CAN_SPI_SSEL;
		req.len      = tx_len;
		req.rx_data  = NULL;
		req.width    = SPIM_WIDTH_1;
		req.tx_data  = tx_buf;
		//req.deass    = 0;

		if ( SPIM_Trans(CAN_SPI, &req) != tx_len )
		{
			return E_COMM_ERR;
		}
	}

	// Read data
	req.ssel     = CAN_SPI_SSEL;
	req.tx_data  = NULL;
	req.len      = rx_len;
	req.width    = SPIM_WIDTH_1;
	req.rx_data  = rx_buf;
	//req.deass    = 1;

	if ( SPIM_Trans(CAN_SPI, &req) != rx_len )
	{
		return E_COMM_ERR;
	}

	return E_NO_ERROR;
}

void CAN_SPI_CS_LOW (void)
{
	GPIO_OutClr(&CANSPI_CS_gpio);
}

void CAN_SPI_CS_HIGH (void)
{
	GPIO_OutSet(&CANSPI_CS_gpio);
}

bool CAN_Configure( void )
{
	int8_t ret;
	bool RetValue = true;
	gpio_cfg_t CAN_SLEEP_gpio;

	CAN_SLEEP_gpio.port = CAN_TRANSCEIVER_SILENT_PORT;
	CAN_SLEEP_gpio.mask = CAN_TRANSCEIVER_SILENT_PIN;
	CAN_SLEEP_gpio.func = GPIO_FUNC_GPIO;
	CAN_SLEEP_gpio.pad = GPIO_PAD_NORMAL;

	GPIO_Config(&CAN_SLEEP_gpio);

	// CAN SLEEP should be active low as per datasheet/schematic to enable CAN transceiver
	GPIO_OutClr(&CAN_SLEEP_gpio);

    ret = begin(CANFD_500KBPS, MCP2518FD_20MHz);

    //CAN Controller is by default in standby mode, but for communication we need to disable it
	disable_2518fd_standyby_mode();

	if ( ret != 0 )
	{
		RetValue = false;
	}

	return RetValue;
}

#if CAN_TEST_ENABLED
static int8_t can_send_message( void )
{
	int8_t ret, len=8, id=0x01, ext=0;

	ret = sendMsgBuf(id, ext, len, send_msg);       // send a standard frame to id 0x01

	if(ret < 0)
	{
		DebugStrLf( MAIN_USB,"CAN BUS sendMsgBuf failed!");
	}
	else
	{
		DebugStrLf( MAIN_USB,"CAN BUS sendMsgBuf ok!");
	}

	DebugStrLf( MAIN_USB,"Message sent:");

	if(ext)
	{
		DebugStr( MAIN_USB, "SENT EXTENDED FRAME TO ID: ");
	}
	else
	{
		DebugStr( MAIN_USB, "SENT STANDARD FRAME TO ID: ");
	}

	return ret;
}

static int8_t can_receive_message( void )
{
	unsigned char len = 0;

	while (CAN_MSGAVAIL != checkReceive() )         // // check if data coming
	{
		Delay_m(100);
	}

	readMsgBuf(&len, recv_msg);            // You should call readMsgBuff before getCanId
	//unsigned long id = getCanId();
	unsigned char ext = isExtendedFrame();

	DebugStrLf( MAIN_USB, "Message received with the following data...");

	if(ext)
	{
		DebugStr( MAIN_USB, "GET EXTENDED FRAME FROM ID: ");
	}
	else
	{
		DebugStr( MAIN_USB, "GET STANDARD FRAME FROM ID: ");
	}
	return len;
}

static bool can_compare_messages( void )
{
	for(int8_t index=0;index<8;index++)
	{
		if( send_msg[index] != recv_msg[index])
		{
			return false;
		}
	}

	return true;
}

void CAN_Test( void )
{

	int8_t ret;
	bool is_matched = false;

	ret = can_send_message();

	if(ret < 0)
	{
		DebugStrLf( MAIN_USB, "main: CAN BUS sendMsgBuf failed!");
	}
	else
	{
		can_receive_message();

		is_matched = can_compare_messages();

		if(is_matched == true)
		{
			DebugStrLf( MAIN_USB, "CAN Test successful, sent and received data matched\n");
		}
		else
		{
			DebugStrLf( MAIN_USB, "CAN Test failed, sent and received data didn't matched\n");
		}
	}
}
#endif
