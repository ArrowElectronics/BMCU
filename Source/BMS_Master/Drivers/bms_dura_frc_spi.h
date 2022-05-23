/*!
********************************************************************************
* @file         :   bms_dura_frc_spi.h
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   Prototypes for SPI module logic on dura-frc connector
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_DURA_FRC_SPI_H
#define BMS_DURA_FRC_SPI_H

#include <stdbool.h>
#include "gpio.h"
#include "tmr_utils.h"
#include "bms_gpio.h"

/* SPI0 configuration: */
#define Iso_SPI               MXC_SPIM0

/* Clock Speed */
#define ADBMS_SPI_Speed       1000000 //500000 /* 1Mhz */

#define CELL_CHANNELS   18

extern bool spi_fault; /* This flag is for spi fault */

typedef enum  {
	ISOSPI_FW,
	ISOSPI_RW,
	FRC_CONNECTOR_FW,
	FRC_CONNECTOR_RW,
	ISOSPI_ALL,
	FRC_ALL,
	ISOSPI_FRC_All,
} dura_frc_arduino_cs;

typedef enum  {
	FORWARD = 0U,
	REVERSE,
} dura_frc_direction;

typedef enum  {
	ISOSPI = 0U,
	FRC,
} dura_frc_interface;

/*
 *@brief: To initiate the SPI port
*/
bool SPI_Dura_FRC_Configure ( void );

/*
 *@brief: To close the SPI port
*/
bool SPI_Dura_FRC_Deconfigure( void );

/*
 *@brief: To clear SPI CS pin
*/
void cs_low( uint8_t pin );

/*
 *@brief: To set SPI CS pin
*/
void cs_high( uint8_t pin );

/*
 * @brief: Writes an array of bytes out of the SPI port
*/
int spi_write_array(uint8_t tx_len, // Option: Number of bytes to be written on the SPI port
		            const  uint8_t *tx_buf //Array of bytes to be written on the SPI port
                    );
/*
 * @brief:  Writes and read a set number of bytes using the SPI port.
*/

int spi_write_read(const  uint8_t *tx_buf,//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data array
                    uint8_t *rx_buf,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
                   );

uint8_t spi_read_byte(uint8_t rx_data);//name conflicts with linduino also needs to take a byte as a parameter

void Update_Dura_FRC_SPI_Interface(uint32_t button_state);
void Update_Dura_FRC_SPI_Direction(uint32_t direction);

#endif /* BMS_DURA_FRC_SPI_H */
