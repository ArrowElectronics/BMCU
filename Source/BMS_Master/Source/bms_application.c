/*!
********************************************************************************
* @file         :   bms_application.c
* @author       :   Srikanth Reddy Ramidi, Ashvin Ramani
* @brief        :   Processing of GUI Command
*******************************************************************************/

#include "bms_application.h"

#define CHECK_BIT(NUM, BIT) (NUM & (1<<BIT))

/* Setting Fault Related constants */
#define OPENWIRE_FAULT_FREQUENCY	20

EventGroupHandle_t  continuous_cmd_event_handle;
//Structure to hold Application layer payload for continuous commands
application_layer_packet ALpacketContinous;

static bool is_cmd_running = false;  // This flag indicates whether any command is under execution
static bool cmd_stop = false; // This flag indicates reception of stop command
static uint16_t voltage_interval_time = 0;
static const uint16_t fault_interval_time = 2000; // Setting fault interval time to 2 seconds for 1IC
static bool run_fault_cmd = false; // This flag indicates transmission of fault command
bool is_gui_connected = false; // This flag indicates wheather a connect command is issued by GUI
bool DCTOBITS[4] = { true, true, true, true }; /*!< Discharge time value /Dcto 0,1,2,3  /Programmed for 120 min*/
bool DTMEN = true; /*!< Enable Discharge Timer Monitor*/
static bool fault_group[6] = {false, false, false, false, false, false};

static bool Check_Gpio_UV(int16_t Gpio_Value);
static bool Check_Gpio_OV(int16_t Gpio_Value);
static bool Check_DIE_UT(double DIET_Value);
static bool Check_DIE_OT(double DIET_Value);
static bool Check_VREGD_UV(uint16_t VREGD_Value);
static bool Check_VREGD_OV(uint16_t VREGD_Value);
static bool Check_VREG_UV(uint16_t VREG_Value);
static bool Check_VREG_OV(uint16_t VREG_Value);
static void Run_GUI_Start_Cell_Vol_Command(application_layer_packet *Message_Payload, uint8_t *data_vol, uint8_t data_length);

/* Commands for Cell Voltage conversion and reading, these should not be alterted hence taken as const */
const uint8_t GUI_Start_Vol_Cmds[TOTAL_GUI_START_VOL_READ_CMDS][ADBMS1818_CMD_SIZE] = {
		{0x03, 0x60, 0xf4, 0x6c}, // ADC Conv payload
		{0x07, 0x14, 0xf3, 0x6c}, // poll ADC payload
		{0x00, 0x04, 0x07, 0xc2}, // RDCVA payload
		{0x00, 0x06, 0x9a, 0x94}, // RDCVB payload
		{0x00, 0x08, 0x5e, 0x52}, // RDCVC payload
		{0x00, 0x0a, 0xc3, 0x04}, // RDCVD payload
		{0x00, 0x09, 0xd5, 0x60}, // RDCVE payload
		{0x00, 0x0b, 0x48, 0x36}  // RDCVF payload
};

/* Commands for Stat Voltage conversion and reading, these should not be alterted hence taken as const */
const uint8_t STAT_VOL_CMDs[TOTAL_GUI_START_VOL_READ_CMDS][ADBMS1818_CMD_SIZE] = {
		{0x05, 0x68, 0x3b, 0xae}, // ADC Conv payload
		{0x00, 0x10, 0xed, 0x72}, // RDSTATA payload
		{0x00, 0x12, 0x70, 0x24}, // RDSTATB payload
};

/* Commands for Aux Voltage conversion and reading, these should not be alterted hence taken as const */
const uint8_t AUX_VOL_CMDs[TOTAL_GUI_START_VOL_READ_CMDS][ADBMS1818_CMD_SIZE] = {
		{0x05, 0x60, 0xd3, 0xa0}, // ADC Conv payload
		{0x00, 0x0c, 0xef, 0xcc}, // RDAUXA payload
		{0x00, 0x0e, 0x72, 0x9a}, // RDAUXB payload
		{0x00, 0x0d, 0x64, 0xfe}, // RDAUXC payload
		{0x00, 0x0f, 0xf9, 0xa8}, // RDAUXD payload
};

/* Commands for reading CFGA registers, these should not be alterted hence taken as const */
const uint8_t RDCFG_Cmd[ADBMS1818_CMD_SIZE] = {
		0x00,0x02,0x2b,0x0a
};

/* Commands for reading CFGB registers, these should not be alterted hence taken as const */
const uint8_t RDCFGB_Cmd[ADBMS1818_CMD_SIZE] = {
		0x00, 0x26, 0x2c, 0xc8
};

/* Commands for writing CFGA registers, these should not be altered hence taken as const */
const uint8_t WRCFG_Cmd[ADBMS1818_CMD_SIZE] = {
		0x00,0x01,0x3d,0x6e
};

/* Commands for writing CFGB registers, these should not be altered hence taken as const */
const uint8_t WRCFGB_Cmd[ADBMS1818_CMD_SIZE] = {
		0x00,0x24,0xb1,0x9e
};

void Set_Fault_Group( uint8_t EventGroup )
{
	uint8_t falutGroup = EventGroup;
    uint8_t loop;
    bool isOneFaultEnabled = false;

    for ( loop = CELL_OV_UV; loop < EN_FAULT_FLAG; loop++ )
    {
	    if ( false != CHECK_BIT(falutGroup, loop) )
	    {
	        fault_group[loop] = true;
	        isOneFaultEnabled = true;
	    }
	    else
	    {
		    fault_group[loop] = false;
	    }
    }

    fault_group[EN_FAULT_FLAG] = isOneFaultEnabled;
}

int8_t Set_Cell_Voltage_Threshold(void)
{
	int8_t ret;
	CELL_ASIC BMS_IC[system_configuration.IC_Count];
	uint8_t rx_cfgadata[1025], rx_cfgbdata[1025], BYTES_IN_REG_W_CRC=8, tx_data[1536] = {0}, BYTES_IN_REG_WO_CRC=6, data_index=0, cmd_index=0;
	uint16_t CRC=0;

	/* Read Back data before configuring settings and then transmit the same data with changed of required settings so that no other values will be disturbed */
	ret = Send_Command_Read_Data(RDCFG_Cmd, ADBMS1818_CMD_SIZE, rx_cfgadata, system_configuration.IC_Count*BYTES_IN_REG_W_CRC,  system_configuration.IC_Count);

	/* Read Back data before configuring settings and then transmit the same data with changed of required settings so that no other values will be disturbed */
	ret &= Send_Command_Read_Data(RDCFGB_Cmd, ADBMS1818_CMD_SIZE, rx_cfgbdata, system_configuration.IC_Count*BYTES_IN_REG_W_CRC,  system_configuration.IC_Count);

	for(uint8_t ic_loop=0;ic_loop<system_configuration.IC_Count;ic_loop++)
	{
		data_index = ic_loop*BYTES_IN_REG_W_CRC;
		cmd_index=0;

		for(uint8_t data_loop=data_index;data_loop<data_index+BYTES_IN_REG_WO_CRC;data_loop++,cmd_index++)
		{
			BMS_IC[ic_loop].config.tx_data[cmd_index] = rx_cfgadata[data_loop];  /* CFGA */
			BMS_IC[ic_loop].configb.tx_data[cmd_index] = rx_cfgbdata[data_loop]; /* CFGB */
		}

		ADBMS1818_Set_CFG_UV(ic_loop, BMS_IC,system_configuration.cell_under_voltage_threshold);
		ADBMS1818_Set_CFG_OV(ic_loop, BMS_IC,system_configuration.cell_over_voltage_threshold);
		ADBMS1818_Set_CFG_Dcto(ic_loop, BMS_IC, DCTOBITS);
		ADBMS1818_Set_CFGB_Dtmen(ic_loop, BMS_IC, DTMEN);
		BMS_IC[ic_loop].config.tx_data[0] = 0xe0;

		CRC = Calculate_CRC(BYTES_IN_REG_WO_CRC, BMS_IC[ic_loop].config.tx_data);
		BMS_IC[ic_loop].config.tx_data[cmd_index] = (uint8_t)(CRC >> 8);
		BMS_IC[ic_loop].config.tx_data[cmd_index+1] = (uint8_t)CRC;

		CRC = Calculate_CRC(BYTES_IN_REG_WO_CRC, BMS_IC[ic_loop].configb.tx_data);
		BMS_IC[ic_loop].configb.tx_data[cmd_index] = (uint8_t)(CRC >> 8);
		BMS_IC[ic_loop].configb.tx_data[cmd_index+1] = (uint8_t)CRC;
	}

	/* Write ConfigA Register */
	/*
	 * ADBMS Write command would look like 4 bytes adbms command then 8 bytes of data for each IC
	 * Here Copying the ADBMS1818 Command
	 */
	for(uint8_t index=0;index<ADBMS1818_CMD_SIZE;index++)
	{
		tx_data[index] = WRCFG_Cmd[index];
	}

	cmd_index = ADBMS1818_CMD_SIZE;

	/*
	 * Copying the data to be transmitted to ADBMS in tx_data buffer
	 */
	for(uint8_t ic_index=0;ic_index<system_configuration.IC_Count;ic_index++)
	{
		for(uint8_t data_index=0;data_index<BYTES_IN_REG_W_CRC;data_index++)
		{
			tx_data[cmd_index+data_index] = BMS_IC[ic_index].config.tx_data[data_index];
		}

		cmd_index+=BYTES_IN_REG_W_CRC;
	}

	ret &= Send_Command(tx_data, ADBMS1818_CMD_SIZE + (BYTES_IN_REG_W_CRC*system_configuration.IC_Count), system_configuration.IC_Count);


	/* Write ConfigB Register */
	/*
	 * ADBMS Write command would look like 4 bytes adbms command then 8 bytes of data for each IC
	 * Here Copying the ADBMS1818 Command
	 */
	for(uint8_t index=0;index<ADBMS1818_CMD_SIZE;index++)
	{
		tx_data[index] = WRCFGB_Cmd[index];
	}

	cmd_index = ADBMS1818_CMD_SIZE;

	/*
	 * Copying the data to be transmitted to ADBMS in tx_data buffer
	 */
	for(uint8_t ic_index=0;ic_index<system_configuration.IC_Count;ic_index++)
	{
		for(uint8_t data_index=0;data_index<BYTES_IN_REG_W_CRC;data_index++)
		{
			tx_data[cmd_index+data_index] = BMS_IC[ic_index].configb.tx_data[data_index];
		}

		cmd_index+=BYTES_IN_REG_W_CRC;
	}

	ret &= Send_Command(tx_data, ADBMS1818_CMD_SIZE + (BYTES_IN_REG_W_CRC*system_configuration.IC_Count), system_configuration.IC_Count);

    return ret;
}

void Set_System_Configuration(application_layer_packet *Message_Payload)
{
	system_configuration.IC_Count = Message_Payload->IC_Count;

	for(int index=0;index<MAX_SLAVE_IC_NUMBER_BYTES;index++)
	{
		system_configuration.IC_Number[index] =Message_Payload->IC_Number[index];
	}

	for(int index=0;index<MAX_SLAVE_IC_COUNT;index++)
	{
		system_configuration.IC_Types[index] =Message_Payload->IC_Types[index];
	}

	if(Message_Payload->DL == CONFIGURATION_DATA_SIZE)
	{
		voltage_interval_time = Message_Payload->Data[0] << 8;
		voltage_interval_time |= ( ( (uint16_t)Message_Payload->Data[1] ) & 0x00FFU );
#if 0
		fault_interval_time = Message_Payload->Data[2] << 8;
		fault_interval_time |= ( ( (uint16_t)Message_Payload->Data[3] ) & 0x00FFU );
#endif

		system_configuration.cell_under_voltage_threshold = Message_Payload->Data[2] << 8;
		system_configuration.cell_under_voltage_threshold |= ( ( (uint16_t)Message_Payload->Data[3] ) & 0x00FFU );

		system_configuration.cell_over_voltage_threshold = Message_Payload->Data[4] << 8;
		system_configuration.cell_over_voltage_threshold |= ( ( (uint16_t)Message_Payload->Data[5] ) & 0x00FFU );

		Set_Fault_Group(Message_Payload->Data[6]);
	}

	system_configuration.Is_Config_Set = true;

	Set_Cell_Voltage_Threshold();
}

int8_t Run_ADBMS1818_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length)
{
	int8_t response = E_NO_ERROR;

	if(Message_Payload->Op_Code == READ)
	{
		response = Send_Command_Read_Data(Message_Payload->Data, Message_Payload->DL, data, data_length, system_configuration.IC_Count);
	}
	else if(Message_Payload->Op_Code == WRITE)
	{
		response = Send_Command(Message_Payload->Data, Message_Payload->DL, system_configuration.IC_Count);
	}

	return response;
}

void Create_Faults_buffer(uint8_t nIC)
{
	if( ( system_configuration.Is_Config_Set == true ) && (is_cmd_running != true ) )
	{
		is_cmd_running = true;

		application_layer_packet Message_Payload;
		uint8_t data[nIC*RESPONSE_SIZE], BYTES_IN_REG_W_CRC=8, BYTES_IN_REG_WO_CRC=6, Fault_Buffer[nIC][FAULT_PAYLOAD_SIZE], Response_Data[FAULT_PAYLOAD_SIZE*nIC];
		int8_t response = E_NO_ERROR;
		static int8_t opencells[MAX_IC_FOR_OPENCELLS][N_CELL_CHANNELS], num_of_open_wires[MAX_IC_FOR_OPENCELLS], fault_counter = 0; // This variable holds the fault frequency value
		int32_t Gpio_Value=0, VReg=0, VRegD=0;
		bool RetValue, pollStatus = false;
		double DIE_Temp=0;

		for(uint8_t ic_index=0,resp_index=0;ic_index<nIC;ic_index++)
		{
			for(uint8_t inner_index=0;inner_index<FAULT_PAYLOAD_SIZE;inner_index++,resp_index++)
			{
				Fault_Buffer[ic_index][inner_index] = 0;
				Response_Data[resp_index] = 0;
			}
		}


		if ( fault_group[SYSTEM_FAULT] )
		{
		    /* Setting SPI Fault */
		    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
		    {
			    if(spi_fault == true)
			    {
				    // If SPI fault found then setting the fault to all IC's responses
				    for(uint8_t inner_ic_index=0;inner_ic_index<nIC;inner_ic_index++)
				    {
					    Fault_Buffer[inner_ic_index][AFE_COMM_SPI_FAIL_BYTE_IN_FAULT_PAYLOAD] = SPI_FAIL_BIT_MASK_IN_FAULT_PAYLOAD;
				    }
				    break;
			    }
		    }
		}

		if ( fault_group[CELL_OV_UV] )
		{
		    taskENTER_CRITICAL();
		    // Converting Cell voltages so that w.r.t latest converted cell values we will get OV/UV faults
		    response = Send_Command((uint8_t *)GUI_Start_Vol_Cmds[CELL_ADC_CONVERSION], ADBMS1818_CMD_SIZE, system_configuration.IC_Count);
		    taskEXIT_CRITICAL();

		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    pollStatus = true;
		}

		if ( fault_group[CELL_OV_UV] || fault_group[SYSTEM_FAULT] || fault_group[OTHER_OV_UV] )
		{
		    taskENTER_CRITICAL();
		    // ADC conversion of STAT voltages
		    response = Send_Command(STAT_VOL_CMDs[STAT_ADC_CONVERSION], ADBMS1818_CMD_SIZE, system_configuration.IC_Count);

		    taskEXIT_CRITICAL();

		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    pollStatus = true;
		}

		if ( fault_group[GPIO_OV_UV] || fault_group[CELL_OV_UV] )
		{
		    // ADC conversion of AUX voltages
		    taskENTER_CRITICAL();
		    response = Send_Command(AUX_VOL_CMDs[AUX_ADC_CONVERSION], ADBMS1818_CMD_SIZE, system_configuration.IC_Count);
		    taskEXIT_CRITICAL();

		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    pollStatus = true;
		}

		if ( true == pollStatus )
		{
		    taskENTER_CRITICAL();
		    ADBMS1818_PollADC(); /* Polling ADC conversion */
		    taskEXIT_CRITICAL();
		}

		if ( fault_group[SYSTEM_FAULT] || fault_group[OTHER_OV_UV] )
		{
		    taskENTER_CRITICAL();
		    // Reading Stat register group A
		    response = Send_Command_Read_Data(STAT_VOL_CMDs[STAT_VOL_REG_GROUP_A], ADBMS1818_CMD_SIZE, data, RESPONSE_SIZE*system_configuration.IC_Count, system_configuration.IC_Count);
		    taskEXIT_CRITICAL();

		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
		    {
		    	if ( fault_group[SYSTEM_FAULT] )
		    	{
			        // Checking CRC of the received bytes if CRC is 0xffff then setting AFE Comm fail bit
			        if( ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + BYTES_IN_REG_WO_CRC] == 0xff ) &&  ( data[( ic_index*BYTES_IN_REG_W_CRC ) + BYTES_IN_REG_WO_CRC + 1] == 0xff ) )
			        {
				        // If AFE COMM fault found then setting the fault to all IC's responses
				        for(uint8_t inner_ic_index=0;inner_ic_index<nIC;inner_ic_index++)
				        {
					        Fault_Buffer[inner_ic_index][AFE_COMM_SPI_FAIL_BYTE_IN_FAULT_PAYLOAD] = AFE_COMM_FAIL_BIT_MASK_IN_FAULT_PAYLOAD;
				        }
			        }
		    	}

		    	if ( fault_group[OTHER_OV_UV] )
		    	{
			        // Extracting DIE Temperature
			        DIE_Temp = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + DIET_LSB_IN_REG_STATB ] + ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + DIET_MSB_IN_REG_STATB ]<<8 );
			        DIE_Temp = ((DIE_Temp*(0.0001 / 0.0076)) - 276);

			        RetValue = Check_DIE_UT(DIE_Temp);

			        if(RetValue == true)
			        {
				        Fault_Buffer[ic_index][OV_UV_VA_VD_STK_DIET_IN_FAULT_PAYLOAD] = DIE_UT_BIT_MASK_IN_FAULT_PAYLOAD;
			        }

			        RetValue = Check_DIE_OT(DIE_Temp);

			        if(RetValue == true)
			        {
				        Fault_Buffer[ic_index][OV_UV_VA_VD_STK_DIET_IN_FAULT_PAYLOAD] |= DIE_OT_BIT_MASK_IN_FAULT_PAYLOAD;
			        }

			        // Extracting VReg Analog value
			        VReg = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + VREG_LSB_IN_REG_STATA ] + ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + VREG_MSB_IN_REG_STATA ]<<8 );

			        RetValue = Check_VREG_UV(VReg);

			        if(RetValue == true)
			        {
				        Fault_Buffer[ic_index][OV_UV_VA_VD_STK_DIET_IN_FAULT_PAYLOAD] |= VA_UV_BIT_MASK_IN_FAULT_PAYLOAD;
			        }

			        RetValue = Check_VREG_OV(VReg);

			        if(RetValue == true)
			        {
				        Fault_Buffer[ic_index][OV_UV_VA_VD_STK_DIET_IN_FAULT_PAYLOAD] |= VA_OV_BIT_MASK_IN_FAULT_PAYLOAD;
			        }
		        }
		    }
		}

		if ( fault_group[CELL_OV_UV] || fault_group[OTHER_OV_UV] )
		{
		    taskENTER_CRITICAL();
		    // Reading Stat register group B
		    response = Send_Command_Read_Data(STAT_VOL_CMDs[STAT_VOL_REG_GROUP_B], ADBMS1818_CMD_SIZE, data, RESPONSE_SIZE*system_configuration.IC_Count, system_configuration.IC_Count);
		    taskEXIT_CRITICAL();

		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
		    {
		    	if ( fault_group[CELL_OV_UV] )
		    	{
			        Fault_Buffer[ic_index][OV_UV_C1_TO_C4_IN_FAULT_PAYLOAD] |= data[ ( ic_index*BYTES_IN_REG_W_CRC ) + OV_UV_C1_TO_C4_IN_REG_STATB ];
			        Fault_Buffer[ic_index][OV_UV_C5_TO_C8_IN_FAULT_PAYLOAD] |= data[ ( ic_index*BYTES_IN_REG_W_CRC ) + OV_UV_C5_TO_C8_IN_REG_STATB ];
			        Fault_Buffer[ic_index][OV_UV_C9_TO_C12_IN_FAULT_PAYLOAD] |= data[ ( ic_index*BYTES_IN_REG_W_CRC ) + OV_UV_C9_TO_C12_IN_REG_STATB ];
		    	}

		    	if ( fault_group[OTHER_OV_UV] )
		    	{
			        VRegD = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + VREGD_LSB_IN_REG_STATB ] + ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + VREGD_MSB_IN_REG_STATB ]<<8 );

			        RetValue = Check_VREGD_UV(VRegD);

			        if(RetValue == true)
			        {
				        Fault_Buffer[ic_index][OV_UV_VA_VD_STK_DIET_IN_FAULT_PAYLOAD] |= VD_UV_BIT_MASK_IN_FAULT_PAYLOAD;
			        }

			        RetValue = Check_VREGD_OV(VRegD);

			        if(RetValue == true)
			        {
				        Fault_Buffer[ic_index][OV_UV_VA_VD_STK_DIET_IN_FAULT_PAYLOAD] |= VD_OV_BIT_MASK_IN_FAULT_PAYLOAD;
			        }
		        }
		    }
		}

		if ( fault_group[GPIO_OV_UV] )
		{
		    /* Reading AUX Reg group A for GPIO1,2,3 voltages */
		    taskENTER_CRITICAL();
		    response = Send_Command_Read_Data(AUX_VOL_CMDs[AUX_VOL_REG_GROUP_A], ADBMS1818_CMD_SIZE, data, RESPONSE_SIZE*system_configuration.IC_Count, system_configuration.IC_Count);
		    taskEXIT_CRITICAL();

		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    //Iterating through all ICs and w.r.t above data filling all IC's GPIO OV/UV flags
		    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
		    {
			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO1_LSB_IN_REG_AUXA ] + ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO1_MSB_IN_REG_AUXA ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD] |= GPIO1_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD] |= GPIO1_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO2_LSB_IN_REG_AUXA ] + ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO2_MSB_IN_REG_AUXA ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD] |= GPIO2_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD] |= GPIO2_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO3_LSB_IN_REG_AUXA ] +
					    ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO3_MSB_IN_REG_AUXA ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD] |= GPIO3_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD] |= GPIO3_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }
		    }

		    //Reading AUX Reg group A for GPIO-4,5 voltages
		    taskENTER_CRITICAL();
		    response = Send_Command_Read_Data(AUX_VOL_CMDs[AUX_VOL_REG_GROUP_B], ADBMS1818_CMD_SIZE, data, RESPONSE_SIZE*system_configuration.IC_Count, system_configuration.IC_Count);
		    taskEXIT_CRITICAL();

		    //Iterating through all ICs and w.r.t above data filling all IC's GPIO OV/UV flags
		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
		    {
			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO4_LSB_IN_REG_AUXB ] +
								    ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO4_MSB_IN_REG_AUXB ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD] |= GPIO4_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G1_TO_G4_IN_FAULT_PAYLOAD] |= GPIO4_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO5_LSB_IN_REG_AUXB ] +
											( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO5_MSB_IN_REG_AUXB ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD] |= GPIO5_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD] = GPIO5_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }
		    }

		    //Reading AUX Reg group A for GPIO-6,7,8 voltages
		    taskENTER_CRITICAL();
		    response = Send_Command_Read_Data(AUX_VOL_CMDs[AUX_VOL_REG_GROUP_C], ADBMS1818_CMD_SIZE, data, RESPONSE_SIZE*system_configuration.IC_Count, system_configuration.IC_Count);
		    taskEXIT_CRITICAL();

		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    //Iterating through all ICs and w.r.t above data filling all IC's GPIO OV/UV flags
		    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
		    {
			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO6_LSB_IN_REG_AUXC ] +
						    ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO6_MSB_IN_REG_AUXC ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD] |= GPIO6_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD] |= GPIO6_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO7_LSB_IN_REG_AUXC ] +
							( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO7_MSB_IN_REG_AUXC ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD] |= GPIO7_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD] |= GPIO7_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO8_LSB_IN_REG_AUXC ] +
						   ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO8_MSB_IN_REG_AUXC ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD] |= GPIO8_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G5_TO_G8_IN_FAULT_PAYLOAD] |= GPIO8_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }
		    }
		}

		if ( fault_group[GPIO_OV_UV] || fault_group[CELL_OV_UV] )
		{
		    //Reading AUX Reg group A for GPIO-9 voltages
		    taskENTER_CRITICAL();
		    response = Send_Command_Read_Data(AUX_VOL_CMDs[AUX_VOL_REG_GROUP_D], ADBMS1818_CMD_SIZE, data, RESPONSE_SIZE*system_configuration.IC_Count, system_configuration.IC_Count);
		    taskEXIT_CRITICAL();

		    if(response != E_NO_ERROR)
		    {
			    goto error;
		    }

		    //Iterating through all ICs and w.r.t above data filling all IC's GPIO OV/UV flags
		    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
		    {
			    Gpio_Value = data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO9_LSB_IN_REG_AUXD ] +
						   ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + GPIO9_MSB_IN_REG_AUXD ]<<8 );

			    RetValue = Check_Gpio_UV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G9_IN_FAULT_PAYLOAD] |= GPIO9_UV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    RetValue = Check_Gpio_OV(Gpio_Value);

			    if(RetValue == true)
			    {
				    Fault_Buffer[ic_index][OV_UV_G9_IN_FAULT_PAYLOAD] |= GPIO9_OV_BIT_MASK_POS_IN_FAULT_PAYLOAD;
			    }

			    if ( fault_group[CELL_OV_UV] )
			    {
			        Fault_Buffer[ic_index][OV_UV_C13_TO_C16_IN_FAULT_PAYLOAD] |= ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + OV_UV_C13_TO_C16_IN_REG_AUXD ] );
			        Fault_Buffer[ic_index][OV_UV_C17_TO_C18_IN_FAULT_PAYLOAD] |= ( data[ ( ic_index*BYTES_IN_REG_W_CRC ) + OV_UV_C17_TO_C18_IN_REG_AUXD ] & 0x0F );
			    }
			}
		}

		if ( fault_group[CELL_OPEN_WIRED] )
		{
			fault_counter++;

			// Execute Open wire detection less frequently compared to other faults
			if( fault_counter == OPENWIRE_FAULT_FREQUENCY )
			{
			    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
			    {
				    num_of_open_wires[ic_index] = 0;
			    }

			    taskENTER_CRITICAL();
			    Get_ADBMS1818_Cell_OpenWires(nIC, opencells, num_of_open_wires);
			    taskEXIT_CRITICAL();

				fault_counter = 0;
			}


		    for(uint8_t ic_index=0;ic_index<nIC;ic_index++)
		    {
			    for(uint8_t data_index=0;data_index<num_of_open_wires[ic_index];data_index++)
			    {
				    if( ( opencells[ic_index][data_index] >= CELL_1_OPEN_WIRE ) && (opencells[ic_index][data_index] <= CELL_8_OPEN_WIRE))
				    {
					    if(opencells[ic_index][data_index] == CELL_1_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C1_TO_C8_IN_FAULT_PAYLOAD] |= CELL1_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_2_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C1_TO_C8_IN_FAULT_PAYLOAD] |= CELL2_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_3_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C1_TO_C8_IN_FAULT_PAYLOAD] |= CELL3_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_4_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C1_TO_C8_IN_FAULT_PAYLOAD] |= CELL4_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_5_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C1_TO_C8_IN_FAULT_PAYLOAD] |= CELL5_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_6_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C1_TO_C8_IN_FAULT_PAYLOAD] |= CELL6_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_7_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C1_TO_C8_IN_FAULT_PAYLOAD] |= CELL7_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_8_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C1_TO_C8_IN_FAULT_PAYLOAD] |= CELL8_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
				    }
				    else if( ( opencells[ic_index][data_index] >= CELL_9_OPEN_WIRE ) && (opencells[ic_index][data_index] <= CELL_16_OPEN_WIRE))
				    {
					    if(opencells[ic_index][data_index] == CELL_9_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C9_TO_C16_IN_FAULT_PAYLOAD] |= CELL9_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_10_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C9_TO_C16_IN_FAULT_PAYLOAD] |= CELL10_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_11_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C9_TO_C16_IN_FAULT_PAYLOAD] |= CELL11_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_12_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C9_TO_C16_IN_FAULT_PAYLOAD] |= CELL12_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_13_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C9_TO_C16_IN_FAULT_PAYLOAD] |= CELL13_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_14_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C9_TO_C16_IN_FAULT_PAYLOAD] |= CELL14_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_15_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C9_TO_C16_IN_FAULT_PAYLOAD] |= CELL15_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_16_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C9_TO_C16_IN_FAULT_PAYLOAD] |= CELL16_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
				    }
				    else if( ( opencells[ic_index][data_index] >= CELL_17_OPEN_WIRE ) && (opencells[ic_index][data_index] <= CELL_18_OPEN_WIRE ))
				    {
					    if(opencells[ic_index][data_index] == CELL_17_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C17_C18_IN_FAULT_PAYLOAD] |= CELL17_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
					    else if(opencells[ic_index][data_index] == CELL_18_OPEN_WIRE)
					    {
						    Fault_Buffer[ic_index][OW_C17_C18_IN_FAULT_PAYLOAD] |= CELL18_OPENWIRE_BIT_MASK_IN_FAULT_PAYLOAD;
					    }
				    }
			    }
		    }
		}

		for(uint8_t index=0;index<system_configuration.IC_Count;index++)
		{
			for(uint8_t inner_index=0;inner_index<FAULT_PAYLOAD_SIZE;inner_index++)
			{
				Response_Data[(index*FAULT_PAYLOAD_SIZE) + inner_index] = Fault_Buffer[index][inner_index];
			}
		}

		Message_Payload.Op_Code = FAULT_MESSAGE;
		Message_Payload.IC_Count = system_configuration.IC_Count;

		for (uint8_t lp = 0U; lp < MAX_SLAVE_IC_NUMBER_BYTES; lp++ )
		{
			Message_Payload.IC_Number[lp] = system_configuration.IC_Number[lp];
		}

		is_cmd_running = false;

		fill_and_send_response_packet_to_queue( &Message_Payload, Response_Data, 0, COMMAND_ACCEPTED );

		error:
		if(response != E_NO_ERROR)
		{
			response = SPI_ISOSPI_COMM_ERROR;
			//fill_and_send_response_packet_to_queue( Message_Payload, data_vol, system_configuration.IC_Count*VOL_READ_IC_RESPONSE_SIZE, response );
		}
	}
}

static bool Check_Gpio_UV(int16_t Gpio_Value)
{
	if(Gpio_Value < GPIO_UV_THRESHOLD)
	{
		return true;
	}

	return false;
}

static bool Check_Gpio_OV(int16_t Gpio_Value)
{
	if(Gpio_Value > GPIO_OV_THRESHOLD)
	{
		return true;
	}

	return false;
}

static bool Check_DIE_UT(double DIET_Value)
{
	if(DIET_Value < DIE_UT_THRESHOLD)
	{
		return true;
	}

	return false;
}

static bool Check_DIE_OT(double DIET_Value)
{
	if(DIET_Value > DIE_OT_THRESHOLD)
	{
		return true;
	}

	return false;
}

static bool Check_VREGD_UV(uint16_t VREGD_Value)
{
	if(VREGD_Value < VD_UV_THRESHOLD)
	{
		return true;
	}

	return false;
}

static bool Check_VREGD_OV(uint16_t VREGD_Value)
{
	if(VREGD_Value > VD_OV_THRESHOLD)
	{
		return true;
	}

	return false;
}

static bool Check_VREG_UV(uint16_t VREG_Value)
{
	if(VREG_Value < VA_UV_THRESHOLD)
	{
		return true;
	}

	return false;
}

static bool Check_VREG_OV(uint16_t VREG_Value)
{
	if(VREG_Value > VA_OV_THRESHOLD)
	{
		return true;
	}

	return false;
}

void Run_OneShot_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length)
{
	/* Setting the is_cmd_running flag to true , as we are executing the command */
	is_cmd_running = true;

	int8_t response;

	response = Run_ADBMS1818_Command(Message_Payload, data, data_length);

	if(response != E_NO_ERROR)
	{
		response = SPI_ISOSPI_COMM_ERROR;
	}
	else
	{
		response = COMMAND_ACCEPTED;
	}

	fill_and_send_response_packet_to_queue( Message_Payload, data, data_length, response );

	/* Setting the is_cmd_running flag to false , as the command execution is done */
	is_cmd_running = false;
}

void Run_Continuous_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length)
{
	/* Setting the is_cmd_running flag to true , as we are executing the command */
	is_cmd_running = true;

	int8_t response;

	while( false == cmd_stop )
	{
		response = Run_ADBMS1818_Command(Message_Payload, data, data_length);

		if(response != E_NO_ERROR)
		{
			response = SPI_ISOSPI_COMM_ERROR;
		}
		else
		{
			response = COMMAND_ACCEPTED;
		}

		fill_and_send_response_packet_to_queue( Message_Payload, data, data_length, response );
	}

	/* Setting the is_cmd_running flag to false , as the command execution is done */
	is_cmd_running = false;
}

void Run_Stop_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length)
{
	// Setting cmd_stop flag to true so that if any continuous command is running then it will stop
	data_length = 0;
	cmd_stop = true;
	run_fault_cmd = false;
	fill_and_send_response_packet_to_queue( Message_Payload, data, data_length, COMMAND_ACCEPTED );
}

void Run_GUI_Start_Vol_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length)
{
	uint8_t data_cell_vol[VOL_READ_IC_RESPONSE_SIZE*system_configuration.IC_Count], data_aux_vol[AUX_VOL_READ_RESPONSE_SIZE*system_configuration.IC_Count], data_stat_vol[STAT_VOL_READ_RESPONSE_SIZE*system_configuration.IC_Count], response_data[TOTAL_START_VOL_RESPONSE_SIZE_EACH_IC*system_configuration.IC_Count];
	uint16_t response_index=0;

	while( false == cmd_stop )
	{
		if(is_cmd_running == false)
		{
			/* Setting the is_cmd_running flag to true , as we are executing the command */
			is_cmd_running = true;

			run_fault_cmd = true;

			memset(data_cell_vol, 0, VOL_READ_IC_RESPONSE_SIZE*system_configuration.IC_Count);
			memset(data_aux_vol, 0, AUX_VOL_READ_RESPONSE_SIZE*system_configuration.IC_Count);
			memset(data_stat_vol, 0, STAT_VOL_READ_RESPONSE_SIZE*system_configuration.IC_Count);
			memset(response_data, 0, TOTAL_START_VOL_RESPONSE_SIZE_EACH_IC*system_configuration.IC_Count);


			Run_GUI_Start_Cell_Vol_Command(Message_Payload, data_cell_vol, data_length);

			Run_GUI_Start_Stat_Vol_Command(Message_Payload, data_stat_vol, data_length);

			Run_GUI_Start_Aux_Vol_Command(Message_Payload, data_aux_vol, data_length);

			response_index = 0;

			for(uint8_t ic_index=0;ic_index<system_configuration.IC_Count;ic_index++)
			{
				response_data[response_index++] = CELL_VOL_DATA_INDICATOR;
				response_data[response_index++] = VOL_READ_IC_RESPONSE_SIZE;

				for(uint8_t response_loop=0;response_loop<VOL_READ_IC_RESPONSE_SIZE;response_loop++)
				{
					response_data[response_index++] = data_cell_vol[ (ic_index*VOL_READ_IC_RESPONSE_SIZE) + response_loop];
				}

				response_data[response_index++] = AUX_VOL_DATA_INDICATOR;
				response_data[response_index++] = AUX_VOL_READ_RESPONSE_SIZE;

				for(uint8_t response_loop=0;response_loop<AUX_VOL_READ_RESPONSE_SIZE;response_loop++)
				{
					response_data[response_index++] = data_aux_vol[ (ic_index*AUX_VOL_READ_RESPONSE_SIZE) + response_loop];
				}

				response_data[response_index++] = STAT_VOL_DATA_INDICATOR;
				response_data[response_index++] = STAT_VOL_READ_RESPONSE_SIZE;

				for(uint8_t response_loop=0;response_loop<STAT_VOL_READ_RESPONSE_SIZE;response_loop++)
				{
					response_data[response_index++] = data_stat_vol[ (ic_index*STAT_VOL_READ_RESPONSE_SIZE) + response_loop];
				}
			}
			fill_and_send_response_packet_to_queue( Message_Payload, response_data, system_configuration.IC_Count*TOTAL_START_VOL_RESPONSE_SIZE_EACH_IC, COMMAND_ACCEPTED );

			is_cmd_running = false;
		}

		Delay_m(voltage_interval_time);
	}


	/* Setting the is_cmd_running flag to false , as the command execution is done */
	is_cmd_running = false;

}

static void Run_GUI_Start_Cell_Vol_Command(application_layer_packet *Message_Payload, uint8_t *data_vol, uint8_t data_length)
{
	uint8_t data[system_configuration.IC_Count*RESPONSE_SIZE];
	int8_t response;

	taskENTER_CRITICAL();

	response = Send_Command((uint8_t *)GUI_Start_Vol_Cmds[CELL_ADC_CONVERSION], ADBMS1818_CMD_SIZE, system_configuration.IC_Count);
	taskEXIT_CRITICAL();

	if(response != E_NO_ERROR)
	{
		goto error;
	}

	taskENTER_CRITICAL();
	ADBMS1818_PollADC();
	taskEXIT_CRITICAL();

	/* Loop to iterate through each voltage register group(A-F) */
	for(uint8_t loop=CELL_VOL_REG_GROUP_A; loop<=CELL_VOL_REG_GROUP_F;loop++)
	{
		/*
		 *Commands to read each stat voltage register group(A-B) and receives data in data field
		 *The data field consists data 0-7 bytes ic-1, 8-15 bytes ic 2 and so on
		 */

		taskENTER_CRITICAL();
		response = Send_Command_Read_Data((uint8_t *)GUI_Start_Vol_Cmds[loop], ADBMS1818_CMD_SIZE, data, system_configuration.IC_Count*RESPONSE_SIZE, system_configuration.IC_Count);
		taskEXIT_CRITICAL();

		if(response != E_NO_ERROR)
		{
			goto error;
		}

		/*
		 * loop to iterate for each IC
		 */
		for(uint8_t ic_loop=0;ic_loop<system_configuration.IC_Count;ic_loop++)//2;ic_loop++)
		{
			/*
			 * loop to iterate through whole response
			 */
			for(uint8_t response_loop=0;response_loop<RESPONSE_SIZE;response_loop++)
			{
				/*
				 * voltage data for each IC is of size STAT_VOL_READ_RESPONSE_SIZE(16)
				 * so stat voltage reg group-A values for IC-1 should store in indices from 0-7, for IC-2 it should be in index 16+0 to 16+7
				 * From above statement the below assignment is derived
				 */
				data_vol[ic_loop*VOL_READ_IC_RESPONSE_SIZE + ( (loop-CELL_VOL_REG_GROUP_A)*RESPONSE_SIZE + response_loop)] = data[(ic_loop*RESPONSE_SIZE)+response_loop];
			}
		}
	}

	error:
	if(response != E_NO_ERROR)
	{
		response = SPI_ISOSPI_COMM_ERROR;
		fill_and_send_response_packet_to_queue( Message_Payload, data_vol, system_configuration.IC_Count*VOL_READ_IC_RESPONSE_SIZE, response );
	}
}

void Run_GUI_Start_Stat_Vol_Command(application_layer_packet *Message_Payload, uint8_t *data_vol, uint8_t data_length)
{
	uint8_t data[system_configuration.IC_Count*RESPONSE_SIZE];
	int8_t response;

	taskENTER_CRITICAL();
	response = Send_Command((uint8_t *)STAT_VOL_CMDs[STAT_ADC_CONVERSION], ADBMS1818_CMD_SIZE, system_configuration.IC_Count);
	taskEXIT_CRITICAL();

	if(response != E_NO_ERROR)
	{
		goto error;
	}

	taskENTER_CRITICAL();
	ADBMS1818_PollADC();
	taskEXIT_CRITICAL();

	/* Loop to iterate through each voltage register group(A-F) */
	for(uint8_t loop=STAT_VOL_REG_GROUP_A; loop<=STAT_VOL_REG_GROUP_B;loop++)
	{
		/*
		 *Commands to read each stat voltage register group(A-B) and receives data in data field
		 *The data field consists data 0-7 bytes ic-1, 8-15 bytes ic 2 and so on
		 */
		taskENTER_CRITICAL();
		response = Send_Command_Read_Data((uint8_t *)STAT_VOL_CMDs[loop], ADBMS1818_CMD_SIZE, data, system_configuration.IC_Count*RESPONSE_SIZE, system_configuration.IC_Count);
		taskEXIT_CRITICAL();

		if(response != E_NO_ERROR)
		{
			goto error;
		}

		/*
		 * loop to iterate for each IC
		 */
		for(uint8_t ic_loop=0;ic_loop<system_configuration.IC_Count;ic_loop++)//2;ic_loop++)
		{
			/*
			 * loop to iterate through whole response
			 */
			for(uint8_t response_loop=0;response_loop<RESPONSE_SIZE;response_loop++)
			{
				/*
				 * voltage data for each IC is of size STAT_VOL_READ_RESPONSE_SIZE(16)
				 * so stat voltage reg group-A values for IC-1 should store in indices from 0-7, for IC-2 it should be in index 16+0 to 16+7
				 * From above statement the below assignment is derived
				 */
				data_vol[ic_loop*STAT_VOL_READ_RESPONSE_SIZE + ( (loop-STAT_VOL_REG_GROUP_A)*RESPONSE_SIZE + response_loop)] = data[(ic_loop*RESPONSE_SIZE)+response_loop];
			}
		}
	}

	error:
	if(response != E_NO_ERROR)
	{
		response = SPI_ISOSPI_COMM_ERROR;
		fill_and_send_response_packet_to_queue( Message_Payload, data_vol, system_configuration.IC_Count*VOL_READ_IC_RESPONSE_SIZE, response );
	}
}

void Run_GUI_Start_Aux_Vol_Command(application_layer_packet *Message_Payload, uint8_t *data_vol, uint8_t data_length)
{
	uint8_t data[system_configuration.IC_Count*RESPONSE_SIZE];
	int8_t response;

	taskENTER_CRITICAL();
	response = Send_Command((uint8_t *)AUX_VOL_CMDs[AUX_ADC_CONVERSION], ADBMS1818_CMD_SIZE, system_configuration.IC_Count);
	taskEXIT_CRITICAL();

	if(response != E_NO_ERROR)
	{
		goto error;
	}

	taskENTER_CRITICAL();
	ADBMS1818_PollADC();
	taskEXIT_CRITICAL();

	/* Loop to iterate through each voltage register group(A-F) */
	for(uint8_t loop=AUX_VOL_REG_GROUP_A; loop<=AUX_VOL_REG_GROUP_D;loop++)
	{
		/*
		 *Commands to read each stat voltage register group(A-B) and receives data in data field
		 *The data field consists data 0-7 bytes ic-1, 8-15 bytes ic 2 and so on
		 */
		taskENTER_CRITICAL();
		response = Send_Command_Read_Data((uint8_t *)AUX_VOL_CMDs[loop], ADBMS1818_CMD_SIZE, data, system_configuration.IC_Count*RESPONSE_SIZE, system_configuration.IC_Count);
		taskEXIT_CRITICAL();

		if(response != E_NO_ERROR)
		{
			goto error;
		}

		/*
		 * loop to iterate for each IC
		 */
		for(uint8_t ic_loop=0;ic_loop<system_configuration.IC_Count;ic_loop++)//2;ic_loop++)
		{
			/*
			 * loop to iterate through whole response
			 */
			for(uint8_t response_loop=0;response_loop<RESPONSE_SIZE;response_loop++)
			{
				/*
				 * voltage data for each IC is of size STAT_VOL_READ_RESPONSE_SIZE(16)
				 * so stat voltage reg group-A values for IC-1 should store in indices from 0-7, for IC-2 it should be in index 16+0 to 16+7
				 * From above statement the below assignment is derived
				 */
				data_vol[ic_loop*AUX_VOL_READ_RESPONSE_SIZE + ( (loop-AUX_VOL_REG_GROUP_A)*RESPONSE_SIZE + response_loop)] = data[(ic_loop*RESPONSE_SIZE)+response_loop];
			}
		}
	}

	error:
	if(response != E_NO_ERROR)
	{
		response = SPI_ISOSPI_COMM_ERROR;
		fill_and_send_response_packet_to_queue( Message_Payload, data_vol, system_configuration.IC_Count*VOL_READ_IC_RESPONSE_SIZE, response );
	}
}

void Run_Connect_Disconnect_Command(application_layer_packet *Message_Payload, uint8_t *data, uint8_t data_length)
{
	if(Message_Payload->Op_Code == CONNECT)
	{
		is_gui_connected = true;
	}
	else if(Message_Payload->Op_Code == DISCONNECT)
	{
		is_gui_connected = false;
	}

	fill_and_send_response_packet_to_queue( Message_Payload, data, 0, COMMAND_ACCEPTED );
}

void Command_Execution_Thread(void* parameter)
{
	application_layer_packet ALpacketfromQueue;
    BaseType_t queueRet;
    uint8_t data[128], data_length=0;
    EventBits_t xEventGroupValue=0;

	while(1)
	{
		queueRet = xQueueReceive( Get_ReceiverQueueHandler(), ( void *) &ALpacketfromQueue,  portMAX_DELAY );

		if(queueRet != pdPASS )
		{
			//DebugStrLf(DEBUG_INTERFACE, "Error while getting element from queue...");
		}
		else
		{
			if(ALpacketfromQueue.OP_Type == STOP)
			{
				Run_Stop_Command(&ALpacketfromQueue, data, data_length);
			}
			else if(is_cmd_running == true)
			{
				fill_and_send_response_packet_to_queue( &ALpacketfromQueue, NULL, 0, PREVIOUS_COMMAND_IN_PROGRESS );
			}
			else if ( ( ALpacketfromQueue.Op_Code == READ ) || ( ALpacketfromQueue.Op_Code == WRITE ) )
			{
				if ( ALpacketfromQueue.Op_Code == READ )
				{
					data_length = system_configuration.IC_Count * RESPONSE_SIZE;
				}

				switch ( ALpacketfromQueue.OP_Type )
				{
					case ONE_SHOT:
					{
						Run_OneShot_Command(&ALpacketfromQueue, data, data_length);
						break;
					}

					case CONTINUOUS:
					{
						ALpacketContinous = ALpacketfromQueue;

						xEventGroupValue |= EVENT_CONTINUOUS;
						xEventGroupSetBits( continuous_cmd_event_handle, xEventGroupValue );
						break;
					}

					default:
					{
						/* Ignore commands because it has been already validated by comms module */
					}
				}
			}
			else if( ( ALpacketfromQueue.Op_Code == CONNECT ) || ( ALpacketfromQueue.Op_Code == DISCONNECT ) )
			{
				Run_Connect_Disconnect_Command(&ALpacketfromQueue, data, data_length);
			}
			else if ( ALpacketfromQueue.Op_Code == CONFIGURATION )
			{
				is_cmd_running = true;
				Set_System_Configuration(&ALpacketfromQueue);
				data_length = 0;
				fill_and_send_response_packet_to_queue( &ALpacketfromQueue, data, data_length, COMMAND_ACCEPTED );
				is_cmd_running = false;
			}
			else if ( ALpacketfromQueue.Op_Code == GUI_START )
			{
				ALpacketContinous = ALpacketfromQueue;

				xEventGroupValue |= EVENT_GUI_START;
				xEventGroupSetBits( continuous_cmd_event_handle, xEventGroupValue );
			}
			else
			{
				/* Ignore others command because it has been already validated by comms module */
			}
		}
	}
}

void Continuous_Command_Thread(void* parameter)
{
	EventBits_t xEventGroupValue;
    uint8_t data[128] = {0}, data_length=0;

	while(1)
	{
		xEventGroupValue = xEventGroupWaitBits( continuous_cmd_event_handle, 0xff, pdTRUE, pdFALSE, portMAX_DELAY);

		if(xEventGroupValue & EVENT_CONTINUOUS)
		{
			cmd_stop = false;
			Run_Continuous_Command(&ALpacketContinous, data, data_length);
		}
		else if(xEventGroupValue & EVENT_GUI_START)
		{
			cmd_stop = false;
			Run_GUI_Start_Vol_Command(&ALpacketContinous, data, data_length);
		}
		else
		{
			/* do nothing */
		}
	}
}

void Faults_Thread (void *p)
{
	while(1)
	{
		while( ( true == run_fault_cmd ) && ( fault_group[EN_FAULT_FLAG] ) )
		{
			Delay_m(fault_interval_time ); 

			/* Double check required because when this task is in sleep mode and
			 * user issue "stop" command than no need to send fault response.
			 */
			if ( true == run_fault_cmd )
			{
			    Create_Faults_buffer(system_configuration.IC_Count);
			}
		}
	}
}

bool Application_Initialize( void )
{
	BaseType_t task_ret;
    bool RetVal = true;
	system_configuration.IC_Count = 0;

	task_ret = xTaskCreate( Command_Execution_Thread, "vTask_command_execution_Thread", 5000, NULL, 1, NULL );

	if( task_ret != pdPASS )
	{
        //DebugStrLf(DEBUG_INTERFACE, "vTask_command_execution_Thread creation failed...");
		RetVal = false;
	}

	if ( true == RetVal )
	{
	    task_ret = xTaskCreate( Continuous_Command_Thread, "vTask_continuous_command_Thread", 5000, NULL, 1, NULL );

	    if( task_ret != pdPASS )
	    {
		    //DebugStrLf(DEBUG_INTERFACE, "vTask_continuous_command_Thread creation failed...");
	    	RetVal = false;
	    }
	}

	if ( true == RetVal )
	{
	    task_ret = xTaskCreate( Faults_Thread, "Faults_Thread", 6000, NULL, 1, NULL );

	    if( task_ret != pdPASS )
	    {
		    //DebugStrLf(DEBUG_INTERFACE, "Faults_Thread creation failed...");
	    	RetVal = false;
	    }
	}

	if ( true == RetVal )
	{
	    continuous_cmd_event_handle = xEventGroupCreate();

	    if(continuous_cmd_event_handle == NULL)
	    {
		    //DebugStrLf(DEBUG_INTERFACE, "Event group creation failed...");
	    	RetVal = false;
	    }
	}

	return RetVal;
}

void ADBMS1818_Set_CFG_UV(uint8_t nIC, CELL_ASIC *ic,uint16_t uv)
{
	uint16_t tmp = (uv/16)-1;
	ic[nIC].config.tx_data[1] = 0x00FF & tmp;
	ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]&0xF0;
	ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]|((0x0F00 & tmp)>>8);
}

void ADBMS1818_Set_CFG_OV(uint8_t nIC, CELL_ASIC *ic,uint16_t ov)
{
	uint16_t tmp = (ov/16);
	ic[nIC].config.tx_data[3] = 0x00FF & (tmp>>4);
	ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]&0x0F;
	ic[nIC].config.tx_data[2] = ic[nIC].config.tx_data[2]|((0x000F & tmp)<<4);
}

/* Helper function to control discharge time value */
void ADBMS1818_Set_CFG_Dcto(uint8_t nIC, CELL_ASIC *ic, bool dcto[4])
{
	uint8_t loop;

	for( loop = 0U; loop < 4; loop++ )
	{
		if ( dcto[loop] )
		{
			ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5] | ( 0x01 << ( loop + 4 ) );
		}
		else
		{
			ic[nIC].config.tx_data[5] = ic[nIC].config.tx_data[5] & ( ~( 0x01 << ( loop + 4 ) ) );
		}
	}
}

/* Helper function to set the DTMEN bit */
void ADBMS1818_Set_CFGB_Dtmen(uint8_t nIC, CELL_ASIC *ic, bool dtmen)
{
	if( dtmen )
	{
		ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] | 0x08;
	}
	else
	{
		ic[nIC].configb.tx_data[1] = ic[nIC].configb.tx_data[1] & 0xF7;
	}
}
