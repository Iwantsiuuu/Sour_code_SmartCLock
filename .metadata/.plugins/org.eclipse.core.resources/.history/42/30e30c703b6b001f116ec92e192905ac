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
#include "RTC/rtc_sc.h"
#include "voice_command.h"
#include "Main_BLE/main_ble.h"
#include "Power_Mode/power_mode.h"
#include "Task_Sensor/setup_i2c.h"
#include "Task_ble/write_rtc_task.h"
#include "Task_Sensor/task_sensor.h"
#include "Task_Button/task_button.h"
#include "Task_Interface/interface.h"

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
