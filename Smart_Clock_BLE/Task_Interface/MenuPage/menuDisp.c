#include "menuDisp.h"

/*Include FreeRTOS*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "task.h"
#include "semphr.h"
#include <timers.h>

#include "main.h"
#include "menu.h"
#include "rtc_sc.h"
#include "main_ble.h"
#include "main_page.h"
#include "timeDatePage.h"
#include "setting_alarm.h"
#include "voice_command.h"
#include "airQualityPage.h"
#include "EnvironmentPage.h"

#define tot_menu (5)

#define TIMEOUT  (1)
static void time_out();

static bool timeout_flag = false;

static uint8_t minute_timeout = 0;
static uint8_t second_timeout = 0;

static void init_menu_disp();
static void deinit_menu_disp();
static void menu_enter(uint8_t *disp);

static void up_Cb();
static void down_Cb();
static void ok_Cb();
static void back_Cb();
static void start_advertisement();


menu_t menuObj;
u8g2_t u8g2;

static uint8_t menu_cursor;
static uint8_t index_back = tot_menu;

typedef enum {
	TIME_DATE,
	ENVIRONMENT,
	AIRQUALITY,
	STOPWATCH,
	ALARM
}menu_var;

const char *menu_list[]={
		"TIME AND DATE",
		"ENVIRONMENT",
		"AIR QUALITY",
		"STOPWATCH",
		"ALARM"
};

const char title_menu[] =
{
		"MENU PAGE"
};

static void init_menu_disp()
{
	button.attachPressed(&btn_obj[BUTTON_UP],up_Cb);
	button.attachPressed(&btn_obj[BUTTON_DOWN],down_Cb);
	button.attachPressed(&btn_obj[BUTTON_ENTER],ok_Cb);
	button.attachPressed(&btn_obj[BUTTON_BACK],back_Cb);

	button.attachHeld(&btn_obj[BUTTON_BACK],start_advertisement);

	interface_construct(&menuObj, &u8g2);
	interface_begin(&menuObj);
	interface_setTitle(&menuObj, title_menu);
	interface_set_menu(&menuObj, menu_list, tot_menu);
	interface_clearAll(&menuObj);

	menu_cursor=255;
	timeout_flag = true;

	vTaskSuspend(voiceHandle);
}

static void deinit_menu_disp()
{
	//	Melakukan deattach button
	for (uint8_t i = 0; i < NUM_OF_BTN; i++)
		button.clearAllISR(&btn_obj[i]);

	interface_clearAll(&menuObj);
}

static void menu_enter(uint8_t *disp)
{
	switch(*disp)
	{
	case TIME_DATE :
		//		Function setting time and date
		rtc_disp();
		break;
	case ENVIRONMENT :
		//		Function BMP Value
		environment_disp();
		break;
	case AIRQUALITY :
		//		Function PASCO2
		airQuality_disp();
		break;
	case STOPWATCH :
		//		Function StopWatch
		stopWatch_disp();
		break;
	case ALARM:
		alarm_disp();
		break;
	}
}

static void draw_menu()
{
	//	interface_draw_menu(&menuObj);
	interface_draw(&menuObj);
	send_buffer_u8g2();
}

void menu_disp_oled()
{
	init_menu_disp();
	while (1)
	{
		time_out();
		if (menu_cursor != 255)
		{
			if (menu_cursor == index_back)
			{
				u8g2_ClearBuffer(&u8g2);
				main_page();
				break;
			}
			else
			{
				deinit_menu_disp();
				menu_enter(&menu_cursor);
				init_menu_disp();
			}
		}
		else
		{
			draw_menu();
		}
		vTaskDelay(20);
	}
	deinit_menu_disp();
}

static void up_Cb()
{
	interface_previous(&menuObj);
	timeout_flag = true;
}

static void down_Cb()
{
	interface_next(&menuObj);
	timeout_flag = true;
}

static void ok_Cb()
{
	menu_cursor = interface_getPosition(&menuObj);
	timeout_flag = true;
}

static void back_Cb()
{
	// Return to main page
	menu_cursor = index_back; //index_back
}

static void start_advertisement()
{
	timeout_flag = true;
	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
}

static void time_out()
{
	cyhal_rtc_read(&rtc_obj, &RTC_TIME);

	if(timeout_flag)
	{
		timeout_flag = false;
		minute_timeout = RTC_TIME.tm_min;
		second_timeout = RTC_TIME.tm_sec;
	}

	if(RTC_TIME.tm_min < minute_timeout)
		minute_timeout = 0;

	if(RTC_TIME.tm_sec < second_timeout)
		second_timeout = 0;

	if ((RTC_TIME.tm_min - minute_timeout > (TIMEOUT)))
	{
		if( (RTC_TIME.tm_sec - second_timeout == (TIMEOUT-1)))
			menu_cursor = index_back;
	}
}
