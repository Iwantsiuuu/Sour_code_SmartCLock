#include "stopwatchPage.h"

#include "menuDisp.h"
#include "main_ble.h"
#include "main_page.h"
#include "voice_command.h"

#define SW_PAGE 1
#define TIMEOUT  (1)

static void time_out();

static bool timeout_flag = false;

static uint8_t minute_timeout = 0;
static uint8_t second_timeout = 0;

/*--------------< Prototype function >---------------*/
static void BackSW_Cb();
static void stopwatch();
static void startSW_Cb();
static void pauseSW_Cb();
static void resetSW_Cb();
static void stopwatch_draw();
static void start_advertisement();
static void init_stopwatch_disp();
static void deinit_stopwatch_disp();
static void speech_stopwatch_cmd(uint32_t cmd);

static void start_cb();
static void stop_cb();
static void reset_cb();
static void back_cb();

/*--------------< Global function >----------------*/
static bool countSW = false;
static char buf_stopwatch[100];

static voice_cmd_t voice_bank[4] =
{
		{(uint8_t)MULAI_CMD, start_cb},
		{(uint8_t)BERHENTI_CMD, stop_cb},
		{(uint8_t)ULANG_CMD, reset_cb},
		{(uint8_t)KEMBALI_CMD, back_cb},
};

static uint8_t THIS_PAGE = 0;
static uint8_t idx_back = SW_PAGE+1;

static uint16_t hour = 0;
static uint16_t minute = 0;
static uint16_t second = 0;
static uint16_t mili_second = 0;
static uint16_t over;

static uint32_t firstT, time_now, dtstopwatch;

//Register the callback function on each button, clear buffer and LCD display, and resume voice command task
void init_stopwatch_disp()
{
	button.attachPressed(&btn_obj[BUTTON_ENTER],startSW_Cb);	//Start the StopWatch count by pressed enter button
	button.attachPressed(&btn_obj[BUTTON_DOWN],pauseSW_Cb);		//Pause the StopWatch count by pressed down button
	button.attachPressed(&btn_obj[BUTTON_UP],resetSW_Cb);		//Reset the StopWatch count by pressed up button
	button.attachPressed(&btn_obj[BUTTON_BACK],BackSW_Cb);		//Back to menu page

	//Start advertisement when the device is not advertising by holding the back button for 1 second
	button.attachHeld(&btn_obj[BUTTON_BACK],start_advertisement);

	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);

	timeout_flag = true;

	THIS_PAGE = SW_PAGE;

	vTaskResume(voiceHandle);
}

//Clear all callback function each button and suspend voice command task
void deinit_stopwatch_disp()
{
	for (uint8_t i = 0; i < 4; i++)
		button.clearAllISR(&btn_obj[i]);
	vTaskSuspend(voiceHandle);
}

//Count StopWatch
void stopwatch()
{
	if (countSW)
	{
		time_now = xTaskGetTickCount();
		dtstopwatch = time_now - firstT;

		hour 		= dtstopwatch / 3600000;
		over 		= dtstopwatch % 3600000;
		minute 		= over / 60000;
		over 		= over % 60000;
		second 		= over / 1000;
		mili_second = over % 1000;
	}
	else if (countSW == false && dtstopwatch == 0)
	{
		hour = 0, minute = 0, second = 0, mili_second = 0;
	}
	sprintf(buf_stopwatch,"%d:%d:%d:%d\r\n",hour,minute,second,mili_second);
}

//Displaying StopWatch count
void stopwatch_draw()
{
	stopwatch();
	u8g2_DrawStr(&u8g2_obj, 0, 15, "stopwatch");
	u8g2_DrawStr(&u8g2_obj, 0, 25, buf_stopwatch);
	send_buffer_u8g2();
}

void stopwatch_page()
{
	init_stopwatch_disp();

	while (1)
	{
		time_out();
#ifdef UNUSE_I2S
		printf("RTC_M: %d, RTC_S: %d\r\nMENIT: %d, DETIK: %d\r\n", RTC_TIME.tm_min, RTC_TIME.tm_sec, minute_timeout, second_timeout);
#endif
		speech_stopwatch_cmd(p_command_id);

		if (THIS_PAGE == SW_PAGE)
		{
			stopwatch_draw();
		}
		else if(THIS_PAGE == idx_back)
		{
			deinit_stopwatch_disp();
			menu_page();
		}
		vTaskDelay(20);
	}
	deinit_stopwatch_disp();
}

static void start_advertisement()
{
	timeout_flag = true;
	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
}

//Start count the StopWatch by pressed enter button
static void startSW_Cb()
{
	timeout_flag = true;
	firstT = xTaskGetTickCount() - dtstopwatch;
	countSW = true;
}

//Pause count the StopWatch by pressed down button
static void pauseSW_Cb()
{
	timeout_flag = true;
	countSW = false;
}

//Reset count the StopWatch by pressed up button
static void resetSW_Cb()
{
	timeout_flag = true;
	countSW = false;
	dtstopwatch = 0;
}

//Start count the StopWatch by pressed enter button
static void BackSW_Cb()
{
	THIS_PAGE = idx_back;
}

//Start count the StopWatch by voice command
static void start_cb()
{
	timeout_flag = true;
	firstT = xTaskGetTickCount() - dtstopwatch;
	countSW = true;
}

//Stop count the StopWatch by voice command
static void stop_cb()
{
	timeout_flag = true;
	countSW = false;
}

//Reset count the StopWatch by voice command
static void reset_cb()
{
	timeout_flag = true;
	countSW = false;
	dtstopwatch = 0;
}

//Back to main page by voice command
static void back_cb()
{
	deinit_stopwatch_disp();
	main_page();
}

//Checks the command from the user and executes the callback function when the command matches the command available on the device.
static void speech_stopwatch_cmd(uint32_t cmd)
{
	uint8_t cmd_len = getSize(voice_bank);			//Lots of commands available
	for(uint8_t i = 0; i < cmd_len; i++)
	{
		if(voice_bank[i].cmd_id == ((uint8_t)cmd))	//Comparing commands from the user with commands available on the device
		{
			voice_bank[i].cb();						//Execute callback function when command matching
			break;
		}
	}
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
		deinit_stopwatch_disp();
		main_page();
	}
}
