/*!
********************************************************************************
* @file         :   bms_eeprom.h
* @author       :   Ashvin Ramani
* @brief        :   Prototypes for On Board EEPROM read/write logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_EEPROM_H
#define BMS_EEPROM_H

#include <stdbool.h>

#include "i2cm.h"        /* To access I2C definitions */
#include "bms_debug_log.h"
#include "bms_i2c.h"

#define I2C_SLAVE_EEPROM_ADDR 0x50              /* 7-bit address[7:1] */


/* **** Function Prototypes **** */

/**
 * @brief      To configure I2C Interface
 */
int32_t EEPROM_Write_Data(const uint8_t *cmd, uint32_t cmd_len, uint8_t *data, uint32_t data_len);

/**
 * @brief      To Deconfigure I2C Interface
 */
int32_t EEPROM_Read_Data(const uint8_t *cmd, uint32_t cmd_len, uint8_t *data, uint32_t data_len);

#endif /* BMS_EEPROM_H */
