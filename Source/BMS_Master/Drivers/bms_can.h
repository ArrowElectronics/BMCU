/*!
********************************************************************************
* @file         :   bms_can.h
* @author       :   Ashvin Ramani
* @brief        :   Prototypes for CAN-SPI module logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_CAN_H_
#define BMS_CAN_H_

#include <stdio.h>
#include <stdbool.h>
#include "bms_gpio.h"
#include "bms_debug_log.h"
#include "bms_delay.h"
#include "MCP2518FD.h"

#define CAN_TEST_ENABLED 0

uint8_t SPICS_Pin, SPICS_Port;

#define	CAN_SPI_SSEL			1
#define SPI_CLOCK				4000000 /* 4MHz */

void init_CAN_SPI( void ); /* define CS after construction before begin() */
int CAN_SPI_Array_Write(uint8_t tx_len, const uint8_t *tx_buf);
int CAN_SPI_Read(const uint8_t *tx_buf, uint8_t tx_len, uint8_t *rx_buf, uint8_t rx_len);
void CAN_SPI_End( void );
void CAN_SPI_CS_LOW (void);
void CAN_SPI_CS_HIGH (void);
bool CAN_Configure( void );

#endif /* BMS_CAN_H_ */
