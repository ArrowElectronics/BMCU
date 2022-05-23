/*!
********************************************************************************
* @file         :   bms_arduino_spi.h
* @author       :   Ashvin Ramani
* @brief        :   Prototypes for SPI module logic on Arduino connector
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_ARDUINO_SPI_H
#define BMS_ARDUINO_SPI_H

#include <stdbool.h>
#include "gpio.h"
#include "tmr_utils.h"
#include "bms_gpio.h"

/* SPI2 configuration: */
#define ARDUINO_SPI           MXC_SPIM2

/* Clock Speed */
#define Arduino_SPI_Speed     1000000 /* 1Mhz */

/*
 *@brief: To initiate the SPI port
*/
bool SPI_Arduino_Configure ( void );

/*
 *@brief: To close the SPI port
*/
bool SPI_Arduino_Deconfigure( void );

/*
 * @brief: Writes an array of bytes out of the SPI port
*/
int spi_arduino_write_array(uint8_t tx_len, // Option: Number of bytes to be written on the SPI port
		                    const  uint8_t *tx_buf //Array of bytes to be written on the SPI port
                           );
/*
 * @brief:  Writes and read a set number of bytes using the SPI port.
*/

int spi_arduino_write_read(const uint8_t *tx_buf,//array of data to be written on SPI port
                    uint8_t tx_len, //length of the tx data array
                    uint8_t *rx_buf,//Input: array that will store the data read by the SPI port
                    uint8_t rx_len //Option: number of bytes to be read from the SPI port
                   );

#endif /* BMS_ARDUINO_SPI_H */
