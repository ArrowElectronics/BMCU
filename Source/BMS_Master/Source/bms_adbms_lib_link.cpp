/*!
********************************************************************************
* @file         :   bms_adbms_lib_link.cpp
* @author       :   Srikanth Reddy Ramidi
* @brief        :   Link between ADBMS lib and application
*******************************************************************************/

#include "bms_adbms_lib_link.h"

#include <stdlib.h>
#include "ADBMS1818.h"
#include "ADBMS181x.h"

Serial_wrapper Serial;

int8_t Send_Command_Read_Data(const uint8_t *cmd, uint8_t cmd_size, uint8_t *data, uint8_t data_len, uint8_t totalIC)
{
	int8_t error;

	wakeup_sleep(totalIC);

	cs_low(CS_PIN);
	error = spi_write_read(cmd, cmd_size, data, data_len);
	cs_high(CS_PIN);

	if(error != E_NO_ERROR)
	{
		DebugStrLf(DEBUG_INTERFACE, "Error : In Send_Command_Read_Data function");
	}

	return error;
}

int8_t Send_Command(const uint8_t *cmd, uint8_t cmd_size, uint8_t totalIC)
{
	int8_t error;

	wakeup_sleep(totalIC);

	cs_low(CS_PIN);
	error = spi_write_array(cmd_size, cmd);
	cs_high(CS_PIN);

	if(error != E_NO_ERROR)
	{
		DebugStrLf(DEBUG_INTERFACE, "Error : In Send_Command function");
	}

	return error;
}

uint16_t Calculate_CRC(uint8_t len, uint8_t *data )
{
	return pec15_calc(len, data);
}

uint32_t ADBMS1818_PollADC( )
{
	return ADBMS181x_pollAdc();
}

void Wakeup_Idle(uint8_t total_ic)
{
	wakeup_idle(total_ic);
}

void Wakeup_Sleep(uint8_t total_ic)
{
	wakeup_sleep(total_ic);
}

/*
 * Duplicated the ADBMS181x_run_openwire_multi function from ADBMS181x.c file
 * Slightly modified to get the open wires
 */
/* Runs the data sheet algorithm for open wire for multiple cell and two consecutive cells detection */
void Get_ADBMS1818_Cell_OpenWires(uint8_t total_ic, int8_t opencells[][18], int8_t *num_of_open_wires )
{
	cell_asic ic[total_ic];
	uint16_t OPENWIRE_THRESHOLD = 4000;
	const uint8_t  N_CHANNELS = 18;

	uint16_t pullUp[total_ic][N_CHANNELS];
	uint16_t pullDwn[total_ic][N_CHANNELS];
	uint16_t openWire_delta[total_ic][N_CHANNELS];

	int8_t error;
	int8_t n=0;
	int8_t i,j,k;
	uint32_t conv_time=0;

	ADBMS1818_init_reg_limits(total_ic, ic);
	for(uint8_t ic_index=0;ic_index<total_ic;ic_index++)
	{
		ic[ic_index].isospi_reverse = false;
	}


	wakeup_sleep(total_ic);
	ADBMS181x_clrcell();

	// Pull Ups
	for (i = 0; i < 5; i++)
	{
		//wakeup_idle(total_ic);
		ADBMS181x_adow(MD_26HZ_2KHZ,PULL_UP_CURRENT,CELL_CH_ALL,DCP_DISABLED);
		conv_time =ADBMS181x_pollAdc();
	}

	wakeup_idle(total_ic);
	error = ADBMS181x_rdcv(0, total_ic,ic);

	for (int cic=0; cic<total_ic; cic++)
	{
	    for (int cell=0; cell<N_CHANNELS; cell++)
		{
		  pullUp[cic][cell] = ic[cic].cells.c_codes[cell];
		}
	}

	// Pull Downs
	for (i = 0; i < 5; i++)
	{
	  //wakeup_idle(total_ic);
	  ADBMS181x_adow(MD_26HZ_2KHZ,PULL_DOWN_CURRENT,CELL_CH_ALL,DCP_DISABLED);
	  conv_time =   ADBMS181x_pollAdc();
	}

	wakeup_idle(total_ic);
	error = ADBMS181x_rdcv(0, total_ic,ic);

	for (int cic=0; cic<total_ic; cic++)
	{
		for (int cell=0; cell<N_CHANNELS; cell++)
		{
		   pullDwn[cic][cell] = ic[cic].cells.c_codes[cell];
		}
	}

	for (int cic=0; cic<total_ic; cic++)
	{
		for (int cell=0; cell<N_CHANNELS; cell++)
		{
			if (pullDwn[cic][cell] < pullUp[cic][cell])
				{
					openWire_delta[cic][cell] = (pullUp[cic][cell] - pullDwn[cic][cell]);
				}
				else
				{
					openWire_delta[cic][cell] = 0;
				}


		}
	}

	for (int cic=0; cic<total_ic; cic++)
	{
		n=0;

		for (int cell=0; cell<N_CHANNELS; cell++)
		{

		  if (openWire_delta[cic][cell]>OPENWIRE_THRESHOLD)
			{
				opencells[cic][n] = cell+1;
				n++;
				for (int j = cell; j < N_CHANNELS-3 ; j++)
				{
					if (pullUp[cic][j + 2] == 0)
					{
					opencells[cic][n] = j+2;
					n++;
					}
				}
				if((cell==N_CHANNELS-4) && (pullDwn[cic][N_CHANNELS-3] == 0))
				{
					  opencells[cic][n] = N_CHANNELS-2;
					  n++;
				}
			}
		}
		if (pullDwn[cic][0] == 0)
		{
		  opencells[cic][n] = 0;
		  n++;
		}

		if (pullDwn[cic][N_CHANNELS-1] == 0)
		{
		  opencells[cic][n] = N_CHANNELS;
		  n++;
		}

		if (pullDwn[cic][N_CHANNELS-2] == 0)
		{
		  opencells[cic][n] = N_CHANNELS-1;
		  n++;
		}

	//Removing repetitive elements
		for(i=0;i<n;i++)
		{
			for(j=i+1;j<n;)
			{
				if(opencells[cic][i]==opencells[cic][j])
				{
					for(k=j;k<n;k++)
						opencells[cic][k]=opencells[cic][k+1];

					n--;
				}
				else
					j++;
			}
		}

	// Sorting open cell array
		for(int i=0; i<n; i++)
		{
			for(int j=0; j<n-1; j++)
			{
				if( opencells[cic][j] > opencells[cic][j+1] )
				{
					k = opencells[cic][j];
					opencells[cic][j] = opencells[cic][j+1];
					opencells[cic][j+1] = k;
				}
			}
		}

		num_of_open_wires[cic] = n;
	}

	(void)conv_time;
	(void)error;
}
