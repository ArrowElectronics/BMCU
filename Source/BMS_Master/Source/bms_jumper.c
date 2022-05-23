/*!
********************************************************************************
* @file         :   bms_jumper.c
* @author       :   Ashvin Ramani
* @brief        :   Jumper detection logic
*******************************************************************************/

#include "bms_jumper.h"

static gpio_cfg_t gpio_jmp1, gpio_jmp2;
static uint32_t jum1_state = true, jum2_state = true;

static void Jumper_Configuration( void );
static void Jumper_Detection_Thread( void* parameter );

bool Jumper_Initialise( void )
{
	BaseType_t task_ret;
    bool RetValue = true;

	task_ret = xTaskCreate( Jumper_Detection_Thread, "Jumper_Detection_Thread", 1000, NULL, 1, NULL );

	if( pdPASS == task_ret )
	{
	  	  //DebugStrLf(DEBUG_INTERFACE, "Jumper_Detection_Thread created successfully...");
	}
	else
	{
	  	 //DebugStrLf(DEBUG_INTERFACE, "Jumper_Detection_Thread creation failed...");
		RetValue = false;
	}

	return RetValue;
}

static void Jumper_Configuration( void )
{
	uint32_t jump1, jump2, state1, state2;

	gpio_jmp1.port = JUMPER1_P4_2_PORT;
	gpio_jmp1.mask = JUMPER1_P4_2_PIN;
	gpio_jmp1.func = GPIO_FUNC_GPIO;
	gpio_jmp1.pad = GPIO_PAD_INPUT;

	GPIO_Config(&gpio_jmp1);

	gpio_jmp2.port = JUMPER2_P4_3_PORT;
	gpio_jmp2.mask = JUMPER2_P4_3_PIN;
	gpio_jmp2.func = GPIO_FUNC_GPIO;
	gpio_jmp2.pad = GPIO_PAD_INPUT;

	GPIO_Config(&gpio_jmp2);

	jump1 = GPIO_InGet(&gpio_jmp1);
	state1 = Get_Pin_Status(JUMPER1_P4_2_PIN, jump1, true);
	Update_Dura_FRC_SPI_Interface(state1);
	jum1_state = state1;

	jump2 = GPIO_InGet(&gpio_jmp2);
	state2 = Get_Pin_Status(JUMPER2_P4_3_PIN, jump2, true);
	//call API(state2);
	jum2_state = state2;
}

static void Jumper_Detection_Thread( void* parameter )
{
	uint32_t jump1, jump2, state1, state2;

	Jumper_Configuration();

	while(1)
	{
		jump1 = GPIO_InGet(&gpio_jmp1);
		state1 = Get_Pin_Status(JUMPER1_P4_2_PIN, jump1, true);
		jump2 = GPIO_InGet(&gpio_jmp2);
		state2 = Get_Pin_Status(JUMPER2_P4_3_PIN, jump2, true);

		if ( ( jum1_state != state1 ) || ( jum2_state != state2 ) )
		{
			vTaskDelay(pdMS_TO_TICKS(500));

			jump1 = GPIO_InGet(&gpio_jmp1);
			state1 = Get_Pin_Status(JUMPER1_P4_2_PIN, jump1, true);
			jump2 = GPIO_InGet(&gpio_jmp2);
			state2 = Get_Pin_Status(JUMPER2_P4_3_PIN, jump2, true);

			if ( jum1_state != state1 )
			{
				Update_Dura_FRC_SPI_Interface(state1);

				jum1_state = state1;
			}

			if ( jum2_state != state2 )
			{
				//call APi(state2)

				jum2_state = state2;
			}
		}

		vTaskDelay(pdMS_TO_TICKS(200));
	}
}
