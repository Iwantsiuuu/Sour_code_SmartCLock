#ifndef WRITE_RTC_TASK_H_
#define WRITE_RTC_TASK_H_

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

#include "main_ble.h"
#include "task_button.h"

void notify_task();
extern char buffer_notify[STRING_BUFFER_SIZE];

#endif
