/*!
********************************************************************************
* @file         :   bms_jumper.h
* @author       :   Ashvin Ramani
* @brief        :   Prototypes for the Jumper detection logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_JUMPER_H
#define BMS_JUMPER_H

/************************* Includes ***************************/

#include "../Drivers/bms_dura_frc_spi.h"
#include "bms_common.h"
#include "bms_gpio.h"

bool Jumper_Initialise( void );

#endif /* BMS_JUMPER_H */
