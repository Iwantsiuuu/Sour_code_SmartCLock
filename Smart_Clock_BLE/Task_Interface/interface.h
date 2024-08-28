#ifndef INTERFACE_H_
#define INTERFACE_H_

#include "cyhal.h"
#include "cybsp.h"
#include "stdio.h"
#include "cy_retarget_io.h"

/*Include FreeRTOS*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"
#include "task.h"

#include "mtb_ssd1306.h"
#include "u8g2.h"
#include "u8g2/u8x8.h"
#include "u8g2_support.h"

#include "setup_i2c.h"
#include "button.h"
#include "task_button.h"
#include "task_interface.h"

#endif
