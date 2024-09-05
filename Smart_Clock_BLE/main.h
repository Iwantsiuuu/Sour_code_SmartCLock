#pragma once

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>

#include "cyhal.h"
#include "cybsp.h"
#include "cy_pdl.h"

/*Include FreeRTOS*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "task.h"
#include "semphr.h"
#include <timers.h>

#include "cy_retarget_io.h"
#include "xensiv_dps3xx_mtb.h"

//Incluce U8G2 Library for Oled and SSD1306
#include "u8g2/u8g2.h"
#include "mtb_ssd1306.h"
#include "u8g2/u8g2_support.h"

//Include Task
#include "rtc_sc.h"
#include "voice_command.h"
#include "main_ble.h"
#include "power_mode.h"
#include "ble_notify_task.h"
#include "setup_i2c.h"
#include "task_sensor.h"
#include "task_button.h"
#include "interface.h"

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/


/*******************************************************************************
 * Exernal variable
 *******************************************************************************/
extern SemaphoreHandle_t semphr_i2c_dev;

extern TaskHandle_t voiceHandle;
extern TaskHandle_t notifyHandle;
extern TaskHandle_t buttonHandle;
extern TaskHandle_t sensorHandle;
extern TaskHandle_t displayHandle;

extern bool systemReady;

#endif
