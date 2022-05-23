/*!
********************************************************************************
* @file         :   bms_usb.c
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   USB module logic
*******************************************************************************/

#include <stdio.h>
#include <stddef.h>

#include "bms_usb_consts.h"
#include "bms_usb.h"
#include "mxc_config.h"
#include "mxc_sys.h"
#include "pwrman_regs.h"
#include "lp.h"
#include "uart.h"
#include "usb.h"
#include "usb_event.h"
#include "enumerate.h"
#include "tmr_utils.h"
#include "cdc_acm.h"

/***** Definitions *****/
#define EVENT_ENUM_COMP     MAXUSB_NUM_EVENTS
#define EVENT_REMOTE_WAKE   (EVENT_ENUM_COMP + 1)

#define WORKAROUND_TIMER    0           // Timer used for USB wakeup workaround
#define TMRn_IRQHandler     TMR0_IRQHandler
#define MXC_TMRn            MXC_TMR_GET_TMR(WORKAROUND_TIMER)
#define TMRn_IRQn           MXC_TMR_GET_IRQ_32(WORKAROUND_TIMER)

/***** Global Data *****/
volatile int configured;
volatile int suspended;
volatile unsigned int event_flags;
int remote_wake_en;

/***** Function Prototypes *****/
static int setconfig_callback(usb_setup_pkt *sud, void *cbdata);
static int setfeature_callback(usb_setup_pkt *sud, void *cbdata);
static int clrfeature_callback(usb_setup_pkt *sud, void *cbdata);
static int event_callback(maxusb_event_t evt, void *data);
static void usb_app_sleep(void);
static void usb_app_wakeup(void);
static int usb_read_callback(void);
static void echo_usb(void);

/***** File Scope Variables *****/

/* This EP assignment must match the Configuration Descriptor */
static const acm_cfg_t acm_cfg = {
  1,                  /* EP OUT */
  MXC_USB_MAX_PACKET, /* OUT max packet size */
  2,                  /* EP IN */
  MXC_USB_MAX_PACKET, /* IN max packet size */
  3,                  /* EP Notify */
  MXC_USB_MAX_PACKET, /* Notify max packet size */
};

static uint8_t uart_tx_data[MXC_UART_FIFO_DEPTH];
static volatile int usb_read_complete;

/******************************************************************************/
int USB_Configure(void)
{
    /* Initialize state */
    configured = 0;
    suspended = 0;
    event_flags = 0;
    remote_wake_en = 0;

    /* Enable the USB clock and power */
    SYS_USB_Enable(1);

    /* Initialize the usb module */
    if (usb_init(NULL) != 0) {
        printf("usb_init() failed\n");
        return E_ABORT;
    }

    /* Initialize the enumeration module */
    if (enum_init() != 0) {
        printf("enum_init() failed\n");
        return E_ABORT;
    }

    /* Register enumeration data */
    enum_register_descriptor(ENUM_DESC_DEVICE, (uint8_t*)&device_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_CONFIG, (uint8_t*)&config_descriptor, 0);
    enum_register_descriptor(ENUM_DESC_STRING, lang_id_desc, 0);
    enum_register_descriptor(ENUM_DESC_STRING, mfg_id_desc, 1);
    enum_register_descriptor(ENUM_DESC_STRING, prod_id_desc, 2);

    /* Handle configuration */
    enum_register_callback(ENUM_SETCONFIG, setconfig_callback, NULL);

    /* Handle feature set/clear */
    enum_register_callback(ENUM_SETFEATURE, setfeature_callback, NULL);
    enum_register_callback(ENUM_CLRFEATURE, clrfeature_callback, NULL);

    /* Initialize the class driver */
    if (acm_init() != 0) {
        printf("acm_init() failed\n");
        return E_ABORT;
    }

    /* Register callbacks */
    usb_event_enable(MAXUSB_EVENT_NOVBUS, event_callback, NULL);
    usb_event_enable(MAXUSB_EVENT_VBUS, event_callback, NULL);
    acm_register_callback(ACM_CB_SET_LINE_CODING, NULL);
    acm_register_callback(ACM_CB_READ_READY, usb_read_callback);
    usb_read_complete = 0;

    /* Start with USB in low power mode */
    usb_app_sleep();
    NVIC_EnableIRQ(USB_IRQn);

    /* Wait for events */
    while (1) {

        echo_usb();

        if (suspended || !configured) {
            //LED_Off(0); /* TODO: LED not required in actual board */
        } else {
            //LED_On(0);
        }

        if (event_flags) {
            /* Display events */
            if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_NOVBUS)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_NOVBUS);
                printf("VBUS Disconnect\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_VBUS)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_VBUS);
                printf("VBUS Connect\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_BRST)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_BRST);
                printf("Bus Reset\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_SUSP)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_SUSP);
                printf("Suspended\n");
            } else if (MXC_GETBIT(&event_flags, MAXUSB_EVENT_DPACT)) {
                MXC_CLRBIT(&event_flags, MAXUSB_EVENT_DPACT);
                printf("Resume\n");
            } else if (MXC_GETBIT(&event_flags, EVENT_ENUM_COMP)) {
                MXC_CLRBIT(&event_flags, EVENT_ENUM_COMP);
                printf("Enumeration complete. Waiting for characters...\n");

              //  TMR_Delay(MXC_TMR0, 10000*1000);

                return E_NO_ERROR;

            } else if (MXC_GETBIT(&event_flags, EVENT_REMOTE_WAKE)) {
                MXC_CLRBIT(&event_flags, EVENT_REMOTE_WAKE);
                printf("Remote Wakeup\n");
            }
        } else {
            LP_EnterLP2();
        }
    }
}

int USB_SendStr( uint8_t * buffer, int len)
{
	return ( acm_write(buffer, len) );
}

int USB_RecvAvl( void )
{
	return acm_canread();
}

int USB_RecvStr( uint8_t * buffer, unsigned int len)
{
	int chars;

	while ( ( chars = acm_canread() ) <= 0 )
	{
		TMR_Delay(MXC_TMR0, 1000*1000);
	}

	if ( chars > len )
	{
		chars = len;
	}

	return ( acm_read(buffer, chars) );
}

/******************************************************************************/
static void echo_usb(void)
{
  int chars;

  if ((chars = acm_canread()) > 0) {

    if (chars > sizeof(uart_tx_data)) {
      chars = sizeof(uart_tx_data);
    }
    printf("acm_canread() :%d\n", chars);

    // Read the data from USB
    if (acm_read(uart_tx_data, chars) != chars) {
      printf("acm_read() failed\n");
      return;
    }

    // Echo it back
    if (acm_present()) {
      chars++;  /* account for the initial character */
      if (acm_write(uart_tx_data, chars) != chars) {
        printf("acm_write() failed\n");
      }
    }
  }
}

/******************************************************************************/
static int setconfig_callback(usb_setup_pkt *sud, void *cbdata)
{
    /* Confirm the configuration value */
    if (sud->wValue == config_descriptor.config_descriptor.bConfigurationValue) {
        configured = 1;
        MXC_SETBIT(&event_flags, EVENT_ENUM_COMP);
        return acm_configure(&acm_cfg); /* Configure the device class */
    } else if (sud->wValue == 0) {
        configured = 0;
        return acm_deconfigure();
    }

    return -1;
}

/******************************************************************************/
static int setfeature_callback(usb_setup_pkt *sud, void *cbdata)
{
    if(sud->wValue == FEAT_REMOTE_WAKE) {
        remote_wake_en = 1;
    } else {
        // Unknown callback
        return -1;
    }

    return 0;
}

/******************************************************************************/
static int clrfeature_callback(usb_setup_pkt *sud, void *cbdata)
{
    if(sud->wValue == FEAT_REMOTE_WAKE) {
        remote_wake_en = 0;
    } else {
        // Unknown callback
        return -1;
    }

    return 0;
}

/******************************************************************************/
static void usb_app_sleep(void)
{
    usb_sleep();
    MXC_PWRMAN->pwr_rst_ctrl &= ~MXC_F_PWRMAN_PWR_RST_CTRL_USB_POWERED;
    if (MXC_USB->dev_cn & MXC_F_USB_DEV_CN_CONNECT) {
        usb_event_clear(MAXUSB_EVENT_DPACT);
        usb_event_enable(MAXUSB_EVENT_DPACT, event_callback, NULL);
    } else {
        usb_event_disable(MAXUSB_EVENT_DPACT);
    }
    suspended = 1;
}

/******************************************************************************/
static void usb_app_wakeup(void)
{
    usb_event_disable(MAXUSB_EVENT_DPACT);
    MXC_PWRMAN->pwr_rst_ctrl |= MXC_F_PWRMAN_PWR_RST_CTRL_USB_POWERED;
    usb_wakeup();
    suspended = 0;
}

/******************************************************************************/
static int event_callback(maxusb_event_t evt, void *data)
{
    /* Set event flag */
    MXC_SETBIT(&event_flags, evt);

    switch (evt) {
        case MAXUSB_EVENT_NOVBUS:
            usb_event_disable(MAXUSB_EVENT_BRST);
            usb_event_disable(MAXUSB_EVENT_SUSP);
            usb_event_disable(MAXUSB_EVENT_DPACT);
            usb_disconnect();
            configured = 0;
            enum_clearconfig();
            acm_deconfigure();
            usb_app_sleep();
            break;
        case MAXUSB_EVENT_VBUS:
            usb_event_clear(MAXUSB_EVENT_BRST);
            usb_event_enable(MAXUSB_EVENT_BRST, event_callback, NULL);
            usb_event_clear(MAXUSB_EVENT_SUSP);
            usb_event_enable(MAXUSB_EVENT_SUSP, event_callback, NULL);
            usb_connect();
            usb_app_sleep();
            break;
        case MAXUSB_EVENT_BRST:
            usb_app_wakeup();
            enum_clearconfig();
            acm_deconfigure();
            configured = 0;
            suspended = 0;
            break;
        case MAXUSB_EVENT_SUSP:
            usb_app_sleep();
            break;
        case MAXUSB_EVENT_DPACT:
            usb_app_wakeup();
            break;
        default:
            break;
    }

    return 0;
}

/******************************************************************************/
static int usb_read_callback(void)
{
    usb_read_complete = 1;
    return 0;
}

/******************************************************************************/
void USB_IRQHandler(void)
{
    usb_event_handler();
}
