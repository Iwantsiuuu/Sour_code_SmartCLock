/******************************************************************************
 * Include header files
 ******************************************************************************/
#include "cyhal.h"
#include "cycfg.h"
#include "cybsp.h"
#include "time.h"

#include "string.h"
#include "rtc_sc.h"
#include "main_ble.h"
#include "setting_alarm.h"
#include "voice_command.h"

/******************************************************************************
 * Prototype variable
 ******************************************************************************/
static bool validate_date_time(int sec, int min, int hour, int mday, int month, int year);

static int get_day_of_week(int day, int month, int year);
static int get_week_of_month(int day, int month, int year);

/******************************************************************************
 * Gelobal variable
 ******************************************************************************/
alarm_t daily_alarm_show;
alarm_t monthly_alarm_show;

cyhal_rtc_t rtc_obj;

RTC_Data_Setup_t RTC_Setup;
RTC_Data_Setup_t daily_alarm;
RTC_Data_Setup_t montly_alarm;

cyhal_alarm_active_t alarm_active_montly =
{
		.en_sec   = 0,
		.en_min   = 1,
		.en_hour  = 1,
		.en_day   = 0,
		.en_date  = 1,
		.en_month = 0,
};

cyhal_alarm_active_t alarm_active_daily =
{
		.en_sec   = 0,
		.en_min   = 1,
		.en_hour  = 1,
		.en_day   = 0,
		.en_date  = 0,
		.en_month = 0,
};

void handle_error(void)
{
	/* Disable all interrupts. */
	__disable_irq();

	CY_ASSERT(0);
}

/*******************************************************************************
 * Function Name: set_new_time
 ********************************************************************************
 * Summary:
 *  This functions takes the user input and sets the new date and time.
 *
 * Parameter:
 *  uint32_t timeout_ms : Maximum allowed time (in milliseconds) for the
 *  function
 *
 * Return :
 *  void
 *******************************************************************************/

void set_new_time()
{
	cy_rslt_t rslt;

	struct tm new_time = {0};

	//	printf("\rEnter time in \"HH MM SS dd mm yyyy\" format \r\n");
	//	printf("%d, %d, %d, %d, %d, %d\r\n", RTC_Setup.hour, RTC_Setup.min, RTC_Setup.sec, RTC_Setup.mday, RTC_Setup.month, RTC_Setup.Year);

	if (validate_date_time(RTC_Setup.hour, RTC_Setup.min, RTC_Setup.hour, RTC_Setup.mday, RTC_Setup.month, RTC_Setup.Year))
	{
		new_time.tm_sec = RTC_Setup.sec;
		new_time.tm_min = RTC_Setup.min;
		new_time.tm_hour = RTC_Setup.hour;
		new_time.tm_mday = RTC_Setup.mday;
		new_time.tm_mon = RTC_Setup.month - 1;
		new_time.tm_year = RTC_Setup.Year - TM_YEAR_BASE;
		new_time.tm_wday = get_day_of_week(RTC_Setup.mday, RTC_Setup.month, RTC_Setup.Year);

		/* Format set RTC using Airoc Connect
		 *
		 * "Hour Minute Second Day Mount Year"
		 * example: RtcV 12 30 0 20 8 2024#
		 *
		 */

		rslt = cyhal_rtc_write(&rtc_obj, &new_time);
		if (CY_RSLT_SUCCESS == rslt)
		{
#ifdef UNUSE_I2S
			printf("\rRTC time updated\r\n\n");
#endif
		}
		else
		{
			handle_error();
		}
	}
}

void set_alarm()
{
	cy_rslt_t rslt;

	struct tm alarm_set_daily = {0};
	struct tm alarm_set_monthly = {0};

	struct tm time_set;

	if(daily)
	{
		cyhal_rtc_read(&rtc_obj, &time_set);

		alarm_set_daily.tm_sec = 0;
		alarm_set_daily.tm_min = daily_alarm.min;
		alarm_set_daily.tm_hour = daily_alarm.hour;
		alarm_set_daily.tm_mday = time_set.tm_mday;
		alarm_set_daily.tm_mon = time_set.tm_mon - 1;
		alarm_set_daily.tm_year = time_set.tm_year - TM_YEAR_BASE;
		alarm_set_daily.tm_wday = get_day_of_week(time_set.tm_mday, time_set.tm_mon, time_set.tm_year);

		/* Format set alarm daily using AIROC Connect
		 *
		 * "Daily Hour Minute#"
		 * example: Daily 12 30#
		 *
		 */

		rslt = cyhal_rtc_set_alarm(&rtc_obj, &alarm_set_daily, alarm_active_montly);
		if (CY_RSLT_SUCCESS == rslt)
		{
			daily_alarm_show.hour = alarm_set_daily.tm_hour;
			daily_alarm_show.minute = alarm_set_daily.tm_min;

			daily = false;
#ifdef UNUSE_I2S
			printf("\rAlarm daily set\r\n\n");
#endif
		}
		else
		{
			handle_error();
		}
	}
	if(monthly)
	{
		cyhal_rtc_read(&rtc_obj, &time_set);

		alarm_set_monthly.tm_sec = 0;
		alarm_set_monthly.tm_min = montly_alarm.min;
		alarm_set_monthly.tm_hour = montly_alarm.hour;
		alarm_set_monthly.tm_mday = time_set.tm_mday;
		alarm_set_monthly.tm_mon = time_set.tm_mon - 1;
		alarm_set_monthly.tm_year = time_set.tm_year - TM_YEAR_BASE;
		alarm_set_monthly.tm_wday = get_day_of_week(time_set.tm_mday, time_set.tm_mon, time_set.tm_year);

		/* Format set alarm monthly using AIROC Connect
		 *
		 * "Monthly Hour Minute Second Date Month Year#"
		 * example: Monthly 12 30 0 20 8 2024#
		 *
		 */

		rslt = cyhal_rtc_set_alarm(&rtc_obj, &alarm_set_monthly, alarm_active_montly);
		if (CY_RSLT_SUCCESS == rslt)
		{
			monthly_alarm_show.date = alarm_set_monthly.tm_mday;
			monthly_alarm_show.hour = alarm_set_monthly.tm_hour;
			monthly_alarm_show.minute = alarm_set_monthly.tm_min;

			monthly = false;
#ifdef UNUSE_I2S
			printf("\rAlarm Monthly set\r\n\n");
#endif
		}
		else
		{
			handle_error();
		}
	}
}

void cyhal_rtc_alarm_interrupt_handler(void* arg, cyhal_rtc_event_t event)
{
	(void)arg;
	if (event == CYHAL_RTC_ALARM)
	{
		// ALARM HAS FIRED
#ifdef UNUSE_I2S
		printf("Play a song\r\n");
#endif

#ifdef USE_I2S
		audio_play = 1;
		audio_code = 2;
#endif
	}
}

/*******************************************************************************
 * Function Name: get_day_of_week
 ********************************************************************************
 * Summary:
 *  Returns a day of the week for a year, month, and day of month that are passed
 *  through parameters. Zeller's congruence is used to calculate the day of
 *  the week. See https://en.wikipedia.org/wiki/Zeller%27s_congruence for more
 *  details.
 *
 *  Note: In this algorithm January and February are counted as months 13 and 14
 *  of the previous year.
 *
 * Parameter:
 *  int day          : The day of the month, Valid range 1..31.
 *  int month        : The month of the year
 *  int year         : The year value. Valid range non-zero value.
 *
 * Return:
 *  Returns a day of the week (0 = Sunday, 1 = Monday, ., 6 = Saturday)
 *
 *******************************************************************************/
static int get_day_of_week(int day, int month, int year)
{
	int ret;
	int k = 0;
	int j = 0;
	if (month < CY_RTC_MARCH)
	{
		month += CY_RTC_MONTHS_PER_YEAR;
		year--;
	}

	k = (year % 100);
	j = (year / 100);
	ret=(day+(13*(month+1)/5)+k+(k/4)+(j/4)+(5*j))%DAYS_PER_WEEK;
	ret = ((ret + 6) % DAYS_PER_WEEK);
	return ret;
}

/*******************************************************************************
 * Function Name: get_week_of_month
 ********************************************************************************
 * Summary:
 *  Returns week number of the month for a year and a month that are passed
 *  through parameters.
 *
 * Parameter:
 *  int day          : The day of the month, Valid range 1..31.
 *  int month        : The month of the year
 *  int year         : The year value. Valid range non-zero value.
 *
 * Return:
 *  Returns a week of the month (0 to 5)
 *
 *******************************************************************************/
//static int get_week_of_month(int day, int month, int year)
//{
//	int count = 0, day_of_week = 0, weekend_day = 0;
//
//	day_of_week = get_day_of_week(1, month, year);
//	weekend_day = 7 - day_of_week;
//	while (day > weekend_day)
//	{
//		count++;
//		weekend_day += 7;
//	}
//
//	return count;
//}

/*******************************************************************************
 * Function Name: validate_date_time
 ********************************************************************************
 * Summary:
 *  This function validates date and time value.
 *
 * Parameters:
 *  uint32_t sec     : The second valid range is [0-59].
 *  uint32_t min     : The minute valid range is [0-59].
 *  uint32_t hour    : The hour valid range is [0-23].
 *  uint32_t date    : The date valid range is [1-31], if the month of February
 *                     is selected as the Month parameter, then the valid range
 *                     is [0-29].
 *  uint32_t month   : The month valid range is [1-12].
 *  uint32_t year    : The year valid range is [> 0].
 *
 * Return:
 *  false - invalid ; true - valid
 *
 *******************************************************************************/
static bool validate_date_time(int sec, int min, int hour, int mday,
		int month, int year)
{
	static const uint8_t days_in_month_table[MONTHS_PER_YEAR] =
	{
			DAYS_IN_JANUARY,
			DAYS_IN_FEBRUARY,
			DAYS_IN_MARCH,
			DAYS_IN_APRIL,
			DAYS_IN_MAY,
			DAYS_IN_JUNE,
			DAYS_IN_JULY,
			DAYS_IN_AUGUST,
			DAYS_IN_SEPTEMBER,
			DAYS_IN_OCTOBER,
			DAYS_IN_NOVEMBER,
			DAYS_IN_DECEMBER,
	};

	uint8_t days_in_month;

	bool rslt = IS_SEC_VALID(sec) & IS_MIN_VALID(min) &
			IS_HOUR_VALID(hour) & IS_MONTH_VALID(month) &
			IS_YEAR_VALID(year);

	if (rslt)
	{
		days_in_month = days_in_month_table[month - 1];

		if (IS_LEAP_YEAR(year) && (month == 2))
		{
			days_in_month++;
		}

		rslt &= (mday > 0U) && (mday <= days_in_month);
	}

	return rslt;
}
