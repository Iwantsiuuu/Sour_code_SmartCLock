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

#include "stopWatchPage.h"
#include "setting_alarm.h"
#include "airQualityPage.h"
#include "EnvironmentPage.h"

#define MAIN_PAGE_ID 		(1)
#define MENU_PAGE_ID		(2)
#define ENVIRONMENT_PAGE 	(3)
#define AIRQUALITY_PAGE		(4)
#define ALARM_DAILY_PAGE	(5)
#define ALARM_MONTHLY_PAGE	(6)
#define STOPWATCH_PAGE		(7)

#define TIMEOUT				(1ul)

static void enter_menu_cb();
static void start_advertisement();
static void speech_main_cmd(uint32_t cmd);

//callback function when voice id recognize from user
static void daily_cb();
static void monthly_cb();
static void stopwatch_cb();
static void environment_cb();
static void air_quality_cb();

voice_cmd_t voice_bank[5] =
{
		{(uint8_t)LINGKUNGAN_CMD, environment_cb},	//Jump to environment page by voice command
		{(uint8_t)UDARA_CMD, air_quality_cb},		//Jump to air quality page by voice command
		{(uint8_t)STOPWATCH_CMD, stopwatch_cb},		//Jump to StopWatch page by voice command
		{(uint8_t)HARIAN_CMD, daily_cb},			//Jump to Alarm daily page by voice command
		{(uint8_t)BULANAN_CMD, monthly_cb}			//Jump to Alarm monthly page by voice command
};

struct tm RTC_TIME;

char buf_temp [20];

static uint8_t THIS_PAGE = 0;

uint8_t koordinatX_oled = 0, koordinatY_oled = 0;

void main_page()
{
	init_main_page();
	while (1)
	{
		speech_main_cmd(p_command_id);		//Checking command by voice command
		if (THIS_PAGE == MAIN_PAGE_ID)
			main_page_draw();

		else if(THIS_PAGE == MENU_PAGE_ID)
		{
			deinit_main_page();
			menu_page();
			init_main_page();
		}
		vTaskDelay(50);
	}
	deinit_main_page();
}

//Displaying time, date, temperature, and battery percentage
static void main_page_draw()
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

//Register the callback function on each button
static void init_main_page()
{
	button.attachPressed(&btn_obj[BUTTON_ENTER], enter_menu_cb);	//Go to menu page by pressed enter button
	button.attachHeld(&btn_obj[BUTTON_BACK],start_advertisement);	//Start advertisement when the device is not advertising by holding the back button for 1 second

	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );

	THIS_PAGE = MAIN_PAGE_ID;	//
}

//Clear all callback function at this page
static void deinit_main_page()
{
	for (uint8_t i = 0; i < NUM_OF_BTN; i++)
		button.clearAllISR(&btn_obj[i]);
}

//Change from main page to menu page
static void enter_menu_cb()
{
	THIS_PAGE = MENU_PAGE_ID;
}

//Start an ad when the device is not advertising by holding the back button for 1 second
static void start_advertisement()
{
	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
}

static void daily_cb()
{
	deinit_main_page();
	alarm_disp();
	init_main_page();
}
static void monthly_cb()
{
	deinit_main_page();
	alarm_disp();
	init_main_page();
}

static void stopwatch_cb()
{
	deinit_main_page();
	stopwatch_page();
	init_main_page();
}

static void environment_cb()
{
	deinit_main_page();
	environment_disp();
	init_main_page();
}

static void air_quality_cb()
{
	deinit_main_page();
	airQuality_disp();
	init_main_page();
}

//Checks the command from the user and executes the callback function when the command matches the command available on the device.
static void speech_main_cmd(uint32_t cmd)
{
	uint8_t cmd_len = getSize(voice_bank);	//Lots of commands available
	for(uint8_t i = 0; i < cmd_len; i++)
	{
		if(voice_bank[i].cmd_id == ((uint8_t)cmd))	//Comparing commands from the user with commands available on the device
		{
			voice_bank[i].cb();	//Execute callback function when command matching
			break;
		}
	}
}
