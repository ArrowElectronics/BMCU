/*!
********************************************************************************
* @file         :   bms_common.h
* @author       :   Srikanth Reddy Ramidi, Ashvin Ramani
* @brief        :   Common Prototypes for various module
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_COMMON_H
#define BMS_COMMON_H

/************************* Includes ***************************/
#include<stdio.h>
#include<stdbool.h>

#include "bms_adbms_lib_link.h"
#include "bms_debug_log.h"
#include "bms_delay.h"
#include "FreeRTOS.h"		/* RTOS firmware */
#include "task.h"			/* Task */
#include "queue.h"			/* Queue */
#include "semphr.h"			/* Semaphore */
#include "event_groups.h"   /* Event groups */

/* Maximum ICs assumed for this protocol*/
#define MAX_SLAVE_IC_COUNT							128 //120

/* ICs number per byte for this protocol*/
#define IC_NUMBERS_PER_BYTE 8

/* Maximum required IC Number bytes for this protocol*/
#define MAX_SLAVE_IC_NUMBER_BYTES					MAX_SLAVE_IC_COUNT/IC_NUMBERS_PER_BYTE

/* Response codes as per GUI protocol document */
typedef enum Response_code
{
	COMMAND_ACCEPTED = 0x01,
	UNRECOGNISED_SOF,
	INVALID_MESSAGE_LENGTH,
	INVALID_MESSAGE_TYPE,
	INVALID_COMMAND_LENGTH,
	UNRECOGNISED_OPCODE,
	INVALID_COMMAND_TYPE,
	INVALID_NUMBER_OF_ICS,
	INVALID_OR_MISMATCHED_IC_NUMBER_WITH_IC_COUNT,
	INVALID_OR_MISMATCHED_IC_TYPE_WITH_IC_COUNT,
	INVALID_OPERATION_TYPE,
	INVALID_COMMAND_DATA_LENGTH,
	REQUESTED_IC_COUNT_NOT_DETECTED,
	CONFIGURATION_NOT_YET_DONE,
	PREVIOUS_COMMAND_IN_PROGRESS,
	CHECKSUM_MISMATCHED,
	ADBMS1818_CRC_MISMATCHED,
	SPI_ISOSPI_COMM_ERROR,
	GUI_NOT_CONNECTED,

} Gui_Response_Code;

/* OP codes as per GUI protocol document */
/* 0x00:         Reserved
 * 0x01 to 0x0A: Special Commands,
 * 0x0B to 0x0C: RAW Commands,
 * 0x0D to 0xFF: Reserved.
 */
enum OpCode
{
    CONNECT       = 0x01,
    DISCONNECT,
    CONFIGURATION,
    FAULT_MESSAGE,
    GUI_START,
    READ          = 0x0B,
    WRITE,
};

/* OP Types as per GUI protocol document */
enum OP_Types
{
	ONE_SHOT = 0x01,
	CONTINUOUS,
	STOP,
};

/* System config structure to hold global system configuration */
struct systemConfig
{
	uint8_t IC_Count;			// Total number of ICs connected
	uint8_t IC_Number[MAX_SLAVE_IC_NUMBER_BYTES];			// Numbers assigned for connected ICs
	uint8_t IC_Types[MAX_SLAVE_IC_COUNT];		// Types assigned for connected ICs
	bool Is_Config_Set;		// Flag for configuration setting--> true (system configured) false(system not configured)
	uint16_t cell_over_voltage_threshold;
	uint16_t cell_under_voltage_threshold;
};

/* Application layer packet */
typedef struct messagePayload
{
	uint16_t CL;            // Command Length
	uint8_t Op_Code;			//Operation code like Configuration, read, write
	uint8_t IC_Count;		// Total ICs count on which operation need to be made
	uint8_t IC_Number[MAX_SLAVE_IC_NUMBER_BYTES];     // IC Number like 1,2....
	uint8_t IC_Types[MAX_SLAVE_IC_COUNT];   // IC Type like ADBMS1818,ADBMS1816
	uint8_t OP_Type;		// Operation type like One shot, continues, stop
	uint8_t DL;             // Data Length
	uint8_t Data[132];		// Data is the actual payload which is meant for ADBMS1818/6
	uint16_t Size;			// Received messagePayload size
	uint8_t Response_Status;// Status code for response packet
} application_layer_packet;


/* Size of each response */
#define RESPONSE_SIZE								8

/* Command and response constants for GUI start command */
#define TOTAL_GUI_START_VOL_READ_CMDS				8
#define VOL_READ_CMD_SIZE							4
#define VOL_READ_IC_RESPONSE_SIZE					48
#define STAT_VOL_READ_RESPONSE_SIZE					16
#define AUX_VOL_READ_RESPONSE_SIZE					32

#define CELL_VOL_DATA_INDICATOR						0x01
#define	AUX_VOL_DATA_INDICATOR						0x02
#define STAT_VOL_DATA_INDICATOR						0x04

#define TOTAL_START_VOL_RESPONSE_SIZE_EACH_IC		102

#define CONFIGURATION_DATA_SIZE						7

/* ADBMS1818 Command size with CRC */
#define ADBMS1818_CMD_SIZE							4

/* Fault Payload related constants */

#define FAULT_PAYLOAD_SIZE							40
#define OV_UV_C1_TO_C4_IN_FAULT_PAYLOAD				0
#define OV_UV_C5_TO_C8_IN_FAULT_PAYLOAD				1
#define OV_UV_C9_TO_C12_IN_FAULT_PAYLOAD			2
#define OV_UV_C13_TO_C16_IN_FAULT_PAYLOAD			3
#define OV_UV_C17_TO_C18_IN_FAULT_PAYLOAD			4
#define OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD				8
#define OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD				9
#define OV_UV_G9_IN_FAULT_PAYLOAD					10
#define OV_UV_VA_VD_STK_DIET_IN_FAULT_PAYLOAD		16
#define OW_C1_TO_C8_IN_FAULT_PAYLOAD				24
#define OW_C9_TO_C16_IN_FAULT_PAYLOAD				25
#define OW_C17_C18_IN_FAULT_PAYLOAD					26
#define AFE_COMM_SPI_FAIL_BYTE_IN_FAULT_PAYLOAD		32

#define VA_UV_BIT_MASK_IN_FAULT_PAYLOAD			1 << 0
#define VA_OV_BIT_MASK_IN_FAULT_PAYLOAD			1 << 1
#define VD_UV_BIT_MASK_IN_FAULT_PAYLOAD			1 << 2
#define VD_OV_BIT_MASK_IN_FAULT_PAYLOAD			1 << 3
#define STK_UV_BIT_MASK_IN_FAULT_PAYLOAD		1 << 4
#define STK_OV_BIT_MASK_IN_FAULT_PAYLOAD		1 << 5
#define DIE_UT_BIT_MASK_IN_FAULT_PAYLOAD		1 << 6
#define DIE_OT_BIT_MASK_IN_FAULT_PAYLOAD		1 << 7

#define SPI_FAIL_BIT_MASK_IN_FAULT_PAYLOAD			1 << 0
#define AFE_COMM_FAIL_BIT_MASK_IN_FAULT_PAYLOAD		1 << 1

#define VA_OV_THRESHOLD								55000 //5.5V
#define VA_UV_THRESHOLD								45000 //4.5V

#define VD_OV_THRESHOLD								36000 //3.6V
#define VD_UV_THRESHOLD								27000 //2.7V

#define GPIO_OV_THRESHOLD							50000 //5V
#define GPIO_UV_THRESHOLD							0 //0V

#define DIE_UT_THRESHOLD							-40 // C
#define DIE_OT_THRESHOLD							+80 // C

#define OV_UV_C1_TO_C4_IN_REG_STATB					2
#define OV_UV_C5_TO_C8_IN_REG_STATB					3
#define OV_UV_C9_TO_C12_IN_REG_STATB				4
#define OV_UV_C13_TO_C16_IN_REG_AUXD				4
#define OV_UV_C17_TO_C18_IN_REG_AUXD				5

#define GPIO1_LSB_IN_REG_AUXA						0
#define GPIO1_MSB_IN_REG_AUXA						1
#define GPIO2_LSB_IN_REG_AUXA						2
#define GPIO2_MSB_IN_REG_AUXA						3
#define GPIO3_LSB_IN_REG_AUXA						4
#define GPIO3_MSB_IN_REG_AUXA						5
#define GPIO4_LSB_IN_REG_AUXB						0
#define GPIO4_MSB_IN_REG_AUXB						1
#define GPIO5_LSB_IN_REG_AUXB						2
#define GPIO5_MSB_IN_REG_AUXB						3
#define GPIO6_LSB_IN_REG_AUXC						0
#define GPIO6_MSB_IN_REG_AUXC						1
#define GPIO7_LSB_IN_REG_AUXC						2
#define GPIO7_MSB_IN_REG_AUXC						3
#define GPIO8_LSB_IN_REG_AUXC						4
#define GPIO8_MSB_IN_REG_AUXC						5
#define GPIO9_LSB_IN_REG_AUXD						0
#define GPIO9_MSB_IN_REG_AUXD						1

#define GPIO1_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 0
#define GPIO1_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 1
#define GPIO2_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 2
#define GPIO2_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 3
#define GPIO3_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 4
#define GPIO3_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 5
#define GPIO4_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 6
#define GPIO4_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 7
#define GPIO5_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 0
#define GPIO5_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 1
#define GPIO6_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 2
#define GPIO6_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 3
#define GPIO7_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 4
#define GPIO7_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 5
#define GPIO8_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 6
#define GPIO8_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 7
#define GPIO9_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 0
#define GPIO9_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD		1 << 1

#define VREGD_LSB_IN_REG_STATB						0
#define VREGD_MSB_IN_REG_STATB						1

#define DIET_LSB_IN_REG_STATB						2
#define DIET_MSB_IN_REG_STATB						3
#define VREG_LSB_IN_REG_STATA						4
#define VREG_MSB_IN_REG_STATA						5

#define CELL1_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 0
#define CELL2_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 1
#define CELL3_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 2
#define CELL4_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 3
#define CELL5_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 4
#define CELL6_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 5
#define CELL7_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 6
#define CELL8_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 7
#define CELL9_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 0
#define CELL10_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 1
#define CELL11_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 2
#define CELL12_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 3
#define CELL13_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 4
#define CELL14_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 5
#define CELL15_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 6
#define CELL16_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 7
#define CELL17_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 0
#define CELL18_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD	1 << 1

#define N_CELL_CHANNELS								18
#define CELL_1_OPEN_WIRE							1
#define CELL_2_OPEN_WIRE							2
#define CELL_3_OPEN_WIRE							3
#define CELL_4_OPEN_WIRE							4
#define CELL_5_OPEN_WIRE							5
#define CELL_6_OPEN_WIRE							6
#define CELL_7_OPEN_WIRE							7
#define CELL_8_OPEN_WIRE							8
#define CELL_9_OPEN_WIRE							9
#define CELL_10_OPEN_WIRE							10
#define CELL_11_OPEN_WIRE							11
#define CELL_12_OPEN_WIRE							12
#define CELL_13_OPEN_WIRE							13
#define CELL_14_OPEN_WIRE							14
#define CELL_15_OPEN_WIRE							15
#define CELL_16_OPEN_WIRE							16
#define CELL_17_OPEN_WIRE							17
#define CELL_18_OPEN_WIRE							18

/* For open cells , a constant IC number is required hence taking 3 as it is the maximum number supported by UI */
#define MAX_IC_FOR_OPENCELLS						3

struct systemConfig system_configuration;

extern bool is_gui_connected;

//cell_asic BMS_IC[TOTAL_IC]; //!< Global Battery Variable

QueueHandle_t Get_ReceiverQueueHandler ( void );

#endif /* BMS_COMMON_H */
