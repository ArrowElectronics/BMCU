/*!
********************************************************************************
* @file         :   bms_usb.h
* @author       :   Ashvin Ramani, Srikanth Reddy Ramidi
* @brief        :   Prototypes for USB module logic
*******************************************************************************/

/* Define to prevent redundant inclusion */
#ifndef BMS_USB_H
#define BMS_USB_H

#define MAIN_USB             1         /* USB instance */

int USB_Configure(void);
int USB_SendStr( uint8_t * buffer, int len);
int USB_RecvAvl(void);
int USB_RecvStr( uint8_t * buffer, unsigned int len);

#endif /* BMS_USB_H */
