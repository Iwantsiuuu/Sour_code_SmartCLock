#include "setting_alarm.h"
#include "main_ble.h"
#include "main_page.h"

#define ALARM_PAGE 1

#define DEAFULT_VARIABLE_VALUE 		(0)
#define NUM_DAY						(31)
#define NUM_MONTH					(12)
#define NUM_HOUR					(23)
#define NUM_MINUTE					(59)
#define NUM_SECON					(59)

#define TIMEOUT  (1)

static void time_out();

static bool timeout_flag = false;

static uint8_t minute_timeout = 0;
static uint8_t second_timeout = 0;

/* Prototype function */
static void init_alarm_disp();
static void deinit_alarm_disp();
static void alarm_draw();

static void increment_var_cb();
static void decrement_var_cb();
static void switch_var_cb();
static void confirm_cb();
static void back_alarm_cb();

static void set_daily();
static void set_monthly();
static void start_advertisement();

/* Global Variable */
static uint8_t num_var = 1;
static uint8_t THIS_PAGE = 0;
static uint8_t current_var = 0;

static uint8_t idx_back = ALARM_PAGE+1;
static uint8_t return_to_main_page = ALARM_PAGE + 2;

bool daily = false;
bool monthly = false;

static bool confirm_alarm_flag = false;

enum var_alarm{
	Hour,
	Minute,
	Secon,
	Day,
	Month,
	Year
};

//Register the callback function on each button, clear buffer and LCD display, and resume voice command task
void init_alarm_disp()
{
	button.attachPressed(&btn_obj[BUTTON_UP],increment_var_cb);		//Increment variable RTC such as hour, minute, second, day, month, and year by pressed up button
	button.attachPressed(&btn_obj[BUTTON_DOWN],decrement_var_cb);	//Decrement variable RTC such as hour, minute, second, day, month, and year by pressed down button
	button.attachPressed(&btn_obj[BUTTON_ENTER],switch_var_cb);		//Switch variable RTC such as hour, minute, second, day, month, and year by pressed enter button
	button.attachPressed(&btn_obj[BUTTON_BACK],back_alarm_cb);		//Back to menu page by pressed back button

	button.attachHeld(&btn_obj[BUTTON_ENTER],confirm_cb);			//Set new alarm by pressing enter button for 1 second
	button.attachHeld(&btn_obj[BUTTON_UP],set_daily);				//Pressing the up button for 1 second to set the daily alarm
	button.attachHeld(&btn_obj[BUTTON_DOWN],set_monthly);			//Pressing the down button for 1 second to set the monthly alarm

	button.attachHeld(&btn_obj[BUTTON_BACK],start_advertisement);	//Start advertisement when the device is not advertising by holding the back button for 1 second

	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);

	timeout_flag = true;

	THIS_PAGE = ALARM_PAGE;
}

//Clear all callback function each button
static void deinit_alarm_disp()
{
	for (uint8_t i = 0; i < NUM_OF_BTN; i++)
		button.clearAllISR(&btn_obj[i]);
}

//Displaying alarm page
static void alarm_draw()
{
	char buff_show_alarm_active[10];

	char buff_HH[10];
	char buff_MM[10];

	if(daily == false && monthly == false)
	{
		sprintf(buff_show_alarm_active, "D: %d:%d | M: %d, %d:%d", daily_alarm_show.hour, daily_alarm_show.minute, monthly_alarm_show.date, monthly_alarm_show.hour, monthly_alarm_show.minute);
		u8g2_DrawStr(&u8g2_obj, 0, 10, "ALARM SETUP");
		u8g2_DrawStr(&u8g2_obj, 0, 25, buff_show_alarm_active);
		u8g2_DrawStr(&u8g2_obj, 2, 40, "Hold button set");
		u8g2_DrawStr(&u8g2_obj, 2, 50, "1. Back for daily");
		u8g2_DrawStr(&u8g2_obj, 2, 60, "2. Down for monthly");
		send_buffer_u8g2();
	}

	if(daily == true)
	{
		monthly = false;
		u8g2_DrawStr(&u8g2_obj, 0, 10, "ALARM SETUP DAILY");
		sprintf(buff_HH,"HH: %d",daily_alarm.hour);
		sprintf(buff_MM,"MM: %d", daily_alarm.min);

		u8g2_DrawStr(&u8g2_obj, 2, 30, buff_HH);
		u8g2_DrawStr(&u8g2_obj, 60, 30, buff_MM);
		send_buffer_u8g2();
	}

	if(monthly == true)
	{
		daily =false;
		u8g2_DrawStr(&u8g2_obj, 0, 10, "ALARM SETUP MONTHLY");
		sprintf(buff_HH,"HH: %d",montly_alarm.hour);
		sprintf(buff_MM,"MM: %d", montly_alarm.min);

		u8g2_DrawStr(&u8g2_obj, 2, 30, buff_HH);
		u8g2_DrawStr(&u8g2_obj, 60, 30, buff_MM);
		send_buffer_u8g2();
	}

	if(confirm_alarm_flag)
		set_alarm();
}

void alarm_disp()
{
	init_alarm_disp();

	while (1)
	{
		time_out();
#ifdef UNUSE_I2S
		printf("\tRTC_M: %d, RTC_S: %d\r\n\tMENIT: %d, DETIK: %d\r\n", RTC_TIME.tm_min, RTC_TIME.tm_sec, minute_timeout, second_timeout);
#endif
		if(p_command_id == KEMBALI_CMD)
			THIS_PAGE = return_to_main_page;

		if (THIS_PAGE == ALARM_PAGE)
		{
			alarm_draw();
		}
		else if(THIS_PAGE == idx_back)
		{
			deinit_alarm_disp();
			menu_page();
			break;
		}
		else
		{
			deinit_alarm_disp();
			main_page();
			break;
		}
		vTaskDelay(20);
	}
	deinit_alarm_disp();
}

static void increment_var_cb()
{
	timeout_flag = true;
	switch (current_var)
	{
	case Hour :
		if(daily)
		{
			daily_alarm.hour++;
			if (daily_alarm.hour > NUM_HOUR)
				daily_alarm.hour = DEAFULT_VARIABLE_VALUE;
		}
		if(monthly)
		{
			montly_alarm.hour++;
			if (montly_alarm.hour > NUM_HOUR)
				montly_alarm.hour = DEAFULT_VARIABLE_VALUE;
		}
		break;

	case Minute :
		if(daily)
		{
			daily_alarm.min++;
			if (daily_alarm.min > NUM_MINUTE)
				daily_alarm.min = DEAFULT_VARIABLE_VALUE;
		}
		if(monthly)
		{
			montly_alarm.min++;
			if (montly_alarm.min > NUM_MINUTE)
				montly_alarm.min = DEAFULT_VARIABLE_VALUE;
		}
		break;

	default:
		break;
	}
}
static void decrement_var_cb()
{
	timeout_flag = true;
	switch (current_var)
	{
	case Hour :
		if(daily)
		{
			daily_alarm.hour--;
			if (daily_alarm.hour < DEAFULT_VARIABLE_VALUE)
				daily_alarm.hour = NUM_HOUR;
		}
		if(monthly)
		{
			montly_alarm.hour--;
			if (montly_alarm.hour < DEAFULT_VARIABLE_VALUE)
				montly_alarm.hour = NUM_HOUR;
		}
		break;

	case Minute :
		if(daily)
		{
			daily_alarm.min--;
			if (daily_alarm.min < DEAFULT_VARIABLE_VALUE)
				daily_alarm.min = NUM_MINUTE;
		}
		if(monthly)
		{
			montly_alarm.min--;
			if (montly_alarm.min < DEAFULT_VARIABLE_VALUE)
				montly_alarm.min = NUM_MINUTE;
		}
		break;

	default:
		break;
	}
}

static void set_daily()
{
	timeout_flag = true;
	monthly = false;
	daily = true;
}

static void set_monthly()
{
	timeout_flag = true;
	daily = false;
	monthly = true;
}

static void start_advertisement()
{
	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
}

//Switch variable alarm by pressing enter button
static void switch_var_cb()
{
	timeout_flag = true;
	current_var++;
	if (current_var > num_var)
		current_var = 0;
}

//Set new alarm by pressing enter button for 1 second
static void confirm_cb()
{
	timeout_flag = true;
	confirm_alarm_flag = true;
}

//Back to menu page by pressed back button
static void back_alarm_cb()
{
	THIS_PAGE = idx_back; //index_back
	daily = false;
	monthly = false;
}

//Return to main page when no button is pressed for 3 minutes
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

	if ((RTC_TIME.tm_min - minute_timeout > (TIMEOUT+1)) && (RTC_TIME.tm_sec - second_timeout == (TIMEOUT-1)))
	{
		THIS_PAGE = return_to_main_page;
	}
}
