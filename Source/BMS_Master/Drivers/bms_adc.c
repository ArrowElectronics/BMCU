/*!
********************************************************************************
* @file         :   bms_adc.c
* @author       :   Ashvin Ramani
* @brief        :   ADC module logic
*******************************************************************************/

/***** Includes *****/
#include "bms_adc.h"

/***** Definitions *****/

static const adc_index_opt_map adc_map[]={
		{0, ADC_CH_0, 0, 1},
		{1, ADC_CH_1, 0, 1},
		{2, ADC_CH_2, 0, 1},
		{3, ADC_CH_3, 0, 1}
};

/***** Globals *****/
#if USE_INTERRUPTS
volatile unsigned int adc_done = 0;
#endif

/***** Functions *****/

/* *****************************************************************************/

#if USE_INTERRUPTS
void AFE_IRQHandler(void)
{
    ADC_ClearFlags(MXC_F_ADC_INTR_ADC_DONE_IF);

    /* Signal bottom half that data is ready */
    adc_done = 1;

    return;
}
#endif

/**
 * @brief  To Configure ADC Channel
 */
bool ADC_Configure( void )
{
	int result;
	bool RetValue = true;

    result = ADC_Init();

    if ( E_NO_ERROR != result )
    {
    	//DebugDataLf( MAIN_USB, "ADC_Configure error:", result, DATA_DEC );
    	RetValue = false;
    }
    else
    {
    	/* Enable ADC IRQ if want to use INT method */
#if USE_INTERRUPTS
        NVIC_EnableIRQ(AFE_IRQn);
#endif
    }

    return RetValue;
}

/*
 * brief : Converts and then prints the data of the selected ADC pin
 */
uint16_t Get_ADC_Conversion(int adcNum)
{
	uint16_t adc_val;
	//uint32_t overflow;

#if USE_INTERRUPTS
	adc_done = 0;
	ADC_StartConvert(adc_map[adcNum].adc_channel, 1, 1);
	while (!adc_done);
#else
	ADC_StartConvert(adc_map[adcNum].adc_channel, 0, 1);
#endif
	//overflow = (ADC_GetData(&adc_val) == E_OVERFLOW ? 1 : 0);

	(void)ADC_GetData(&adc_val);

	//DebugStr( MAIN_USB, "\n" );
    //DebugData( MAIN_USB, "ADC Channel", adc_map[adcNum].adc_channel, DEC );
    //DebugData( MAIN_USB, " Count is:", adc_val, HEX16 );
	//DebugData( MAIN_USB, ", Overflow Status is:", overflow, DEC );

	return adc_val;
}
