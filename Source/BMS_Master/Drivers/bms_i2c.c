/*!
********************************************************************************
* @file         :   bms_i2c.c
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   I2C module logic
*******************************************************************************/

/***** Includes *****/
#include "bms_i2c.h"

const sys_cfg_i2cm_t i2cm_sys_cfg =
{
	    .clk_scale = CLKMAN_SCALE_DIV_1,
	    .io_cfg = IOMAN_I2CM(I2C_MASTER_IDX, 1, 0)
};

bool I2C_Configure ( void )
{
	int result;
	bool RetValue = true;

    result = I2CM_Init(I2C_MASTER, &i2cm_sys_cfg, I2C_SPEED);

    if ( E_NO_ERROR != result )
    {
    	RetValue = false;
		//DebugDataLf( MAIN_USB, "I2C_Configure error:", result, DATA_DEC);
	}

    return RetValue;
}

bool I2C_Deconfigure ( void )
{
	int result = 0;
	bool RetValue = true;

	result = I2CM_Shutdown(I2C_MASTER);

	if( E_NO_ERROR  != result )
	{
		RetValue = false;
		//DebugDataLf( MAIN_USB, "I2C_Deconfigure error:", result, DEC);
	}

	return RetValue;
}
