/*!
********************************************************************************
* @file         :   bms_main.c
* @author       :   Srikanth Reddy Ramidi, Ashvin Ramani
* @brief        :   Main program body
*******************************************************************************/

#include "bms_main.h"

static bool InitializeAllModules ( void );

int main ( void )
{
    bool Result = false;

    Result = InitializeAllModules();

    if ( true == Result )
    {
        Board_Status_LED_On();

        /* If everything was OK so far, start scheduler here. */
        vTaskStartScheduler();
    }
    else
    {
        /* The scheduler has not started because of having some failure in
         * system initialization. So, stopping the execution */

    	return E_SHUTDOWN;
    }

	while(1)
	{
		/* This loop is to ensure that all the threads have a parent task and
		 * also to never get out of main function */
	}
}

static bool InitializeAllModules ( void )
{
	int32_t Result;
	bool RetValue = false;

	GPIO_Configure();                                    /* Configure GPIO's and it's INT. */
    
    if( DEBUG_INTERFACE == MAIN_UART )
	{
		RetValue = UART_Configure(MAIN_UART_BAUDRATE);   /* Initialize UART Module. */
	}
	else if( DEBUG_INTERFACE == MAIN_USB )
	{
		Result = USB_Configure();              /* Initialize USB Module. */

		if( Result == E_NO_ERROR )
		{
			RetValue = true;
		}
	}

	if ( true == RetValue )
	{
		RetValue = SPI_Dura_FRC_Configure();   /* Initialize SPI Module on DURA-FRC Connector. */
	}

	if ( true == RetValue )
	{
		RetValue = CAN_Configure();            /* Initialize CAN Module. */
	}

	if ( true == RetValue )
	{
		RetValue = SPI_Arduino_Configure();    /* Initialize SPI Module on Arduino Connector. */
	}

	if ( true == RetValue )
	{
		RetValue = RTC_Configure();            /* Enable 32.768KHz external connected oscillator for RTC with RTC Module */
	}

	if ( true == RetValue )
	{
		RetValue = Jumper_Initialise();        /* Initialize Jumper Settings. */
	}

	if ( true == RetValue )
	{
		RetValue = ADC_Configure();            /* Initialize ADC Module. */
	}

    if ( true == RetValue )
    {
    	RetValue = GUIComms_Initialize();      /* Initialize the low-level BMS Protocol driver. */
    }

    if ( true == RetValue )
    {
    	RetValue = Application_Initialize();   /* Initialize the application. */
    }

	return RetValue;
}
