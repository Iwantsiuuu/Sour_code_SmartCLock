#include "timeDatePage.h"

/*Include FreeRTOS*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "task.h"
#include "semphr.h"
#include <timers.h>

#include "main.h"
#include "rtc_sc.h"
#include "main_ble.h"
#include "main_page.h"
#include "voice_command.h"
#include "task_interface.h"

/* Macros */
#define RTC_PAGE 1

#define DEAFULT_VARIABLE_VALUE 		(0)
#define NUM_DAY						(31)
#define NUM_MONTH					(12)
#define NUM_HOUR					(23)
#define NUM_MINUTE					(59)
#define NUM_SECON					(59)
#define ENABLE 						(1)

/* Prototype function */
static void rtc_draw();
static void init_rtc_disp();
static void deinit_rtc_disp();

static void confirm_cb();
static void back_rtc_cb();
static void switch_var_cb();
static void increment_var_cb();
static void decrement_var_cb();
//static void increment_var_held_cb();
//static void decrement_var_held_cb();
static void start_advertisement();

#define TIMEOUT  (1)
static void time_out();

static bool timeout_flag = false;

static uint8_t minute_timeout = 0;
static uint8_t second_timeout = 0;

/* Gelobal Variable */
static uint8_t num_var = 6;
static uint8_t THIS_PAGE = 0;
static uint8_t current_var = 0;
static uint8_t idx_back = RTC_PAGE + 1;
static uint8_t return_to_main = RTC_PAGE + 2;

bool confirm_flag_rtc = false;
static bool RTC_ENABLE = false;

enum var_rtc{
	Hour,
	Minute,
	Secon,
	Day,
	Month,
	Year
};


/*******************************************************************************
* Function Name: init_rtc_disp
********************************************************************************
* Summary:
*  The init_rtc_disp function used for register button for set RTC parameter,
*  clear previous buffer and display, reset the voice command variable value, resume voice command task,
*  and set page to RTC page.
*
* Parameters:
*  -
*
* Return:
*  -
*
*******************************************************************************/
void init_rtc_disp()
{
	button.attachPressed(&btn_obj[BUTTON_UP],increment_var_cb);
	button.attachPressed(&btn_obj[BUTTON_DOWN],decrement_var_cb);
	button.attachPressed(&btn_obj[BUTTON_ENTER],switch_var_cb);
	button.attachPressed(&btn_obj[BUTTON_BACK],back_rtc_cb);

//	button.attachHeld(&btn_obj[BUTTON_UP],increment_var_held_cb);
//	button.attachHeld(&btn_obj[BUTTON_DOWN],decrement_var_held_cb);
	button.attachHeld(&btn_obj[BUTTON_ENTER],confirm_cb);
	button.attachHeld(&btn_obj[BUTTON_BACK],start_advertisement);

	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);

	timeout_flag = true;

	THIS_PAGE = RTC_PAGE;

	vTaskResume(voiceHandle);
}

/*******************************************************************************
* Function Name: deinit_rtc_disp
********************************************************************************
* Summary:
*  The deinit_rtc_disp function used for deleted callback function on button
*
* Parameters:
*  -
*
* Return:
*  -
*
*******************************************************************************/
static void deinit_rtc_disp()
{
	/* Delete calback function for quick press on all button */
	for (uint8_t i = 0; i < NUM_OF_BTN; i++)
		button.clearAllISR(&btn_obj[i]);
}

/*******************************************************************************
* Function Name: rtc_draw
********************************************************************************
* Summary:
*  The rtc_draw function used to display the value of the RTC variable to the LCD
*
* Parameters:
*  -
*
* Return:
*  -
*
*******************************************************************************/
static void rtc_draw()
{
	char buff_HH[10];
	char buff_MM[10];
	char buff_SS[10];
	char buff_dd[10];
	char buff_mm[10];
	char buff_yy[10];

	sprintf(buff_HH,"HH: %d",RTC_Setup.hour);
	sprintf(buff_MM,"MM: %d", RTC_Setup.min);
	sprintf(buff_SS,"SS: %d", RTC_Setup.sec);
	sprintf(buff_dd,"dd: %d", RTC_Setup.mday);
	sprintf(buff_mm,"mm: %d", RTC_Setup.month);
	sprintf(buff_yy,"yy: %d", RTC_Setup.Year);

	u8g2_DrawStr(&u8g2_obj, 0, 10, "Time & date settings");

	u8g2_DrawStr(&u8g2_obj, 2, 30, buff_HH);
	u8g2_DrawStr(&u8g2_obj, 60, 30, buff_dd);

	u8g2_DrawStr(&u8g2_obj, 2, 40, buff_MM);
	u8g2_DrawStr(&u8g2_obj, 60, 40, buff_mm);

	u8g2_DrawStr(&u8g2_obj, 2, 50, buff_SS);
	u8g2_DrawStr(&u8g2_obj, 60, 50, buff_yy);
	send_buffer_u8g2();

	if(confirm_flag_rtc)
		set_new_time();
}

/*******************************************************************************
* Function Name: rtc_draw
********************************************************************************
* Summary:
*  The rtc_draw function used to display the value of the RTC variable to the user
*
* Parameters:
*  -
*
* Return:
*  -
*
*******************************************************************************/
void rtc_disp()
{
	init_rtc_disp();
	while (1)
	{
		time_out();
#ifdef UNUSE_I2S
		printf("\tRTC_M: %d, RTC_S: %d\r\n\tMENIT: %d, DETIK: %d\r\n", RTC_TIME.tm_min, RTC_TIME.tm_sec, minute_timeout, second_timeout);
#endif
		if (THIS_PAGE == RTC_PAGE)
			rtc_draw();

		else if(THIS_PAGE == idx_back)
		{
			deinit_rtc_disp();
			menu_page();
			break;
		}
		else
		{
			deinit_rtc_disp();
			main_page();
			break;
		}
		vTaskDelay(20);
	}
	deinit_rtc_disp();
}

/*******************************************************************************
* Function Name: rtc_set_first
********************************************************************************
* Summary:
*  The rtc_draw function used to display the value of the RTC variable to the user
*
* Parameters:
*  -
*
* Return:
*  -
*
*******************************************************************************/
void rtc_set_first()
{
	init_rtc_disp();
	while(1)
	{
		if ((RTC_ENABLE = cyhal_rtc_is_enabled(&rtc_obj)) == ENABLE)
			THIS_PAGE = idx_back; //index_back

		if (THIS_PAGE == RTC_PAGE)
			rtc_draw();
		else
		{
			deinit_rtc_disp();
			break;
		}
		vTaskDelay(20);
	}
	deinit_rtc_disp();
}

/*******************************************************************************
* Function Name: increment_var_cb
********************************************************************************
* Summary:
*  The increment_var_cb function used to increment variable RTC such as hour, minute, second, day, month, and year
*
* Parameters:
*  -
*
* Return:
*  -
*
*******************************************************************************/
static void increment_var_cb()
{
	timeout_flag = true;
	switch (current_var)
	{
	case Hour :
		if (RTC_Setup.hour > NUM_HOUR)
			RTC_Setup.hour = DEAFULT_VARIABLE_VALUE;
		RTC_Setup.hour++;
		break;

	case Minute :
		if (RTC_Setup.min > NUM_MINUTE)
			RTC_Setup.min = DEAFULT_VARIABLE_VALUE;
		RTC_Setup.min++;
		break;

	case Secon :
		if (RTC_Setup.sec > NUM_SECON)
			RTC_Setup.sec = DEAFULT_VARIABLE_VALUE;
		RTC_Setup.sec++;
		break;

	case Day :
		if (RTC_Setup.mday > NUM_DAY)
			RTC_Setup.mday = DEAFULT_VARIABLE_VALUE;
		RTC_Setup.mday++;
		break;

	case Month :
		if (RTC_Setup.month > NUM_MONTH)
			RTC_Setup.month = DEAFULT_VARIABLE_VALUE;
		RTC_Setup.month++;
		break;

	case Year :
		RTC_Setup.Year++;
		break;

	default:
		break;
	}
}

/*******************************************************************************
* Function Name: decrement_var_cb
********************************************************************************
* Summary:
*  The decrement_var_cb function used to decrement variable RTC such as hour, minute, second, day, month, and year
*
* Parameters:
*  -
*
* Return:
*  -
*
*******************************************************************************/
static void decrement_var_cb()
{
	timeout_flag = true;
	switch (current_var)
	{
	case Hour :
		RTC_Setup.hour--;
		if (RTC_Setup.hour < DEAFULT_VARIABLE_VALUE)
			RTC_Setup.hour = NUM_HOUR;
		break;

	case Minute :
		RTC_Setup.min--;
		if (RTC_Setup.min < DEAFULT_VARIABLE_VALUE)
			RTC_Setup.min = NUM_MINUTE;
		break;

	case Secon :
		RTC_Setup.sec--;
		if (RTC_Setup.sec < DEAFULT_VARIABLE_VALUE)
			RTC_Setup.sec = NUM_SECON;
		break;

	case Day :
		RTC_Setup.mday--;
		if (RTC_Setup.mday < DEAFULT_VARIABLE_VALUE)
			RTC_Setup.mday = NUM_DAY;
		break;

	case Month :
		RTC_Setup.month--;
		if (RTC_Setup.month < DEAFULT_VARIABLE_VALUE)
			RTC_Setup.month = NUM_MONTH;
		break;

	case Year :
		RTC_Setup.Year--;
		break;

	default:
		break;
	}
}

//static void increment_var_held_cb()
//{
//	timeout_flag = true;
//	switch (current_var)
//	{
//	case Hour :
//		if (RTC_Setup.hour > NUM_HOUR)
//			RTC_Setup.hour = DEAFULT_VARIABLE_VALUE;
//		RTC_Setup.hour++;
//		break;
//
//	case Minute :
//		if (RTC_Setup.min > NUM_MINUTE)
//			RTC_Setup.min = DEAFULT_VARIABLE_VALUE;
//		RTC_Setup.min++;
//		break;
//
//	case Secon :
//		if (RTC_Setup.sec > NUM_SECON)
//			RTC_Setup.sec = DEAFULT_VARIABLE_VALUE;
//		RTC_Setup.sec++;
//		break;
//
//	case Day :
//		if (RTC_Setup.mday > NUM_DAY)
//			RTC_Setup.mday = DEAFULT_VARIABLE_VALUE;
//		RTC_Setup.mday++;
//		break;
//
//	case Month :
//		if (RTC_Setup.month > NUM_MONTH)
//			RTC_Setup.month = DEAFULT_VARIABLE_VALUE;
//		RTC_Setup.month++;
//		break;
//
//	case Year :
//		RTC_Setup.Year++;
//		break;
//
//	default:
//		break;
//	}
//}
//
//static void decrement_var_held_cb()
//{
//	timeout_flag = true;
//	switch (current_var)
//	{
//	case Hour :
//		RTC_Setup.hour--;
//		if (RTC_Setup.hour < DEAFULT_VARIABLE_VALUE)
//			RTC_Setup.hour = NUM_HOUR;
//		break;
//
//	case Minute :
//		RTC_Setup.min--;
//		if (RTC_Setup.min < DEAFULT_VARIABLE_VALUE)
//			RTC_Setup.min = NUM_MINUTE;
//		break;
//
//	case Secon :
//		RTC_Setup.sec--;
//		if (RTC_Setup.sec < DEAFULT_VARIABLE_VALUE)
//			RTC_Setup.sec = NUM_SECON;
//		break;
//
//	case Day :
//		RTC_Setup.mday--;
//		if (RTC_Setup.mday < DEAFULT_VARIABLE_VALUE)
//			RTC_Setup.mday = NUM_DAY;
//		break;
//
//	case Month :
//		RTC_Setup.month--;
//		if (RTC_Setup.month < DEAFULT_VARIABLE_VALUE)
//			RTC_Setup.month = NUM_MONTH;
//		break;
//
//	case Year :
//		RTC_Setup.Year--;
//		break;
//
//	default:
//		break;
//	}
//}

static void switch_var_cb()
{
	timeout_flag = true;
	current_var++;
	if (current_var > num_var)
		current_var = 0;
}

//Set new time by holding enter button
static void confirm_cb()
{
	confirm_flag_rtc = true;
	timeout_flag = true;
}

//Back to menu page
static void back_rtc_cb()
{
	THIS_PAGE = idx_back; //index_back
}

//Start advertisement when the device is not advertising by holding the back button for 1 second
static void start_advertisement()
{
	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
}

static void time_out()
{
	cyhal_rtc_read(&rtc_obj, &RTC_TIME);

	if(timeout_flag)
	{
		minute_timeout = RTC_Setup.min;
		second_timeout = RTC_Setup.sec;
		timeout_flag = false;
	}

	if(RTC_Setup.min < minute_timeout)
		minute_timeout = 0;

	if(RTC_Setup.sec < second_timeout)
		second_timeout = 0;

	if ((RTC_Setup.min - minute_timeout > (TIMEOUT+1)) && (RTC_Setup.sec - second_timeout == (TIMEOUT-1)))
	{
		THIS_PAGE = return_to_main;
	}
}
