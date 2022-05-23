/*!
********************************************************************************
* @file         :   bms_eeprom.c
* @author       :   Ashvin Ramani
* @brief        :   On Board EEPROM read/write logic
*******************************************************************************/

/***** Includes *****/
#include "bms_eeprom.h"

/***** Definitions *****/

/***** Globals *****/

/***** Functions *****/

/*****************************************************************************/

int32_t EEPROM_Write_Data(const uint8_t *cmd, uint32_t cmd_len, uint8_t *data, uint32_t data_len)
{
	int32_t Result;

	Result = I2CM_Write(I2C_MASTER, I2C_SLAVE_EEPROM_ADDR, cmd, cmd_len, data, data_len);

    if ( data_len != Result)
    {
    	//DebugDataLf( MAIN_USB, "I2C_Write error:", Result, DEC);
    }

    return Result;
}

int32_t EEPROM_Read_Data(const uint8_t *cmd, uint32_t cmd_len, uint8_t *data, uint32_t data_len)
{
	int32_t Result;

	Result = I2CM_Read(I2C_MASTER, I2C_SLAVE_EEPROM_ADDR, cmd, cmd_len, data, data_len);

    if ( data_len != Result)
    {
    	//DebugDataLf( MAIN_USB, "I2C_Read error:", Result, DEC);
    }

    return Result;
}
