#ifndef MAIN_BLE_H_
#define MAIN_BLE_H_

#include "cybsp.h"
#include "cyhal.h"
#include "cy_retarget_io.h"

/* Library for malloc and free */
#include "stdlib.h"

/* FreeRTOS */
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>

/* btstack */
#include "wiced_bt_stack.h"

/* App utilities */
#include "app_bt_utils.h"

/* Include header files from BT configurator */
#include "cycfg_bt_settings.h"
#include "cycfg_gap.h"
#include "cycfg_gatt_db.h"
#include "rtc_sc.h"
#include <string.h>

#define APP_TIMEOUT_LED_BLINK          (200)

#define getSize(my_array)    (sizeof(my_array) / sizeof(my_array[0]))

typedef struct cmd_key_t
{
  char *cmd;
  void (*cb) (char *str_);
} cmd_key;

/* Typdef for function used to free allocated buffer to stack */
typedef void (*pfn_free_buffer_t)(uint8_t *);

/*******************************************************************
 * Function Prototypes
 ******************************************************************/

/* Callback function for Bluetooth stack management type events */
wiced_bt_dev_status_t app_bt_management_callback(wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data);

/*Global Variable*/
extern bool daily_notify_BLE;
extern bool monthly_notify_BLE;

extern bool environment_notify;
extern bool air_quality_notify;

extern cyhal_pwm_t PWM_obj;
extern TimerHandle_t timer_led_blink;

extern uint8_t event_data_ble;
extern uint8_t BLE_COMMAND;
extern uint8_t advertisement_mode;

extern uint16_t connection_id;
extern uint16_t reason_dic;

extern bool set_rtc;
extern bool set_new_alarm;

//extern bool flag_bt_actived;

#endif
