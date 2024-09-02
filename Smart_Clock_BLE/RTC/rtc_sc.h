#pragma once

#ifndef RTC_SC_H_
#define RTC_SC_H_

/*******************************************************************************
 * Macros
 *******************************************************************************/
#define UART_TIMEOUT_MS (10u)      /* in milliseconds */
#define INPUT_TIMEOUT_MS (120000u) /* in milliseconds */

#define STRING_BUFFER_SIZE (80)

/* Available commands */
#define RTC_CMD_SET_DATE_TIME ('1')
#define RTC_CMD_CONFIG_DST ('2')

#define RTC_CMD_ENABLE_DST ('1')
#define RTC_CMD_DISABLE_DST ('2')
#define RTC_CMD_QUIT_CONFIG_DST ('3')

#define FIXED_DST_FORMAT ('1')
#define RELATIVE_DST_FORMAT ('2')

/* Macro used for checking validity of user input */
#define MIN_SPACE_KEY_COUNT (5)

/* Structure tm stores years since 1900 */
#define TM_YEAR_BASE (1900u)

/* Maximum value of seconds and minutes */
#define MAX_SEC_OR_MIN (60u)

/* Maximum value of hours definition */
#define MAX_HOURS_24H (23UL)

/* Month per year definition */
#define MONTHS_PER_YEAR (12U)

/* Days per week definition */
#define DAYS_PER_WEEK (7u)

/* Days in month */
#define DAYS_IN_JANUARY (31U)   /* Number of days in January */
#define DAYS_IN_FEBRUARY (28U)  /* Number of days in February */
#define DAYS_IN_MARCH (31U)     /* Number of days in March */
#define DAYS_IN_APRIL (30U)     /* Number of days in April */
#define DAYS_IN_MAY (31U)       /* Number of days in May */
#define DAYS_IN_JUNE (30U)      /* Number of days in June */
#define DAYS_IN_JULY (31U)      /* Number of days in July */
#define DAYS_IN_AUGUST (31U)    /* Number of days in August */
#define DAYS_IN_SEPTEMBER (30U) /* Number of days in September */
#define DAYS_IN_OCTOBER (31U)   /* Number of days in October */
#define DAYS_IN_NOVEMBER (30U)  /* Number of days in November */
#define DAYS_IN_DECEMBER (31U)  /* Number of days in December */

/* Flags to indicate the if the entered time is valid */
#define DST_DISABLED_FLAG (0)
#define DST_VALID_START_TIME_FLAG (1)
#define DST_VALID_END_TIME_FLAG (2)
#define DST_ENABLED_FLAG (3)

/* Macro to validate seconds parameter */
#define IS_SEC_VALID(sec) ((sec) <= MAX_SEC_OR_MIN)

/* Macro to validate minutes parameters */
#define IS_MIN_VALID(min) ((min) <= MAX_SEC_OR_MIN)

/* Macro to validate hour parameter */
#define IS_HOUR_VALID(hour) ((hour) <= MAX_HOURS_24H)

/* Macro to validate month parameter */
#define IS_MONTH_VALID(month) (((month) > 0U) && ((month) <= MONTHS_PER_YEAR))

/* Macro to validate the year value */
#define IS_YEAR_VALID(year) ((year) > 0U)

/* Checks whether the year passed through the parameter is leap or not */
#define IS_LEAP_YEAR(year) \
		(((0U == (year % 4UL)) && (0U != (year % 100UL))) || (0U == (year % 400UL)))

#define RTC_CALLBACK_ARG (NULL)
#define RTC_INTERRUPT_PRIORITY (3u)

#define ALARM_MONTHLY 	(1)
#define ALARM_DAILY		(2)
/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/

void set_new_time();
void set_alarm();
void cyhal_rtc_alarm_interrupt_handler(void* arg, cyhal_rtc_event_t event);

typedef struct{
	uint8_t mday,
	month,
	sec,
	min,
	hour;
	uint16_t Year;
}RTC_Data_Setup_t;

typedef struct data_alarm
{
	uint8_t hour;
	uint8_t minute;
	uint8_t date;
}alarm_t;

extern cyhal_rtc_t rtc_obj;
extern RTC_Data_Setup_t RTC_Setup;
extern RTC_Data_Setup_t daily_alarm;
extern RTC_Data_Setup_t montly_alarm;

extern alarm_t daily_alarm_show;
extern alarm_t monthly_alarm_show;

#endif
