/*!
********************************************************************************
* @file         :   bms_gui_comms.c
* @author       :   Srikanth Reddy Ramidi, Ashvin Ramani
* @brief        :   Logic of Communication between BMS and GUI
*******************************************************************************/

#include "bms_gui_comms.h"

typedef struct SerialResponse{
	uint8_t Buffer[1024];
}serial_response;

/* SOF(Start of Frame) is fixed to 'BMS' as per GUI protocol */
const int8_t SOF[3]={0x42,0x4d,0x53};

//Commands Queue handle
QueueHandle_t xReceiverQueueHandle = NULL, xSenderQueueHandle = NULL;

QueueHandle_t Get_ReceiverQueueHandler ( void )
{
    return xReceiverQueueHandle;
}

uint16_t ascii_to_string_of_hexa(uint8_t* data, uint8_t length, uint8_t* converted_data)
{
    uint16_t conv_index=0;
    uint8_t ch;

    for( int index = 0; index < length; index++)
    {
        for( int in_index = 0; in_index < 2; in_index++)
        {
            if( in_index == 0 )
            {
                ch = (data[index] >> 4) & 0x0f;
            }
            else
            {
                ch = data[index] & 0x0f;
            }

            if( ( ch == 0U ) || (( ch > 0U ) && ( ch <= 9U ) ) )
            {
            	/*
            	 * For 0 to 9 the value in hexa will be '0' to '9'
            	 * So in order to convert to string add the integer to 0's ASCII value(i.e.., '0')
            	 */
                converted_data[conv_index++] = '0' +  ch;
            }
            else if( ( ch >= 10U ) && ( ch <= 15U ) )
            {
            	/*
				 * For 10 to 15 the value in hexa will be 'A' to 'F'
				 * So in order to convert to string substract the starting integer value(10)
				 * from the sum of A's ASCII value(i.e.., '0') and integer
				 */
                converted_data[conv_index++] = 'A' +  ch - 10;
            }
            else
            {
            	/* Keep Code Happy! */
            }
        }
    }

    return conv_index;
}

Gui_Response_Code crc_error_check(transport_layer_packet TLpacket)
{
	uint8_t received_data_len=0;
	uint16_t received_crc=0, calculated_crc = 0;


	if( ( TLpacket.ApplicationLayerPacket.Op_Code == READ ) || ( TLpacket.ApplicationLayerPacket.Op_Code == WRITE ) )
	{
		if( TLpacket.ApplicationLayerPacket.DL > 0 )
		{
			uint8_t received_data[TLpacket.ApplicationLayerPacket.DL];

			received_data_len = ADBMS1818_CMD_SIZE - CRC_SIZE ;

			for(uint8_t index=0;index<received_data_len;index++)
			{
				received_data[index] = TLpacket.ApplicationLayerPacket.Data[index];
			}

			/* Next 2 bytes after command are CRC bytes */
			received_crc = TLpacket.ApplicationLayerPacket.Data[received_data_len] << 8 ;
			received_crc |= ( ( (uint16_t)TLpacket.ApplicationLayerPacket.Data[received_data_len + 1] ) & 0x00FFU );

			calculated_crc = Calculate_CRC(received_data_len, received_data);

			if(received_crc != calculated_crc)
			{
				return ADBMS1818_CRC_MISMATCHED;
			}

			/*
			 * For Write command be like : ADBMS1818(first 4 bytesincluding crc) then data to write(next 8 bytes including crc)
			 * eg:- ccxxaaaaaamm
			 * cc is adbms command and xx is it's CRC, aaaaaa is data of 1st IC and mm is it's CRC
			 */
			if(TLpacket.ApplicationLayerPacket.Op_Code == WRITE)
			{
				for(uint8_t ic_index=0;ic_index<TLpacket.ApplicationLayerPacket.IC_Count;ic_index++)
				{

					received_data_len = RESPONSE_SIZE - CRC_SIZE ;

					for(uint8_t index=0;index<received_data_len;index++)
					{
						/*
						 * Command and data to written will be stored in Data field in the following format
						 * Data : ADBMS1818_CMD_SIZE(first 4 bytes including CRC) then IC1 data to write(next 8 bytes including CRC) then IC2 data to write(next 8 bytes including CRC)
						 */
						received_data[index] = TLpacket.ApplicationLayerPacket.Data[ADBMS1818_CMD_SIZE + (ic_index*RESPONSE_SIZE) + index];
					}

					/* Next 2 bytes after command are CRC bytes */
					received_crc = TLpacket.ApplicationLayerPacket.Data[ADBMS1818_CMD_SIZE + (ic_index*RESPONSE_SIZE) + received_data_len] << 8 ;
					received_crc |= ( ( (uint16_t)TLpacket.ApplicationLayerPacket.Data[ADBMS1818_CMD_SIZE + (ic_index*RESPONSE_SIZE) + received_data_len + 1] ) & 0x00FFU );
					calculated_crc = Calculate_CRC(received_data_len, received_data);

					if(received_crc != calculated_crc)
					{
						return ADBMS1818_CRC_MISMATCHED;
					}
				}
			}
		}
	}

	return COMMAND_ACCEPTED;
}

Gui_Response_Code Transport_layer_Error_Check(transport_layer_packet TLpacket)
{
	uint16_t calculatedChecksum=0;

	if( strncmp((const char*)TLpacket.SOF, (const char*)SOF, 3) != 0 ) // First 3 bytes of packet are Start Of Frame bytes, comparing it with predefined SOF
	{
		return UNRECOGNISED_SOF;
	}

	/* Message Length is the length of the packet excluding SOF(first 3) bytes */
	if( (TLpacket.ML != (TLpacket.size-sizeof(TLpacket.ML)) - sizeof(TLpacket.SOF) ) || (TLpacket.ML == 0U) )
	{
		return INVALID_MESSAGE_LENGTH;
	}

	if( (TLpacket.MT != COMMAND) )
	{
		return INVALID_MESSAGE_TYPE;
	}

	if( TLpacket.ReceivedChecksum != TLpacket.CalculatedChecksum )
	{
		return CHECKSUM_MISMATCHED;
	}

	return COMMAND_ACCEPTED;

}

Gui_Response_Code Check_GUI_Connection_Error( transport_layer_packet TLpacket )
{
	/* If command is other than connect and if gui is not connected then throw error */
	if( ( TLpacket.ApplicationLayerPacket.Op_Code != CONNECT ) && (is_gui_connected == false) )
	{
		return GUI_NOT_CONNECTED;
	}

	return COMMAND_ACCEPTED;
}

uint16_t Calculate_Checksum(transport_layer_packet TLpacket)
{
	uint16_t SumOfBytes=0, Checksum=0;

	/* Add all bytes in the transport layer packet except checksum value */

	SumOfBytes += TLpacket.SOF[0];
	SumOfBytes += TLpacket.SOF[1];
	SumOfBytes += TLpacket.SOF[2];

	SumOfBytes += (TLpacket.ML & 0x00FF); // First byte of 2 bytes
	SumOfBytes += (TLpacket.ML >> 8); // second byte of 2 bytes

	SumOfBytes += TLpacket.MT;

	SumOfBytes += (TLpacket.ApplicationLayerPacket.CL & 0x00FF); // First byte of 2 bytes
	SumOfBytes += (TLpacket.ApplicationLayerPacket.CL >> 8); // second byte of 2 bytes

	SumOfBytes += TLpacket.ApplicationLayerPacket.Op_Code;

	for ( uint8_t index = 0U; index < MAX_SLAVE_IC_NUMBER_BYTES; index++ )
	{
		SumOfBytes += TLpacket.ApplicationLayerPacket.IC_Number[index];
	}

	SumOfBytes += TLpacket.ApplicationLayerPacket.DL;

	if(TLpacket.ApplicationLayerPacket.DL != 0)
	{
		for(int index=0;index<TLpacket.ApplicationLayerPacket.DL;index++)
		{
			SumOfBytes += TLpacket.ApplicationLayerPacket.Data[index];
		}
	}

	if(TLpacket.ApplicationLayerPacket.Op_Code == CONFIGURATION)
	{
		uint8_t icType_Cnt = 0U;
		for(int index=0;index<MAX_SLAVE_IC_NUMBER_BYTES;index++)
		{
			for (uint8_t lp = 0U; lp < IC_NUMBERS_PER_BYTE; lp++ )
			{
				if( (TLpacket.ApplicationLayerPacket.IC_Number[index] >> lp) & 0x1 )
				{
					SumOfBytes += TLpacket.ApplicationLayerPacket.IC_Types[icType_Cnt];
				}
				icType_Cnt++;
			}
		}
	}

	if(TLpacket.MT == COMMAND )
	{
		SumOfBytes += TLpacket.ApplicationLayerPacket.IC_Count;
		SumOfBytes += TLpacket.ApplicationLayerPacket.OP_Type;
	}
	else if(TLpacket.MT == RESPONSE)
	{
		SumOfBytes += TLpacket.ApplicationLayerPacket.Response_Status;

	}


	/* 0x10000 - sum of bytes gives the checksum of the recieved packet */
	Checksum = 0x10000 - SumOfBytes;

	return Checksum;
}

uint16_t Calculate_Received_Packet_Checksum(uint8_t* packet, uint16_t packet_size_wo_checksum)
{
	uint16_t SumOfBytes=0, Checksum=0;

	/* Add all bytes in the packet except checksum value */

	for(uint16_t index=0;index<packet_size_wo_checksum;index++)
	{
		SumOfBytes += packet[index];
	}

	/* 0x10000 - sum of bytes gives the checksum of the recieved packet */
	Checksum = 0x10000 - SumOfBytes;

	return Checksum;
}

Gui_Response_Code Application_layer_Error_Check(application_layer_packet msgPayload)
{

	/* Command Length is the length of the messagePayload excluding CL(first 2) bytes */
	if( (msgPayload.CL != (msgPayload.Size-2U) ) || (msgPayload.CL == 0U) )
	{
		return INVALID_COMMAND_LENGTH;
	}


	if( (msgPayload.Op_Code != CONFIGURATION) && (msgPayload.Op_Code != READ) && (msgPayload.Op_Code != WRITE) && (msgPayload.Op_Code != FAULT_MESSAGE) && (msgPayload.Op_Code != CONNECT) && (msgPayload.Op_Code != DISCONNECT) && (msgPayload.Op_Code != GUI_START) )
	{
		return UNRECOGNISED_OPCODE;
	}

	if( (msgPayload.Op_Code == READ) || (msgPayload.Op_Code == WRITE) || (msgPayload.Op_Code == GUI_START) )
	{
		if(system_configuration.Is_Config_Set == false)
		{
			return CONFIGURATION_NOT_YET_DONE;
		}
		else
		{

			if( msgPayload.IC_Count == 0U || ( msgPayload.IC_Count > system_configuration.IC_Count ) )
			{
				return INVALID_NUMBER_OF_ICS;
			}

			/* Checking whether the received IC Number is matching with the Configured IC Numbers*/
			for(int index=0;index<MAX_SLAVE_IC_NUMBER_BYTES;index++)
			{
				for (uint8_t lp = 0U; lp < IC_NUMBERS_PER_BYTE; lp++ )
				{
					if( ( msgPayload.IC_Number[index] >> lp ) & 0x1 )
					{
						if( !( (system_configuration.IC_Number[index] >> lp) & (0x01) ) )
						{
							return INVALID_OR_MISMATCHED_IC_NUMBER_WITH_IC_COUNT;
						}
					}
				}
			}

			/*
			 * Data Length should be same as the received message payload size - all the field's size except Data field
			 * DL = message payload size - CL size - Opcode size - IC Count size - IC Number size - OP Type size - DL size
			 */
			if( msgPayload.DL != ( msgPayload.Size - sizeof(msgPayload.CL) - sizeof(msgPayload.Op_Code) - sizeof(msgPayload.IC_Count) - MAX_SLAVE_IC_NUMBER_BYTES -
			     	 	 	 	   sizeof(msgPayload.OP_Type) - sizeof(msgPayload.DL) ) )
			{
				return INVALID_COMMAND_DATA_LENGTH;
			}

		}
	}

	if( msgPayload.Op_Code == CONFIGURATION )
	{
		if( msgPayload.IC_Count == 0U )
		{
			return INVALID_NUMBER_OF_ICS;
		}

		for (int index=0;index<msgPayload.IC_Count;index++)
		{
			if( !( ( msgPayload.IC_Types[index]  == ADBMS1818 ) || ( msgPayload.IC_Types[index] == ADBMS1816 ) ) )
			{
				return INVALID_OR_MISMATCHED_IC_TYPE_WITH_IC_COUNT;
			}
		}

		/* Fix data length */
		if( msgPayload.DL != CONFIGURATION_DATA_SIZE )
		{
			return INVALID_COMMAND_DATA_LENGTH;
		}
	}

	if( msgPayload.OP_Type  == ONE_SHOT || msgPayload.OP_Type == CONTINUOUS || msgPayload.OP_Type == STOP )
	{
		/* Command specific check */
		if ( ( msgPayload.Op_Code  == CONNECT || msgPayload.Op_Code  == DISCONNECT || msgPayload.Op_Code == CONFIGURATION || msgPayload.Op_Code == GUI_START ) && msgPayload.OP_Type != ONE_SHOT )
		{
			return INVALID_OPERATION_TYPE;
		}
	}
	else
	{
		return INVALID_OPERATION_TYPE;
	}

	if( ( msgPayload.Op_Code  == CONNECT || msgPayload.Op_Code  == DISCONNECT || msgPayload.Op_Code  == GUI_START ) && msgPayload.DL != 0U )
	{
		return INVALID_COMMAND_DATA_LENGTH;
	}

	return COMMAND_ACCEPTED;
}

void parse_received_packet(uint8_t* packet, uint16_t packet_size, transport_layer_packet *TLpacket)
{
	TLpacket->size = packet_size;

	/* First 3 bytes of packet are SOF bytes */
	TLpacket->SOF[0] = packet[TL_PACKET_SOF_BYTE1_POS];
	TLpacket->SOF[1] = packet[TL_PACKET_SOF_BYTE2_POS];
	TLpacket->SOF[2] = packet[TL_PACKET_SOF_BYTE3_POS];


	/* 4th and 5th bytes of packet are Message Length bytes */
	TLpacket->ML = packet[TL_PACKET_ML_BYTE1_POS];
	TLpacket->ML = TLpacket->ML << 8;
	TLpacket->ML |= ( ( (uint16_t)packet[TL_PACKET_ML_BYTE2_POS] ) & 0x00FFU );

	/* 6th byte of packet is Message Type byte */
	TLpacket->MT = packet[TL_PACKET_MT_POS];

	/* Last 2 bytes in the packet consists of checksum value */
	TLpacket->ReceivedChecksum = packet[TL_PACKET_CHECKSUM_BYTE1_POS(packet_size)];
	TLpacket->ReceivedChecksum = TLpacket->ReceivedChecksum << 8;
	TLpacket->ReceivedChecksum |= ( ( (uint16_t)packet[TL_PACKET_CHECKSUM_BYTE2_POS(packet_size)] ) & 0x00FFU );

	/* Message payload size = TLpacket size - SOF size - ML size - MT size - Checksum size */
	TLpacket->ApplicationLayerPacket.Size = TLpacket->size - sizeof(TLpacket->SOF) - sizeof(TLpacket->ML) - sizeof(TLpacket->MT) - sizeof(TLpacket->ReceivedChecksum) ;

	/* First 2 bytes of Message payload are Message Length bytes */
	TLpacket->ApplicationLayerPacket.CL = packet[MESSAGE_PAYLOAD_CL_BYTE1_POS];
	TLpacket->ApplicationLayerPacket.CL = TLpacket->ApplicationLayerPacket.CL << 8;
	TLpacket->ApplicationLayerPacket.CL = packet[MESSAGE_PAYLOAD_CL_BYTE2_POS];


	TLpacket->ApplicationLayerPacket.Op_Code = packet[MESSAGE_PAYLOAD_OPCODE_POS];

	if( !( ( TLpacket->ApplicationLayerPacket.Op_Code == CONNECT ) || ( TLpacket->ApplicationLayerPacket.Op_Code == DISCONNECT ) ) )
//	if( ( ( TLpacket->ApplicationLayerPacket.Op_Code == CONFIGURATION ) || ( TLpacket->ApplicationLayerPacket.Op_Code == FAULT_MESSAGE ) || ( TLpacket->ApplicationLayerPacket.Op_Code == GUI_START ) || ( TLpacket->ApplicationLayerPacket.Op_Code == READ ) || ( TLpacket->ApplicationLayerPacket.Op_Code == WRITE ) ) )
	{
		TLpacket->ApplicationLayerPacket.IC_Count = packet[MESSAGE_PAYLOAD_IC_COUNT_POS];

		for ( uint8_t lp = 0U; lp < MAX_SLAVE_IC_NUMBER_BYTES; lp++ )
		{
		    TLpacket->ApplicationLayerPacket.IC_Number[lp] = packet[MESSAGE_PAYLOAD_IC_NUM_END_POS - lp]; // Should take from last to first since the ICT values are assigned in 120,119...2.1 manner;
		}
	}

	if( ( TLpacket->ApplicationLayerPacket.Op_Code == CONNECT ) || ( TLpacket->ApplicationLayerPacket.Op_Code == DISCONNECT ) )
	{
		TLpacket->ApplicationLayerPacket.IC_Count = 0;

		for ( uint8_t lp = 0U; lp < MAX_SLAVE_IC_NUMBER_BYTES; lp++ )
		{
		    TLpacket->ApplicationLayerPacket.IC_Number[lp] = 0U;
		}

		TLpacket->ApplicationLayerPacket.DL =  packet[MESSAGE_PAYLOAD_DL_POS_FOR_CONNECT_DISCONNECT];
		TLpacket->ApplicationLayerPacket.OP_Type = packet[MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_CONNECT_DISCONNECT];
	}

	else if(TLpacket->ApplicationLayerPacket.Op_Code == CONFIGURATION)
	{
		TLpacket->ApplicationLayerPacket.DL =  packet[MESSAGE_PAYLOAD_DL_POS_FOR_CONFIG];

		TLpacket->ApplicationLayerPacket.OP_Type = packet[MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_CONFIG];

		for(uint8_t ic_index=0;ic_index<MAX_SLAVE_IC_COUNT;ic_index++)
		{
			TLpacket->ApplicationLayerPacket.IC_Types[ic_index] = packet[MESSAGE_PAYLOAD_IC_TYPE_END_POS - ic_index]; // Should take from last to first since the ICT values are assigned in 120,119...2.1 manner
		}

		if(TLpacket->ApplicationLayerPacket.DL > 0)
		{
			for(uint8_t data_index=0;data_index<TLpacket->ApplicationLayerPacket.DL;data_index++)
			{
				TLpacket->ApplicationLayerPacket.Data[data_index] = packet[MESSAGE_PAYLOAD_DATA_START_POS_FOR_CONFIG + data_index];
			}
		}
	}
	else
	{
		TLpacket->ApplicationLayerPacket.DL =  packet[MESSAGE_PAYLOAD_DL_POS_FOR_NON_CONFIG];

		TLpacket->ApplicationLayerPacket.OP_Type = packet[MESSAGE_PAYLOAD_OP_TYPE_POS_FOR_NON_CONFIG];

		if(TLpacket->ApplicationLayerPacket.DL > 0)
		{
			for(uint8_t data_index=0;data_index<TLpacket->ApplicationLayerPacket.DL;data_index++)
			{
				TLpacket->ApplicationLayerPacket.Data[data_index] = packet[MESSAGE_PAYLOAD_DATA_START_POS_FOR_NON_CONFIG + data_index];
			}
		}
	}

	TLpacket->CalculatedChecksum = Calculate_Received_Packet_Checksum(packet, packet_size-2);
}

void send_response_buffer_to_queue(transport_layer_packet TL_response_packet)
{

	uint8_t response_buffer[140] = {0};
	uint16_t converted_response_size;
	serial_response response_to_send = {0};
	BaseType_t queueRet;

	/* First 3 bytes of packet are SOF bytes */
	response_buffer[RESPONSE_BUFFER_SOF_BYTE1_POS] = TL_response_packet.SOF[0];
	response_buffer[RESPONSE_BUFFER_SOF_BYTE2_POS] = TL_response_packet.SOF[1];
	response_buffer[RESPONSE_BUFFER_SOF_BYTE3_POS] = TL_response_packet.SOF[2];

	/* 4th and 5th bytes of packet are Message Length bytes */
	response_buffer[RESPONSE_BUFFER_ML_BYTE1_POS] = (TL_response_packet.ML >> 8); // First byte of 2 bytes
	response_buffer[RESPONSE_BUFFER_ML_BYTE2_POS] = (TL_response_packet.ML  & 0x00FF); // second byte of 2 bytes

	/* 6th byte of packet is Message Type byte */
	response_buffer[RESPONSE_BUFFER_MT_POS] = TL_response_packet.MT;

	response_buffer[RESPONSE_BUFFER_RL_BYTE1_POS] = (TL_response_packet.ApplicationLayerPacket.CL >> 8); // First byte of 2 bytes
	response_buffer[RESPONSE_BUFFER_RL_BYTE2_POS] = (TL_response_packet.ApplicationLayerPacket.CL & 0x00FF ); // second byte of 2 bytes

	response_buffer[RESPONSE_BUFFER_OPCODE_POS] = TL_response_packet.ApplicationLayerPacket.Op_Code ;

	if( !( (TL_response_packet.ApplicationLayerPacket.Op_Code == CONNECT) || (TL_response_packet.ApplicationLayerPacket.Op_Code == DISCONNECT) ) )
	{
		for (uint8_t lp = 0U; lp < MAX_SLAVE_IC_NUMBER_BYTES; lp++)
		{
			response_buffer[RESPONSE_BUFFER_IC_NUM_START_POS + lp] = TL_response_packet.ApplicationLayerPacket.IC_Number[lp];
		}
		response_buffer[RESPONSE_BUFFER_RES_STATUS_POS] = TL_response_packet.ApplicationLayerPacket.Response_Status ;

		response_buffer[RESPONSE_BUFFER_DL_POS] = TL_response_packet.ApplicationLayerPacket.DL ;
	}
	else
	{
		response_buffer[RESPONSE_BUFFER_RES_STATUS_CONNECT_POS] = TL_response_packet.ApplicationLayerPacket.Response_Status ;

		response_buffer[RESPONSE_BUFFER_DL_CONNECT_POS] = TL_response_packet.ApplicationLayerPacket.DL ;
	}

	/*
	 * After DL field Data field will present that will be of size DL
	 * so if DL is non-zero then Data will be filled in response buffer
	 * in indices starting from DL_position + 1 position till DL_position + DL
	 * Checksum will be filled in next 2 bytes after Data
	 * So checksum will be in indices DL_position + DL + 1 and DL_position + DL + 2
	 */

	if(TL_response_packet.ApplicationLayerPacket.DL != 0)
	{
		for(uint8_t data_index=0;data_index<TL_response_packet.ApplicationLayerPacket.DL;data_index++)
		{
			response_buffer[RESPONSE_BUFFER_DATA_START_POS + data_index] = TL_response_packet.ApplicationLayerPacket.Data[data_index] ;
		}

		response_buffer[RESPONSE_BUFFER_CHECKSUM_BYTE1_POS(TL_response_packet.ApplicationLayerPacket.DL)] = (TL_response_packet.CalculatedChecksum >> 8) ; // First byte of 2 bytes
		response_buffer[RESPONSE_BUFFER_CHECKSUM_BYTE2_POS(TL_response_packet.ApplicationLayerPacket.DL)] = (TL_response_packet.CalculatedChecksum & 0x00FF ) ; // second byte of 2 bytes

	}

	if( !( (TL_response_packet.ApplicationLayerPacket.Op_Code == CONNECT) || (TL_response_packet.ApplicationLayerPacket.Op_Code == DISCONNECT) ) )
	{
		/*
		 * After DL field Data field will present that will be of size DL
		 * if DL is zero then we can ignore Data field  in response buffer
		 * After Data field, checksum need to be filled
		 * so in indices  DL_position + 1 and DL_position + 2 Checksum will be filled
		 */
		response_buffer[RESPONSE_BUFFER_CHECKSUM_BYTE1_POS(TL_response_packet.ApplicationLayerPacket.DL)] = (TL_response_packet.CalculatedChecksum >> 8) ; // First byte of 2 bytes
		response_buffer[RESPONSE_BUFFER_CHECKSUM_BYTE2_POS(TL_response_packet.ApplicationLayerPacket.DL)] = (TL_response_packet.CalculatedChecksum & 0x00FF ) ; // second byte of 2 bytes

		/*
		 * GUI expecting response buffer as string so converting response from ascii to string of hexa
		 */
		converted_response_size = ascii_to_string_of_hexa(response_buffer,RESPONSE_BUFFER_SIZE(TL_response_packet.ApplicationLayerPacket.DL), response_to_send.Buffer);
	}
	else
	{
		response_buffer[RESPONSE_BUFFER_CHECKSUM_BYTE1_CONNECT_POS] = (TL_response_packet.CalculatedChecksum >> 8) ; // First byte of 2 bytes
		response_buffer[RESPONSE_BUFFER_CHECKSUM_BYTE2_CONNECT_POS] = (TL_response_packet.CalculatedChecksum & 0x00FF ) ; // second byte of 2 bytes

		/*
		 * GUI expecting response buffer as string so converting response from ascii to string of hexa
		 */
		converted_response_size = ascii_to_string_of_hexa(response_buffer,RESPONSE_BUFFER_SIZE_CONNECT, response_to_send.Buffer);
	}
	response_to_send.Buffer[converted_response_size] = 0;

	/* Pushing response buffer to sender queue */
	queueRet = xQueueSendToBack( xSenderQueueHandle, (const void *) &response_to_send,  portMAX_DELAY );

	if(queueRet != pdPASS )
	{
		//DebugStrLf(DEBUG_INTERFACE, "Error while pushing element into queue...");
	}

}

void fill_and_send_response_packet_to_queue( application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length, uint8_t response_code )
{
	transport_layer_packet TL_response_packet;
	memset(&TL_response_packet, 0, sizeof(transport_layer_packet));

	/* Filling common parameters which will be same for all OP Types */
	for(uint8_t index=0;index<sizeof(SOF);index++)
	{
		TL_response_packet.SOF[index] = SOF[index];
	}

	TL_response_packet.MT = RESPONSE;

	TL_response_packet.ApplicationLayerPacket.Op_Code = Message_Payload->Op_Code;

	TL_response_packet.ApplicationLayerPacket.Response_Status = response_code;

	/* Data Length will be zero for all cases except read with status 'Command_Accepted' */
	TL_response_packet.ApplicationLayerPacket.DL = 0;

	if( (Message_Payload->Op_Code == CONFIGURATION) || (Message_Payload->OP_Type == STOP) )
	{
		/* For configuration IC Number is same as command's IC Number */
		for ( uint8_t lp = 0U; lp < MAX_SLAVE_IC_NUMBER_BYTES; lp++ )
		{
			TL_response_packet.ApplicationLayerPacket.IC_Number[lp] = Message_Payload->IC_Number[(MAX_SLAVE_IC_NUMBER_BYTES - 1) - lp];
		}

		/* Response length of configuration/write will be summation of sizes of opcode, IC number, Response status and data length */

		TL_response_packet.ApplicationLayerPacket.CL = sizeof(TL_response_packet.ApplicationLayerPacket.Op_Code) + MAX_SLAVE_IC_NUMBER_BYTES
										+ sizeof(TL_response_packet.ApplicationLayerPacket.Response_Status) + sizeof(TL_response_packet.ApplicationLayerPacket.Response_Status);

		/* Message Length will be summation of size of MT, MP(sizeof(CL)+CL), sizeof(checksum) */
		TL_response_packet.ML =  sizeof(TL_response_packet.MT) + sizeof(TL_response_packet.ApplicationLayerPacket.CL) + TL_response_packet.ApplicationLayerPacket.CL +  sizeof(TL_response_packet.CalculatedChecksum);

		TL_response_packet.CalculatedChecksum = Calculate_Checksum(TL_response_packet);

		send_response_buffer_to_queue(TL_response_packet);
	}
	else if( ( ( Message_Payload->Op_Code == GUI_START ) || ( Message_Payload->Op_Code == READ ) || ( Message_Payload->Op_Code == WRITE ) || ( Message_Payload->Op_Code == FAULT_MESSAGE ) )  && (Message_Payload->OP_Type != STOP) )
	{
		for(uint8_t outer_index=0;outer_index<MAX_SLAVE_IC_NUMBER_BYTES;outer_index++)
		{
			memset(TL_response_packet.ApplicationLayerPacket.IC_Number, 0x00, MAX_SLAVE_IC_NUMBER_BYTES);

			for (uint8_t lp = 0U; lp < IC_NUMBERS_PER_BYTE; lp++ )
			{
			    /* Check for the requested IC Numbers */
			    if( ( Message_Payload->IC_Number[outer_index] >> lp ) & 0x01 )
			    {
				    /* This is the response of outer_index numbered IC  */
				    TL_response_packet.ApplicationLayerPacket.IC_Number[(MAX_SLAVE_IC_NUMBER_BYTES - 1) - outer_index] = (0x01 << lp ) ;

				    if( ( Message_Payload->Op_Code == FAULT_MESSAGE ) || ( ( ( Message_Payload->Op_Code == GUI_START || Message_Payload->Op_Code == READ ) && ( response_code == COMMAND_ACCEPTED ) ) ) )
				    {
					    if (Message_Payload->Op_Code == GUI_START )
					    {
					        TL_response_packet.ApplicationLayerPacket.DL = TOTAL_START_VOL_RESPONSE_SIZE_EACH_IC ;

						    for(uint8_t inner_index=0;inner_index<TOTAL_START_VOL_RESPONSE_SIZE_EACH_IC;inner_index++)
						    {
							    /* data array consists of all IC's response in sequence for 1st IC (0-48), 2nd IC(49-95) ... */
							    TL_response_packet.ApplicationLayerPacket.Data[inner_index] = data[( ( outer_index*IC_NUMBERS_PER_BYTE ) + lp )*TOTAL_START_VOL_RESPONSE_SIZE_EACH_IC + inner_index];
						    }
					    }
					    else if( Message_Payload->Op_Code == READ )
					    {
						    TL_response_packet.ApplicationLayerPacket.DL = RESPONSE_SIZE ;

						    for(uint8_t inner_index=0;inner_index<RESPONSE_SIZE;inner_index++)
						    {
							    /* data array consists of all IC's response in sequence for 1st IC (0-7), 2nd IC(8-15) ... */
							    TL_response_packet.ApplicationLayerPacket.Data[inner_index] = data[( ( outer_index*IC_NUMBERS_PER_BYTE ) + lp )*RESPONSE_SIZE + inner_index];
						    }
					    }
					    else if(Message_Payload->Op_Code == FAULT_MESSAGE)
					    {
						    TL_response_packet.ApplicationLayerPacket.DL = FAULT_PAYLOAD_SIZE;

						    for(uint8_t inner_index=0;inner_index<FAULT_PAYLOAD_SIZE;inner_index++)
						    {
							    /* data array consists of all IC's response in sequence for 1st IC (0-39), 2nd IC(40-79) ... */
							    TL_response_packet.ApplicationLayerPacket.Data[inner_index] = data[( ( outer_index*IC_NUMBERS_PER_BYTE ) + lp )*FAULT_PAYLOAD_SIZE + inner_index];
						    }
					    }
					    else
					    {
						    /* do nothing */
					    }

					    /* Response length of read will be summation of sizes of opcode, IC number, Response status and data length + data length value*/
					    TL_response_packet.ApplicationLayerPacket.CL = sizeof(TL_response_packet.ApplicationLayerPacket.Op_Code) + MAX_SLAVE_IC_NUMBER_BYTES
															    + sizeof(TL_response_packet.ApplicationLayerPacket.Response_Status) + sizeof(TL_response_packet.ApplicationLayerPacket.DL)
														        + TL_response_packet.ApplicationLayerPacket.DL;

					    /* Message Length will be summation of size of MT, MP(sizeof(CL)+CL), sizeof(checksum) */
					    TL_response_packet.ML =  sizeof(TL_response_packet.MT) + sizeof(TL_response_packet.ApplicationLayerPacket.CL) + TL_response_packet.ApplicationLayerPacket.CL +  sizeof(TL_response_packet.CalculatedChecksum);

					    TL_response_packet.CalculatedChecksum = Calculate_Checksum(TL_response_packet);


					    send_response_buffer_to_queue(TL_response_packet);
				    }
				    else
				    {
					    TL_response_packet.ApplicationLayerPacket.DL = 0;

					    /* Response length of read will be summation of sizes of opcode, IC number, Response status and data length + data length value*/
					    TL_response_packet.ApplicationLayerPacket.CL = sizeof(TL_response_packet.ApplicationLayerPacket.Op_Code) + MAX_SLAVE_IC_NUMBER_BYTES
															    + sizeof(TL_response_packet.ApplicationLayerPacket.Response_Status) + sizeof(TL_response_packet.ApplicationLayerPacket.DL)
														        + TL_response_packet.ApplicationLayerPacket.DL;

					    /* Message Length will be summation of size of MT, MP(sizeof(CL)+CL), sizeof(checksum) */
					    TL_response_packet.ML =  sizeof(TL_response_packet.MT) + sizeof(TL_response_packet.ApplicationLayerPacket.CL) + TL_response_packet.ApplicationLayerPacket.CL +  sizeof(TL_response_packet.CalculatedChecksum);

					    TL_response_packet.CalculatedChecksum = Calculate_Checksum(TL_response_packet);

					    send_response_buffer_to_queue(TL_response_packet);
				    }
			    }
		    }
		}
	}
	else if( ( Message_Payload->Op_Code == CONNECT ) || ( Message_Payload->Op_Code == DISCONNECT ) )
	{

		TL_response_packet.ApplicationLayerPacket.Op_Code = Message_Payload->Op_Code;

		TL_response_packet.ApplicationLayerPacket.OP_Type = Message_Payload->OP_Type;

		/* Response length of read will be summation of sizes of opcode, IC number, Response status and data length + data length value*/
		TL_response_packet.ApplicationLayerPacket.CL = sizeof(TL_response_packet.ApplicationLayerPacket.Op_Code) + sizeof(TL_response_packet.ApplicationLayerPacket.Response_Status) + sizeof(TL_response_packet.ApplicationLayerPacket.DL);

		/* Message Length will be summation of size of MT, MP(sizeof(CL)+CL), sizeof(checksum) */
		TL_response_packet.ML =  sizeof(TL_response_packet.MT) + sizeof(TL_response_packet.ApplicationLayerPacket.CL) + TL_response_packet.ApplicationLayerPacket.CL +  sizeof(TL_response_packet.CalculatedChecksum);

		TL_response_packet.CalculatedChecksum = Calculate_Checksum(TL_response_packet);

		send_response_buffer_to_queue(TL_response_packet);

	}
	else
	{
		 /* NOTE*: This one is for "UNRECOGNISED OpCode". */
		TL_response_packet.ApplicationLayerPacket.Op_Code = Message_Payload->Op_Code;

		TL_response_packet.ApplicationLayerPacket.OP_Type = Message_Payload->OP_Type;

		/* Response length of read will be summation of sizes of opcode, IC number, Response status and data length + data length value*/
		TL_response_packet.ApplicationLayerPacket.CL = sizeof(TL_response_packet.ApplicationLayerPacket.Op_Code) + sizeof(TL_response_packet.ApplicationLayerPacket.Response_Status) + sizeof(TL_response_packet.ApplicationLayerPacket.DL);

		/* Message Length will be summation of size of MT, MP(sizeof(CL)+CL), sizeof(checksum) */
		TL_response_packet.ML =  sizeof(TL_response_packet.MT) + sizeof(TL_response_packet.ApplicationLayerPacket.CL) + TL_response_packet.ApplicationLayerPacket.CL +  sizeof(TL_response_packet.CalculatedChecksum);

		TL_response_packet.CalculatedChecksum = Calculate_Checksum(TL_response_packet);

		send_response_buffer_to_queue(TL_response_packet);
	}
}

int16_t usb_receive_packet_with_valid_sof(uint8_t* rcvBuf)
{
	uint8_t received_char;
	int16_t usb_return=0;
	uint16_t initial_buf_length=0, remaining_message_length=0;


	while(1)
	{
		usb_return = USB_RecvStr((uint8_t *)&received_char, 1);

		if( usb_return < 0 )
		{
			return -1;
		}

		rcvBuf[initial_buf_length] = received_char;
		initial_buf_length++;

		if((initial_buf_length == 1) && (rcvBuf[0] != SOF[0]))
		{
			//DebugData( DEBUG_INTERFACE, "Byte 0(", rcvBuf[0], HEX);
			//DebugStrLf( DEBUG_INTERFACE, ") is not B");
			return ERROR_IN_DATA_VALIDATION;
		}
		else if((initial_buf_length == 2) && (rcvBuf[1] != SOF[1]))
		{
			//DebugData( DEBUG_INTERFACE, "Byte 1(", rcvBuf[1], HEX);
			//DebugStrLf( DEBUG_INTERFACE, ") is not M");
			return ERROR_IN_DATA_VALIDATION;
		}
		else if((initial_buf_length == 3) && (rcvBuf[2] != SOF[2]))
		{
			//DebugData( DEBUG_INTERFACE, "Byte 2(", rcvBuf[2], HEX);
			//DebugStrLf( DEBUG_INTERFACE, ") is not S");
			return ERROR_IN_DATA_VALIDATION;
		}
		else if((initial_buf_length == 4) )
		{
			remaining_message_length = rcvBuf[3] << 8;
		}
		else if((initial_buf_length == 5) )
		{
			remaining_message_length |= ( ( (uint16_t)rcvBuf[4] ) & 0x00FFU );

			//usb_return = UART_RecvStr(DEBUG_INTERFACE, (char *)rcvBuf+initial_buf_length, remaining_message_length, NULL);
			usb_return = USB_RecvStr((uint8_t *)rcvBuf+initial_buf_length, remaining_message_length);

			if( usb_return < 0 )
			{
				return ERROR_IN_DATA_VALIDATION;
			}
			else
			{
				return initial_buf_length+remaining_message_length;
			}
		}

		received_char = 0;
	}
}

void print_received_packet(uint8_t* rcvBuf, uint8_t rcv_len)
{
	DebugStrLf( DEBUG_INTERFACE, "Received packet:" );

	for(int index=0;index<rcv_len;index++)
	{
		DebugData( DEBUG_INTERFACE, " " , rcvBuf[index], HEX);
	}
	DebugStrLf(DEBUG_INTERFACE, " ");

}

void vTask_serial_receiver_Thread(void* parameter)
{
	uint8_t rcvBuf[sizeof(transport_layer_packet)*2];
	transport_layer_packet TLpacket;
	int16_t rcvd_packet_size, avail_num_bytes=0;
    const TickType_t xTicksToWait = portMAX_DELAY;
    BaseType_t queueRet;
	Gui_Response_Code response = COMMAND_ACCEPTED;
    UBaseType_t queueLen;

	while(1)
	{
		rcvd_packet_size=avail_num_bytes=0;
		memset(rcvBuf, 0, sizeof(rcvBuf));

		rcvd_packet_size = usb_receive_packet_with_valid_sof(rcvBuf);

		if(rcvd_packet_size  > 0 )
		{
			parse_received_packet(rcvBuf, rcvd_packet_size, &TLpacket);

			response = Check_GUI_Connection_Error(TLpacket);

			if( response != COMMAND_ACCEPTED )
			{
				fill_and_send_response_packet_to_queue( &TLpacket.ApplicationLayerPacket, NULL, 0, response );
			}
			else
			{
				response = Transport_layer_Error_Check(TLpacket);

				if( response != COMMAND_ACCEPTED )
				{
					//DebugDataLf(DEBUG_INTERFACE, "Transport_layer : Error occured in received packet with response code...", response, HEX);
					fill_and_send_response_packet_to_queue( &TLpacket.ApplicationLayerPacket, NULL, 0, response );
				}
				else
				{

					response = Application_layer_Error_Check(TLpacket.ApplicationLayerPacket);

					if( response != COMMAND_ACCEPTED )
					{
						//DebugDataLf(DEBUG_INTERFACE, "Application_layer : Error occured in received packet with response code...", response, HEX);

						fill_and_send_response_packet_to_queue( &TLpacket.ApplicationLayerPacket, NULL, 0, response );
					}
					else
					{
						response = crc_error_check(TLpacket);

						if( response != COMMAND_ACCEPTED )
						{
							fill_and_send_response_packet_to_queue( &TLpacket.ApplicationLayerPacket, NULL, 0, response );
						}
						else
						{
							queueLen = uxQueueMessagesWaiting( xReceiverQueueHandle);

							while(queueLen == MAX_QUEUE_LIMIT)
							{
								//If queue is full then wait till queue releases one item
								queueLen = uxQueueMessagesWaiting( xReceiverQueueHandle);
							}

							queueRet = xQueueSendToBack( xReceiverQueueHandle, (const void *) &TLpacket.ApplicationLayerPacket,  xTicksToWait );
							if(queueRet != pdPASS )
							{
								//DebugStrLf(DEBUG_INTERFACE, "Error while pushing element into queue...");
							}
						}
					}
				}
			}
		}
		rcvd_packet_size = avail_num_bytes = 0;
	}
}

void vTask_serial_sender_Thread(void* parameter)
{
    BaseType_t queueRet;
    serial_response response_to_send;

	while(1)
	{
		queueRet = xQueueReceive( xSenderQueueHandle, ( void *) &response_to_send,  portMAX_DELAY );

		if(queueRet != pdPASS )
		{
			//DebugStrLf(DEBUG_INTERFACE, "Error while getting element from queue...");
		}
		else
		{
			/*
			 * Sending Data Over USB
			 */
			/* TODO: Remove this /n */
			DebugStrLf(DEBUG_INTERFACE, (char *)response_to_send.Buffer);
			//DebugStr(DEBUG_INTERFACE, (char *)response_to_send.Buffer);
		}
	}
}

bool GUIComms_Initialize( void )
{
	BaseType_t task_ret;
    bool RetVal = true;
	system_configuration.Is_Config_Set = false;

	/*
	 * Creates a queue
	 * parameter 1 : Number of items in queue
	 * parameter 2 : Size of each item
	 */
	xReceiverQueueHandle = xQueueCreate( MAX_QUEUE_LIMIT, sizeof(application_layer_packet) );

	if (xReceiverQueueHandle == NULL)
	{
		//DebugStrLf(DEBUG_INTERFACE, "Error creating ReceiverQueue...");
		RetVal = false;
	}

	if ( true == RetVal )
	{
	    xSenderQueueHandle = xQueueCreate( MAX_QUEUE_LIMIT, sizeof(serial_response) );

	    if (xSenderQueueHandle == NULL)
        {
		    //DebugStrLf(DEBUG_INTERFACE, "Error creating SenderQueue...");
	    	RetVal = false;
	    }
	}

	if ( true == RetVal )
	{
	    task_ret = xTaskCreate( vTask_serial_receiver_Thread, "vTask_serial_receiver_Thread", 500, NULL, 1, NULL );

	    if(task_ret != pdPASS)
	    {
            //DebugStrLf(DEBUG_INTERFACE, "vTask_serial_receiver_Thread creation failed...");
	        RetVal = false;
	    }
	}

	if ( true == RetVal )
	{
	    task_ret = xTaskCreate( vTask_serial_sender_Thread, "vTask_serial_sender_Thread", 500, NULL, 1, NULL );

	    if(task_ret != pdPASS)
	    {
		    //DebugStrLf(DEBUG_INTERFACE, "vTask_serial_sender_Thread creation failed...");
	    	RetVal = false;
	    }
	}

	return RetVal;
}
