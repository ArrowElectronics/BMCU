/*!
********************************************************************************
* @file         :   bms_application.h
* @author       :   Srikanth Reddy Ramidi, Ashvin Ramani
* @brief        :   Prototypes for Processing of GUI Command
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_APPLICATION_H
#define BMS_APPLICATION_H

/************************* Includes ***************************/

#include "bms_common.h"
#include "bms_gui_comms.h"
#include "bms_dura_frc_spi.h"
#include "bms_adbms_lib_link.h"

/* Cell Voltage commands sequence for GUI Start command */
enum GUI_Start_Voltage_Cmd_Sequence{
	CELL_ADC_CONVERSION = 0x00,
	CELL_POLL_ADC,
	CELL_VOL_REG_GROUP_A,
	CELL_VOL_REG_GROUP_B,
	CELL_VOL_REG_GROUP_C,
	CELL_VOL_REG_GROUP_D,
	CELL_VOL_REG_GROUP_E,
	CELL_VOL_REG_GROUP_F,
};

/* Stat Voltage commands sequence for GUI Start command */
enum Stat_Voltage_Cmd_Sequence{
	STAT_ADC_CONVERSION = 0x00,
	STAT_VOL_REG_GROUP_A,
	STAT_VOL_REG_GROUP_B,
};

/* Aux Voltage commands sequence for GUI Start command */
enum Aux_Voltage_Cmd_Sequence{
	AUX_ADC_CONVERSION = 0x00,
	AUX_VOL_REG_GROUP_A,
	AUX_VOL_REG_GROUP_B,
	AUX_VOL_REG_GROUP_C,
	AUX_VOL_REG_GROUP_D,
};


enum command_event_bits{
	EVENT_CONTINUOUS = 0x01,
	EVENT_GUI_START = 0x02,
};

typedef enum
{
	CELL_OV_UV =      0x00,
	GPIO_OV_UV =      0x01,
	OTHER_OV_UV =     0x02,
	CELL_OPEN_WIRED = 0x03,
	SYSTEM_FAULT =    0x04,
	EN_FAULT_FLAG  = 0x05,
}fault_enable_group;

/*! IC register structure. */
typedef struct
{
  uint8_t tx_data[8];  //!< Stores data + CRC to be transmitted
  uint8_t rx_data[8];  //!< Stores received data
  uint8_t rx_pec_match; //!< If a PEC error was detected during most recent read cmd
} IC_REGISTER;

/*! Cell variable structure */
typedef struct
{
  IC_REGISTER config;
  IC_REGISTER configb;
} CELL_ASIC;

bool Application_Initialize( void );

void Command_Execution_Thread(void* parameter);

/*
 * @brief: This is the continuous command execution thread which monitors the event group
 * With respect to the event bit it will either perform continuous or GUI start command
 */
void Continuous_Command_Thread(void* parameter);

/*
 * @brief: This API is used for setting Over voltage and Under voltage Thresold in ADBMS board
 */
int8_t Set_Cell_Voltage_Threshold(void);

/*
 * @brief: This API is used for setting system configuration like number of boards and type of boards
 */
void Set_System_Configuration(application_layer_packet *Message_Payload);

/*
 * @brief: Helper function for executing adbms1818 command
 */
int8_t Run_ADBMS1818_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length);

/*
 * @brief: Helper function for executing one shot command
 */
void Run_OneShot_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length);

/*
 * @brief: Helper function for executing continuous command
 */
void Run_Continuous_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length);

/*
 * @brief: Helper function for executing stop command
 */
void Run_Stop_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length);

/*
 * @brief: Helper function for executing GUI start command i.e.., sending real time cell voltage measurements to GUI
 */
void Run_GUI_Start_Vol_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length);

void Run_GUI_Start_Stat_Vol_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length);

void Run_GUI_Start_Aux_Vol_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length);

/*
 * @brief: Helper function for executing Connect/Disconnect
 */
void Run_Connect_Disconnect_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length);

/*
 * @brief: Wrapper function for setting Cell UV
 */
void ADBMS1818_Set_CFG_UV(uint8_t nIC, CELL_ASIC *ic,uint16_t uv);

/*
 * @brief: Wrapper function for setting Cell OV
 */
void ADBMS1818_Set_CFG_OV(uint8_t nIC, CELL_ASIC *ic,uint16_t uv);

/*
 * @brief: Wrapper function for setting DCTO
 */
void ADBMS1818_Set_CFG_Dcto(uint8_t nIC, CELL_ASIC *ic, bool dcto[4]);

/*
 * @brief: Wrapper function for setting DTMEN bit
 */
void ADBMS1818_Set_CFGB_Dtmen(uint8_t nIC, CELL_ASIC *ic, bool dtmen);
void Faults_Thread(void* parameter);

#endif /* BMS_APPLICATION_H */
