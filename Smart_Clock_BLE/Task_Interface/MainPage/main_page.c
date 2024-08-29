/*
 *
 * main_page.c
 *
 * Created on  : 03 April 2024
 * 	    Author : Ridwan Bahari
 * 	    E-Mail : ridwanbahari236@gmail.com
 *
 */

#include "main_page.h"

#include "rtc_sc.h"
#include "main_ble.h"
#include "power_mode.h"
#include "task_button.h"
#include "task_sensor.h"
#include "setting_alarm.h"
#include "voice_command.h"
#include "task_interface.h"
#include "airQualityPage.h"
#include "EnvironmentPage.h"

#define MAIN_PAGE_ID 		(1)
#define MENU_PAGE_ID		(2)
#define ENVIRONMENT_PAGE 	(3)
#define AIRQUALITY_PAGE		(4)
#define ALARM_DAILY_PAGE	(5)
#define ALARM_MONTHLY_PAGE	(6)

#define TIMEOUT				(1ul)

static void enter_menu_cb();
static void start_advertisement();
static void speech_main_cmd(uint32_t cmd);

struct tm RTC_TIME;

char buf_temp [20];

static uint8_t THIS_PAGE = 0;
static uint8_t MODE_DISPLAY = 0;

uint8_t koordinatX_oled = 0, koordinatY_oled = 0;

void main_page()
{
	init_main_page();

	while (1)
	{
		speech_main_cmd(p_command_id);

		if (THIS_PAGE == MAIN_PAGE_ID)
			default_mode_draw();

		else if(THIS_PAGE == MENU_PAGE_ID)
		{
			deinit_main_page();
			menu_disp_oled();
			init_main_page();
		}
		else if(THIS_PAGE == ENVIRONMENT_PAGE)
		{
			deinit_main_page();
			environment_disp();
			init_main_page();
		}
		vTaskDelay(50);
	}
	deinit_main_page();
}

static void default_mode_draw()
{
	char buf_time[STRING_BUFFER_SIZE];
	char buf_date[STRING_BUFFER_SIZE];

	cyhal_rtc_read(&rtc_obj, &RTC_TIME);

	strftime(buf_time, sizeof(buf_time), "%X %p", &RTC_TIME);
	strftime(buf_date, STRING_BUFFER_SIZE, "%b %d, %Y", &RTC_TIME);

#ifdef USE_DUMMY_DATA
	sprintf(buf_temp,"%0.0f C\xB0\r\n", dps_sensor.temperature);
#endif

#ifdef USE_BME680
	sprintf(buf_temp,"%0.0f C\xB0\r\n", bme680_sensor.temperature);
#endif

#ifdef USE_BMP280
	sprintf(buf_temp,"%0.0f C\xB0\r\n", bmp280_sensor.temperature);
#endif

#ifdef USE_DPS310
	sprintf(buf_temp,"%0.0f C\xB0\r\n", dps_sensor.temperature);
#endif

	u8g2_DrawStr(&u8g2_obj, (koordinatX_oled+110), (koordinatY_oled+8), "80%");
	u8g2_DrawStr(&u8g2_obj, (koordinatX_oled+35), (koordinatY_oled+25), buf_time);
	u8g2_DrawStr(&u8g2_obj, (koordinatX_oled+30), (koordinatY_oled+35), buf_date);
	u8g2_DrawStr(&u8g2_obj, (koordinatX_oled+45), (koordinatY_oled+50), buf_temp);

	send_buffer_u8g2();
}

static void init_main_page()
{
	/* Initialization button with callback function */
	button.attachPressed(&btn_obj[BUTTON_ENTER], enter_menu_cb);
	button.attachDoublePressed(&btn_obj[BUTTON_ENTER],start_advertisement);

	THIS_PAGE = MAIN_PAGE_ID;
	MODE_DISPLAY = 0;

	cyhal_rtc_read(&rtc_obj, &RTC_TIME);
	//	timeout_flag = true;
}

static void deinit_main_page()
{
	// deAttach button
	for (uint8_t i = 0; i < NUM_OF_BTN; i++)
	{
		button.dettachPressed(&btn_obj[i]);
		button.dettachDoublePressed(&btn_obj[i]);
	}
}

static void enter_menu_cb()
{
	//	timeout_flag = true;
	THIS_PAGE = MENU_PAGE_ID;
}

static void start_advertisement()
{
	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
}

static void speech_main_cmd(uint32_t cmd)
{
	switch(cmd)
	{
	case ENVIRONMENT_CMD:
		//		timeout_flag = true;
		THIS_PAGE = ENVIRONMENT_PAGE;
		break;
	case AIR_QUALITY_CMD:
		THIS_PAGE = AIRQUALITY_PAGE;
		break;
	case ALARM_SETTING_CMD:
		THIS_PAGE = ALARM_DAILY_PAGE;
		break;
	default:
		break;
	}
}
