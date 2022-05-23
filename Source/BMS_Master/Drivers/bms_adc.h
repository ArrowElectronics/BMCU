/*!
********************************************************************************
* @file         :   bms_adc.h
* @author       :   Ashvin Ramani
* @brief        :   Prototypes for ADC module logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_ADC_H
#define BMS_ADC_H

/* **** Includes **** */

#include <stdio.h>
#include <stdbool.h>

#include "mxc_config.h"
#include "adc.h"
#include "mxc_errors.h"
#include "nvic_table.h"

/* **** Definitions **** */

#define USE_INTERRUPTS        0U /* 0U:Polling method, 1U:INT method */

typedef struct {
		int index;
		mxc_adc_chsel_t adc_channel;
	    uint32_t     resistor_1;         /* high part of divider */
	    uint32_t     resistor_2;         /* low part of divider */
}adc_index_opt_map;

/* **** Function Prototypes **** */

/**
 * @brief  To Configure ADC Channel
 */
bool ADC_Configure(void);
uint16_t Get_ADC_Conversion(int adcNum);

#endif /* BMS_ADC_H */
