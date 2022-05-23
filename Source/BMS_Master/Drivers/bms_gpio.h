/*!
********************************************************************************
* @file         :   bms_gpio.h
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   Prototypes for GPIO Configuration logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_GPIO_H
#define BMS_GPIO_H

/* **** Includes **** */
#include "stddef.h"
#include "gpio.h"
#include "mxc_sys.h"

#define STATUS_LED_PORT			        PORT_2
#define STATUS_LED_PIN			        PIN_0

#define SPI0_SCK_PORT			        PORT_0
#define SPI0_SCK_PIN			        PIN_4
#define SPI0_MOSI_PORT			        PORT_0
#define SPI0_MOSI_PIN			        PIN_5
#define SPI0_MISO_PORT			        PORT_0
#define SPI0_MISO_PIN			        PIN_6
#define DURA_SPI0_FW_SS0_PORT           PORT_0
#define DURA_SPI0_FW_SS0_PIN	        PIN_7
#define DURA_SPI0_RW_SS1_PORT	        PORT_4
#define DURA_SPI0_RW_SS1_PIN	        PIN_4
#define FRC_SPI0_FW_SS2_PORT	        PORT_4
#define FRC_SPI0_FW_SS2_PIN		        PIN_5
#define FRC_SPI0_RW_SS3_PORT	        PORT_4
#define FRC_SPI0_RW_SS3_PIN		        PIN_6
#define FRC_SPI_EN_PORT		            PORT_4
#define FRC_SPI_EN_PIN                  PIN_7


#define	CAN_SPI1_CLK_PORT               PORT_1
#define CAN_SPI1_CLK_PIN		        PIN_0
#define CAN_SPI1_MOSI_PORT		        PORT_1
#define CAN_SPI1_MOSI_PIN		        PIN_1
#define CAN_SPI1_MISO_PORT		        PORT_1
#define CAN_SPI1_MISO_PIN		        PIN_2
#define	CAN_SPI1_SS0_PORT		        PORT_1
#define	CAN_SPI1_SS0_PIN		        PIN_3
#define CAN_SPI1_INT_PORT		        PORT_1
#define CAN_SPI1_INT_PIN		        PIN_4
#define CAN_TRANSCEIVER_SILENT_PORT     PORT_1
#define CAN_TRANSCEIVER_SILENT_PIN      PIN_5
#define CONTACTOR_EN_PORT		        PORT_4
#define CONTACTOR_EN_PIN		        PIN_0

#define JUMPER1_P4_2_PORT		        PORT_4
#define JUMPER1_P4_2_PIN		        PIN_2
#define JUMPER2_P4_3_PORT		        PORT_4
#define JUMPER2_P4_3_PIN		        PIN_3


#define PMUX_STATE1_PORT		        PORT_2
#define PMUX_STATE1_PIN		            PIN_1
#define PMUX_STATE2_PORT		        PORT_2
#define PMUX_STATE2_PIN		            PIN_2

#define ARD_PWR_SW_FAULT_PORT           PORT_3
#define ARD_PWR_SW_FAULT_PIN	        PIN_6
#define ARD_PWR_SW_EN_PORT		        PORT_4
#define ARD_PWR_SW_EN_PIN		        PIN_1

#define P2_3_PORT				        PORT_2
#define P2_3_PIN				        PIN_3
#define P3_7_PORT				        PORT_3
#define P3_7_PIN				        PIN_7

typedef enum
{
	STATUS_LED = 0U,

	SPI0_SCK,
	SPI0_MOSI,
	SPI0_MISO,
	DURA_SPI0_FW_SS0,
	DURA_SPI0_RW_SS1,
	FRC_SPI0_FW_SS2,
	FRC_SPI0_RW_SS3,
	FRC_SPI_EN,

	CAN_SPI1_CLK,
	CAN_SPI1_MOSI,
	CAN_SPI1_MISO,
	CAN_SPI1_SS0,
	CAN_SPI1_INT,
	CAN_TRANSCEIVER_SILENT,
	CONTACTOR_EN,

	JUMPER1_P4_2,
	JUMPER2_P4_3,

	PMUX_STATE1,
	PMUX_STATE2,

	ARD_PWR_SW_FAULT,
	ARD_PWR_SW_EN,

	P2_3,
	P3_7

} gpio_caseNum;

typedef enum
{
	GPIO_INPUT,
	GPIO_OUTPUT,
	GPIO_INPUT_OUTPUT,
} gpio_direction;

typedef enum
{
	GPIO_LOW_LEVEL,
	GPIO_HIGH_LEVEL,
	GPIO_LOW_HIGH_LEVEL,
} gpio_polarity;

typedef enum
{
	GPIO_VDDIO,
	GPIO_VDDIOH,
} gpio_level;

typedef struct
{
	gpio_caseNum caseNum;
	char name[8];
	uint32_t port;
	uint32_t pinMask;
	gpio_func_t func;
	gpio_pad_t pad;
	uint8_t direction;
	uint8_t activeState;
	uint8_t level;
} gpio_pin_port_map;

void GPIO_Configure( void );
uint32_t Get_Pin_Status(uint32_t pin, uint32_t state, int doingShift);
void Board_Status_LED_On( void );
void Board_Status_LED_Off( void );
void Contactor_Enable( void );
void Contactor_Disable( void );

#endif /* BMS_GPIO_H */
