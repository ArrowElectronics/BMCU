/*!
********************************************************************************
* @file         :   bms_uart.h
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   Prototypes for UART module logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_UART_H
#define BMS_UART_H

/* **** Includes **** */
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "uart.h"


/* **** Definitions **** */
/****************************** UART definitions ******************************/
#define MAIN_UART             0         /* UART instance */
#define MAIN_UART_BAUDRATE    115200    /* UART BaudRate */

/**
 * @brief      To receive data from UART Interface
 */
bool UART_Configure(long);
bool UART_Deconfigure(void);
int UART_RecvStr(uint8_t interface, char * buffer, int len, int *num);
int UART_RecvStrAvl(uint8_t interface, char * buffer, int len, int num);
int UART_SendStr(uint8_t interface, const char * buffer, int len);

#endif /* BMS_UART_H */
