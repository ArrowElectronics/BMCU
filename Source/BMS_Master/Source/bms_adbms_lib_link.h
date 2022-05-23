/*!
********************************************************************************
* @file         :   bms_adbms_lib_link.h
* @author       :   Srikanth Reddy Ramidi
* @brief        :   Prototypes for Link between ADBMS lib and application
*******************************************************************************/

/************************* Includes ***************************/
/* FreeRTOS */
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
#define EXTERNC extern "C" /* Exposing C++ functions as C functions for linking with C functions */
#else
#define EXTERNC            /* Nothing to do before function declaration for using those as C++ functions */
#endif


/*
 * @brief: Helper function for writing command and reading it's response data from ADBMS
 */
EXTERNC int8_t Send_Command_Read_Data(const uint8_t *cmd, uint8_t cmd_size, uint8_t *data, uint8_t data_len, uint8_t totalIC);

/*
 * @brief: Helper function for writing command to from ADBMS
 */
EXTERNC int8_t Send_Command(const uint8_t *cmd, uint8_t cmd_size, uint8_t totalIC);

/*
 * @brief: Wrapper function for calculating CRC15 of ADBMS command
 */
EXTERNC uint16_t Calculate_CRC(uint8_t len, uint8_t *data );

/*
 * @brief: Wrapper function for polling ADC command
 */
EXTERNC uint32_t ADBMS1818_PollADC( );

EXTERNC void Wakeup_Idle(uint8_t total_ic);

EXTERNC void Wakeup_Sleep(uint8_t total_ic);

EXTERNC void Get_ADBMS1818_Cell_OpenWires(uint8_t total_ic, int8_t opencells[][18], int8_t *num_of_open_wires );

