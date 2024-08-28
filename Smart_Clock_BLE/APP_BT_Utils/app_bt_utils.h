/******************************************************************************
* File Name:   util_functions.h
*
* Description: This file consists of the utility functions that will help
*              debugging and developing the applications easier with much
*              more meaningful information.
*
* Related Document: See Readme.md
*
*******************************************************************************/

#ifndef __APP_BT_UTILS_H__
#define __APP_BT_UTILS_H__

/******************************************************************************
 *                                INCLUDES
 ******************************************************************************/
#include "wiced_bt_dev.h"
#include "wiced_bt_gatt.h"
#include <stdio.h>

/******************************************************************************
 *                                Constants
 ******************************************************************************/
#define CASE_RETURN_STR(enum_val)          case enum_val: return #enum_val;

#define FROM_BIT16_TO_8(val)            ((uint8_t)((val) >> 8 ))

/****************************************************************************
 *                              FUNCTION DECLARATIONS
 ***************************************************************************/
void print_bd_address(wiced_bt_device_address_t bdadr);
const char *get_bt_event_name(wiced_bt_management_evt_t event);
const char *get_bt_advert_mode_name(wiced_bt_ble_advert_mode_t mode);
const char *get_bt_gatt_disconn_reason_name(wiced_bt_gatt_disconn_reason_t reason);
const char *get_bt_gatt_status_name(wiced_bt_gatt_status_t status);

#endif      /*__APP_BT_UTILS_H__ */


/* [] END OF FILE */
