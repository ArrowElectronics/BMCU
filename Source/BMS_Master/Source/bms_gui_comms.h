/*!
********************************************************************************
* @file         :   bms_gui_comms.h
* @author       :   Srikanth Reddy Ramidi, Ashvin Ramani
* @brief        :   Prototypes for Logic of Communication between BMS and GUI
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_GUI_COMMS_H
#define BMS_GUI_COMMS_H

/************************* Includes ***************************/

#include "bms_common.h"
#include "bms_gpio.h"

/* Message types as per GUI protocol document */
enum Message_Type
{
	COMMAND = 0x01,
	RESPONSE,
};

/* IC Types as per GUI protocol document */
enum IC_Types
{
	ADBMS1818 = 0x01,
	ADBMS1816,
};

/* Transport layer packet */
typedef struct TransportLayerPacket
{
	uint8_t SOF[3];                      // Start of Frame
	uint16_t ML;                         // Message Length
	uint8_t MT;                          // Message Type
	application_layer_packet ApplicationLayerPacket;		// Message Payload for Application Layer
	uint16_t ReceivedChecksum;			// Received Checksum of the packet
	uint16_t CalculatedChecksum;		// Calculated Checksum of the packet
	uint16_t size;						// whole packet size
} transport_layer_packet;

/* Transport layer attributes position in the Transport layer packet */
#define TL_PACKET_SOF_BYTE1_POS						0
#define TL_PACKET_SOF_BYTE2_POS						TL_PACKET_SOF_BYTE1_POS + 1
#define TL_PACKET_SOF_BYTE3_POS						TL_PACKET_SOF_BYTE2_POS + 1
#define TL_PACKET_ML_BYTE1_POS						TL_PACKET_SOF_BYTE3_POS + 1
#define TL_PACKET_ML_BYTE2_POS						TL_PACKET_ML_BYTE1_POS + 1
#define TL_PACKET_MT_POS							TL_PACKET_ML_BYTE2_POS + 1
#define TL_PACKET_CHECKSUM_BYTE1_POS(N)				N-2
#define TL_PACKET_CHECKSUM_BYTE2_POS(N)				N-1

/* Application layer attributes position in the Message payload */
#define MESSAGE_PAYLOAD_CL_BYTE1_POS						TL_PACKET_MT_POS+ 1
#define MESSAGE_PAYLOAD_CL_BYTE2_POS						MESSAGE_PAYLOAD_CL_BYTE1_POS + 1
#define MESSAGE_PAYLOAD_OPCODE_POS							MESSAGE_PAYLOAD_CL_BYTE2_POS + 1
#define MESSAGE_PAYLOAD_IC_COUNT_POS						MESSAGE_PAYLOAD_OPCODE_POS + 1
#define MESSAGE_PAYLOAD_IC_NUM_START_POS					MESSAGE_PAYLOAD_IC_COUNT_POS + 1
#define MESSAGE_PAYLOAD_IC_NUM_END_POS					    MESSAGE_PAYLOAD_IC_NUM_START_POS + MAX_SLAVE_IC_NUMBER_BYTES - 1

#define MESSAGE_PAYLOAD_IC_TYPE_START_POS					MESSAGE_PAYLOAD_IC_NUM_END_POS + 1
#define MESSAGE_PAYLOAD_IC_TYPE_END_POS						MESSAGE_PAYLOAD_IC_TYPE_START_POS + MAX_SLAVE_IC_COUNT - 1

#define MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_CONFIG				MESSAGE_PAYLOAD_IC_TYPE_END_POS + 1
#define MESSAGE_PAYLOAD_DL_POS_FOR_CONFIG					MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_CONFIG + 1

#define MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_NON_CONFIG			MESSAGE_PAYLOAD_IC_NUM_END_POS + 1
#define MESSAGE_PAYLOAD_DL_POS_FOR_NON_CONFIG				MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_NON_CONFIG + 1

#define MESSAGE_PAYLOAD_DATA_START_POS_FOR_CONFIG  			MESSAGE_PAYLOAD_DL_POS_FOR_CONFIG+1

#define MESSAGE_PAYLOAD_DATA_START_POS_FOR_NON_CONFIG  		MESSAGE_PAYLOAD_DL_POS_FOR_NON_CONFIG+1
#define MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_CONNECT_DISCONNECT	MESSAGE_PAYLOAD_OPCODE_POS + 1
#define MESSAGE_PAYLOAD_DL_POS_FOR_CONNECT_DISCONNECT		MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_CONNECT_DISCONNECT + 1

/* Response packet attributes positions in buffer */
#define RESPONSE_BUFFER_SOF_BYTE1_POS				0
#define RESPONSE_BUFFER_SOF_BYTE2_POS				RESPONSE_BUFFER_SOF_BYTE1_POS + 1
#define RESPONSE_BUFFER_SOF_BYTE3_POS				RESPONSE_BUFFER_SOF_BYTE2_POS + 1
#define RESPONSE_BUFFER_ML_BYTE1_POS				RESPONSE_BUFFER_SOF_BYTE3_POS + 1
#define RESPONSE_BUFFER_ML_BYTE2_POS				RESPONSE_BUFFER_ML_BYTE1_POS + 1
#define RESPONSE_BUFFER_MT_POS						RESPONSE_BUFFER_ML_BYTE2_POS + 1
#define RESPONSE_BUFFER_RL_BYTE1_POS				RESPONSE_BUFFER_MT_POS + 1
#define RESPONSE_BUFFER_RL_BYTE2_POS				RESPONSE_BUFFER_RL_BYTE1_POS + 1
#define RESPONSE_BUFFER_OPCODE_POS					RESPONSE_BUFFER_RL_BYTE2_POS + 1
#define RESPONSE_BUFFER_IC_NUM_START_POS			RESPONSE_BUFFER_OPCODE_POS + 1
#define RESPONSE_BUFFER_IC_NUM_END_POS			    RESPONSE_BUFFER_IC_NUM_START_POS + MAX_SLAVE_IC_NUMBER_BYTES - 1
#define RESPONSE_BUFFER_RES_STATUS_POS				RESPONSE_BUFFER_IC_NUM_END_POS + 1
#define RESPONSE_BUFFER_DL_POS						RESPONSE_BUFFER_RES_STATUS_POS + 1
#define RESPONSE_BUFFER_DATA_START_POS				RESPONSE_BUFFER_DL_POS + 1
#define RESPONSE_BUFFER_CHECKSUM_BYTE1_POS(DL)		( (DL) == 0 ? (RESPONSE_BUFFER_DL_POS + 1) : ( (RESPONSE_BUFFER_DATA_START_POS) + (DL) ) )
#define RESPONSE_BUFFER_CHECKSUM_BYTE2_POS(DL)		RESPONSE_BUFFER_CHECKSUM_BYTE1_POS(DL) + 1
#define RESPONSE_BUFFER_SIZE(DL)					RESPONSE_BUFFER_CHECKSUM_BYTE2_POS(DL) + 1
#define RESPONSE_BUFFER_RES_STATUS_CONNECT_POS 		RESPONSE_BUFFER_OPCODE_POS + 1
#define RESPONSE_BUFFER_DL_CONNECT_POS				RESPONSE_BUFFER_RES_STATUS_CONNECT_POS + 1
#define RESPONSE_BUFFER_CHECKSUM_BYTE1_CONNECT_POS	RESPONSE_BUFFER_DL_CONNECT_POS + 1
#define RESPONSE_BUFFER_CHECKSUM_BYTE2_CONNECT_POS	RESPONSE_BUFFER_CHECKSUM_BYTE1_CONNECT_POS + 1
#define RESPONSE_BUFFER_SIZE_CONNECT				RESPONSE_BUFFER_CHECKSUM_BYTE2_CONNECT_POS + 1

#define ERROR_IN_DATA_VALIDATION					-1

/* Maximum queue length for commands queue*/
#define MAX_QUEUE_LIMIT								10

/* Maximum size for receiving command via UART  */
#define MAX_CMD_SIZE								200

#define CRC_SIZE									2

bool GUIComms_Initialize( void );

/*
 * @brief: This is the serial receiver thread which monitors Serial interface for any commands from GUI and push those commands to receiver queue
 */
void vTask_serial_receiver_Thread(void* parameter);

/*
 * @brief: This is the serial sender thread which monitors sender queue for elements and if found  then it will send it to GUI
 */
void vTask_serial_sender_Thread(void* parameter);

/*
 * @brief: This API will perform transport layer error check
 */
Gui_Response_Code Transport_layer_Error_Check(transport_layer_packet TLpacket);

/*
 * @brief: This API will calculate the checksum of provided packet
 */
uint16_t Calculate_Checksum(transport_layer_packet TL_command_packet);

/*
 * @brief: This API will calculate the checksum of provided packet
 */
uint16_t Calculate_Received_Packet_Checksum(uint8_t* packet, uint16_t packet_size_wo_checksum);

/*
 * @brief: This API will perform application layer error check
 */
Gui_Response_Code Application_layer_Error_Check(application_layer_packet msgPayload);

/*
 * @brief: This API will parse the received packet and stores it in the transport_layer_packet structure
 */
void parse_received_packet(uint8_t* packet, uint16_t packet_size, transport_layer_packet *TLpacket);

/*
 * @brief: Helper function for constructing response buffer from response packet structure and sending it to queue
 */
void send_response_buffer_to_queue(transport_layer_packet TL_response_packet);

/*
 * @brief: Helper function for filling the response packet structure and sending response buffer to queue
 */
void fill_and_send_response_packet_to_queue( application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length, uint8_t response_code );

/*
 * @brief: Helper function for checking GUI connection
 */
Gui_Response_Code Check_GUI_Connection_Error( transport_layer_packet TLpacket  );

/*
 *  @brief: Helper function for receiving packet with valid SOF
 */
int16_t usb_receive_packet_with_valid_sof(uint8_t* rcvBuf);

/*
 *  @brief: Helper function for parsing the received packet
 */
void print_received_packet(uint8_t* rcvBuf, uint8_t rcv_len);

#endif /* BMS_GUI_COMMS_H */
