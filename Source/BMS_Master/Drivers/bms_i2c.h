/*!
********************************************************************************
* @file         :   bms_i2c.h
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   Prototypes for I2C module logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_I2C_H
#define BMS_I2C_H

#include <stdbool.h>

#include "i2cm.h"        /* To access I2C definitions */
#include "bms_debug_log.h"

#define I2C_MASTER            MXC_I2CM0
#define I2C_MASTER_IDX        0
//#define I2C_SLAVE_IIS2DH_ADDR 0x18              /* 7-bit address[7:1] */
//#define I2C_SLAVE_EEPROM_ADDR 0x50              /* 7-bit address[7:1] */
#define I2C_SPEED             I2CM_SPEED_100KHZ   /* I2CM_SPEED_400KHZ */

/* **** Function Prototypes **** */

/**
 * @brief      To configure I2C Interface
 */
bool I2C_Configure(void);

/**
 * @brief      To Deconfigure I2C Interface
 */
bool I2C_Deconfigure(void);

#endif /* BMS_I2C_H */
