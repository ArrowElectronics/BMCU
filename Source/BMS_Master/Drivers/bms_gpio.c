/*!
********************************************************************************
* @file         :   bms_gpio.c
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   GPIO Configuration logic
*******************************************************************************/

/***** Includes *****/
#include "bms_gpio.h"

/***** Definitions *****/


/*
 * brief: Gpio pins port and pinMask mapping
 */
static const gpio_pin_port_map gpioMap[] =
{
    { STATUS_LED,  "P2.0", STATUS_LED_PORT,       STATUS_LED_PIN,       GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_HIGH_LEVEL, GPIO_VDDIOH},

    { SPI0_SCK,    "P0.4", SPI0_SCK_PORT,         SPI0_SCK_PIN,         GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_HIGH_LEVEL, GPIO_VDDIOH},
    { SPI0_MOSI,   "P0.5", SPI0_MOSI_PORT,        SPI0_MOSI_PIN,        GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { SPI0_MISO,   "P0.6", SPI0_MISO_PORT,        SPI0_MISO_PIN,        GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_INPUT,  GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { DURA_SPI0_FW_SS0,  "P0.7", DURA_SPI0_FW_SS0_PORT, DURA_SPI0_FW_SS0_PIN, GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { DURA_SPI0_RW_SS1,  "P4.4", DURA_SPI0_RW_SS1_PORT, DURA_SPI0_RW_SS1_PIN, GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { FRC_SPI0_FW_SS2,   "P4.5", FRC_SPI0_FW_SS2_PORT,  FRC_SPI0_FW_SS2_PIN,  GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { FRC_SPI0_RW_SS3,   "P4.6", FRC_SPI0_RW_SS3_PORT,  FRC_SPI0_RW_SS3_PIN,  GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_INPUT_OUTPUT, GPIO_LOW_LEVEL, GPIO_VDDIOH},
    { FRC_SPI_EN,        "P4.7", FRC_SPI_EN_PORT,       FRC_SPI_EN_PIN,       GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_LOW_LEVEL,  GPIO_VDDIOH},

    { CAN_SPI1_CLK,      "P1.0", CAN_SPI1_CLK_PORT,     CAN_SPI1_CLK_PIN,     GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_HIGH_LEVEL, GPIO_VDDIOH},
    { CAN_SPI1_MOSI,     "P1.1", CAN_SPI1_MOSI_PORT,    CAN_SPI1_MOSI_PIN,    GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { CAN_SPI1_MISO,     "P1.2", CAN_SPI1_MISO_PORT,    CAN_SPI1_MISO_PIN,    GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_INPUT,  GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { CAN_SPI1_SS0,      "P1.3", CAN_SPI1_SS0_PORT,     CAN_SPI1_SS0_PIN,     GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { CAN_SPI1_INT,      "P1.4", CAN_SPI1_INT_PORT,     CAN_SPI1_INT_PIN,     GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_INPUT,  GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { CAN_TRANSCEIVER_SILENT, "P1.5", CAN_TRANSCEIVER_SILENT_PORT, CAN_TRANSCEIVER_SILENT_PIN, GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_HIGH_LEVEL, GPIO_VDDIOH},
    { CONTACTOR_EN, "P4.0", CONTACTOR_EN_PORT,     CONTACTOR_EN_PIN,     GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_HIGH_LEVEL, GPIO_VDDIOH},

    { JUMPER1_P4_2, "P4.2", JUMPER1_P4_2_PORT,     JUMPER1_P4_2_PIN,     GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_INPUT,  GPIO_LOW_HIGH_LEVEL, GPIO_VDDIOH},
    { JUMPER2_P4_3, "P4.3", JUMPER2_P4_3_PORT,     JUMPER2_P4_3_PIN,     GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_INPUT,  GPIO_LOW_HIGH_LEVEL, GPIO_VDDIOH},

    { PMUX_STATE1,  "P2.1", PMUX_STATE1_PORT,      PMUX_STATE1_PIN,      GPIO_FUNC_GPIO, GPIO_PAD_INPUT_PULLUP, GPIO_INPUT,  GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { PMUX_STATE2,  "P2.2", PMUX_STATE2_PORT,      PMUX_STATE2_PIN,      GPIO_FUNC_GPIO, GPIO_PAD_INPUT_PULLUP, GPIO_INPUT,  GPIO_LOW_LEVEL,  GPIO_VDDIOH},

    { ARD_PWR_SW_FAULT, "P3.6", ARD_PWR_SW_FAULT_PORT, ARD_PWR_SW_FAULT_PIN, GPIO_FUNC_GPIO, GPIO_PAD_INPUT_PULLUP, GPIO_INPUT,  GPIO_LOW_LEVEL,  GPIO_VDDIOH},
    { ARD_PWR_SW_EN, "P4.1", ARD_PWR_SW_EN_PORT,    ARD_PWR_SW_EN_PIN,    GPIO_FUNC_GPIO, GPIO_PAD_NORMAL, GPIO_OUTPUT, GPIO_HIGH_LEVEL, GPIO_VDDIOH},

    { P2_3, "P2.3", P2_3_PORT,             P2_3_PIN,             GPIO_FUNC_GPIO, GPIO_PAD_INPUT, GPIO_INPUT, GPIO_LOW_HIGH_LEVEL, GPIO_VDDIOH},
    { P3_7, "P3.7", P3_7_PORT,             P3_7_PIN,             GPIO_FUNC_GPIO, GPIO_PAD_INPUT, GPIO_INPUT, GPIO_LOW_HIGH_LEVEL, GPIO_VDDIOH},
};


/***** Globals *****/

/***** Functions *****/
static void GPIO_Set_Voltage_Level( void );
static void Power_Source_INT_Configure( void );
static void Arduino_Power_SW_On( void );
static void Arduino_Power_SW_Off( void );
static void ArduinoPwrSwitchFlt_Callback( void *Param );
static void DCJackPowerSource_Callback( void *Param );
static void USBPowerSource_Callback( void *Param );
static void Arduino_GPIO_Configure( void );
/*
 * brief : To init voltage level of GPIO's
 */
void GPIO_Configure( void )
{
	gpio_cfg_t gpio;
	gpio_caseNum entry;

	GPIO_Set_Voltage_Level();

	/* Configure Board Status LED */
	entry = STATUS_LED;
	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;

	GPIO_Config( &gpio );
	GPIO_OutClr( &gpio );

	/* Configure Contactor_Enable Pin */
	entry = CONTACTOR_EN;
	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;

	GPIO_Config( &gpio );
	GPIO_OutClr( &gpio );

	/* Configure Unused GPIO's with an Input direction */
	/* P2.3 */
	entry = P2_3;
	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;
	GPIO_Config( &gpio );

	/* P3.7 */
	entry = P3_7;
	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;
	GPIO_Config( &gpio );

	/* Configure Arduino GPIO's as an Input by default */
    Arduino_GPIO_Configure();

	/* Configure GPIO INT on MCU Power Pin */
	Power_Source_INT_Configure();
}

uint32_t Get_Pin_Status(uint32_t pin, uint32_t state, int doingShift)
{
	uint32_t gpioPin = pin;
	uint32_t gpioState = state;

	// shift down if desired
    if ( gpioPin > 0 && doingShift )
	{
		while ( ( gpioPin & 0x1 ) == 0)
		{
			gpioState = gpioState >> 1;
			gpioPin = gpioPin >> 1;
		}
	}

    return gpioState;
}

/*
 * brief : To init voltage level of GPIO's
 */
void Board_Status_LED_On( void )
{
	gpio_cfg_t gpio;
	gpio_caseNum entry = STATUS_LED;

	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;

	GPIO_OutSet( &gpio );
}

/*
 * brief : To init voltage level of GPIO's
 */
void Board_Status_LED_Off( void )
{
	gpio_cfg_t gpio;
	gpio_caseNum entry = STATUS_LED;

	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;

	GPIO_OutClr( &gpio );
}

/*
 * brief : To init voltage level of GPIO's
 */
void Contactor_Enable( void )
{
	gpio_cfg_t gpio;
	gpio_caseNum entry = CONTACTOR_EN;

	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;

	GPIO_OutSet( &gpio );
}

/*
 * brief : To init voltage level of GPIO's
 */
void Contactor_Disable( void )
{
	gpio_cfg_t gpio;
	gpio_caseNum entry = CONTACTOR_EN;

	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;

	GPIO_OutClr( &gpio );
}

/*
 * brief : To init voltage level of GPIO's
 */
static void Power_Source_INT_Configure( void )
{
	gpio_cfg_t dcJackPwr, usbPwr, ardPwrSWEN, ardPwrSWFlt;
	gpio_caseNum entry = ARD_PWR_SW_EN;

	/* Configure Arduino Power Switch EN */
	ardPwrSWEN.port = gpioMap[entry].port;
	ardPwrSWEN.mask = gpioMap[entry].pinMask;
	ardPwrSWEN.func = gpioMap[entry].func;
	ardPwrSWEN.pad =  gpioMap[entry].pad;

	GPIO_Config( &ardPwrSWEN );

	/* Configure Arduino Power Switch Flt */
	entry = ARD_PWR_SW_FAULT;
	ardPwrSWFlt.port = gpioMap[entry].port;
	ardPwrSWFlt.mask = gpioMap[entry].pinMask;
	ardPwrSWFlt.func = gpioMap[entry].func;
	ardPwrSWFlt.pad =  gpioMap[entry].pad;

	GPIO_Config( &ardPwrSWFlt );

	/* Configure INT on Arduino Power Switch Flt Pin*/
	GPIO_RegisterCallback(&ardPwrSWFlt, ArduinoPwrSwitchFlt_Callback, NULL);
    GPIO_IntConfig(&ardPwrSWFlt, GPIO_INT_FALLING_EDGE);
    GPIO_IntEnable(&ardPwrSWFlt);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(ardPwrSWFlt.port));

	/* Configure DCJack Power INT Pin */
	entry = PMUX_STATE1;
	dcJackPwr.port = gpioMap[entry].port;
	dcJackPwr.mask = gpioMap[entry].pinMask;
	dcJackPwr.func = gpioMap[entry].func;
	dcJackPwr.pad =  gpioMap[entry].pad;

	GPIO_Config( &dcJackPwr );

	/* Configure USB Power INT Pin */
	entry = PMUX_STATE2;
	usbPwr.port = gpioMap[entry].port;
	usbPwr.mask = gpioMap[entry].pinMask;
	usbPwr.func = gpioMap[entry].func;
	usbPwr.pad =  gpioMap[entry].pad;

	GPIO_Config( &usbPwr );

	/* Enable/Disable arduino power switch based on pin state of DCJack & USB power source */
	if ( PMUX_STATE1_PIN != GPIO_InGet(&dcJackPwr) )
	{
		GPIO_OutSet( &ardPwrSWEN );
	}
	else if ( PMUX_STATE2_PIN != GPIO_InGet(&usbPwr) )
	{
		GPIO_OutClr( &ardPwrSWEN );
	}
	else
	{
		/* do nothing... */
	}

	/* Configure INT on DCJack Power Pin*/
	GPIO_RegisterCallback(&dcJackPwr, DCJackPowerSource_Callback, NULL);
    GPIO_IntConfig(&dcJackPwr, GPIO_INT_FALLING_EDGE);
    GPIO_IntEnable(&dcJackPwr);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(dcJackPwr.port));

    /* Configure INT on USB Power Pin*/
	GPIO_RegisterCallback(&usbPwr, USBPowerSource_Callback, NULL);
    GPIO_IntConfig(&usbPwr, GPIO_INT_FALLING_EDGE);
    GPIO_IntEnable(&usbPwr);
    NVIC_EnableIRQ(MXC_GPIO_GET_IRQ(usbPwr.port));
}

/*
 * brief : To init voltage level of GPIO's
 */
static void Arduino_Power_SW_On( void )
{
	gpio_cfg_t gpio;
	gpio_caseNum entry = ARD_PWR_SW_EN;

	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;

	GPIO_OutSet( &gpio );
}

/*
 * brief : To init voltage level of GPIO's
 */
static void Arduino_Power_SW_Off( void )
{
	gpio_cfg_t gpio;
	gpio_caseNum entry = ARD_PWR_SW_EN;

	gpio.port = gpioMap[entry].port;
	gpio.mask = gpioMap[entry].pinMask;
	gpio.func = gpioMap[entry].func;
	gpio.pad = gpioMap[entry].pad;

	GPIO_OutClr( &gpio );
}

static void ArduinoPwrSwitchFlt_Callback( void *Param )
{
	/* TODO: Need to define functionality for this */
}

static void DCJackPowerSource_Callback( void *Param )
{
	Arduino_Power_SW_On();
}

static void USBPowerSource_Callback( void *Param )
{
	Arduino_Power_SW_Off();
}

static void Arduino_GPIO_Configure( void )
{
    gpio_cfg_t gpio;

    gpio.port = PORT_3;
    gpio.mask = PIN_0;
    gpio.func = GPIO_FUNC_GPIO;
    gpio.pad = GPIO_PAD_INPUT;
    GPIO_Config( &gpio );

    gpio.mask = PIN_1;
    GPIO_Config( &gpio );

    gpio.mask = PIN_2;
    GPIO_Config( &gpio );

    gpio.mask = PIN_3;
    GPIO_Config( &gpio );

    gpio.mask = PIN_4;
    GPIO_Config( &gpio );

    gpio.mask = PIN_5;
    GPIO_Config( &gpio );
}

static void GPIO_Set_Voltage_Level( void )
{
	gpio_cfg_t gpio;
	uint8_t gpio_selection = (uint8_t)STATUS_LED;
	uint8_t sizeOfMap = (uint8_t)(sizeof(gpioMap) / sizeof(gpioMap[0]));

	for ( ; gpio_selection < sizeOfMap; gpio_selection++ )
	{
		gpio.port = gpioMap[gpio_selection].port;
		gpio.mask = gpioMap[gpio_selection].pinMask;
		gpio.func = gpioMap[gpio_selection].func;
		gpio.pad = gpioMap[gpio_selection].pad;

		if ( GPIO_VDDIOH == gpioMap[gpio_selection].level )
		{
			SYS_IOMAN_UseVDDIOH(&gpio);
		}

		if ( GPIO_VDDIO == gpioMap[gpio_selection].level )
		{
			SYS_IOMAN_UseVDDIO(&gpio);
		}
	}
}
