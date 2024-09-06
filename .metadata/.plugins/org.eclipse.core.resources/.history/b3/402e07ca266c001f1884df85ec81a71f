#include "EnvironmentPage.h"

#include "main_page.h"
#include "menuDisp.h"
#include "main_ble.h"
#include "voice_command.h"

/*Include FreeRTOS*/
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "task.h"
#include "semphr.h"
#include <timers.h>

#define ENVI_PAGE 1

#define TIMEOUT  (1)
static void time_out();

static bool timeout_flag = false;

static uint8_t minute_timeout = 0;
static uint8_t second_timeout = 0;

/* Prototype function */
static void prev_Cb();
static void environment_draw();
static void environment_getVal();
static void start_advertisement();

/* Global Variable */
static char data_buf_pressure[20], data_buf_temperatur[20], data_buf_humidity[20];
static uint8_t THIS_PAGE = 0;
static uint8_t idx_back = ENVI_PAGE + 1;
static uint8_t return_to_main_page = ENVI_PAGE + 2;

void init_environment_disp()
{
	button.attachPressed(&btn_obj[BUTTON_BACK],prev_Cb);
	button.attachHeld(&btn_obj[BUTTON_BACK],start_advertisement);

	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);

	THIS_PAGE = ENVI_PAGE;
	timeout_flag = true;

	vTaskResume(voiceHandle);
}

void deinit_environment_disp()
{
	button.dettachPressed(&btn_obj[BUTTON_BACK]);//BACK
	button.dettachHeld(&btn_obj[BUTTON_BACK]);
	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);
}

static void environment_getVal()
{
#ifdef USE_DPS310
	sprintf(data_buf_pressure  ,"Pres :%0.2f\t hPa",dps_sensor.pressure);
	sprintf(data_buf_temperatur,"Temp :%0.2f\t C\xB0",dps_sensor.temperature);
#endif

#ifdef USE_BMP280
	sprintf(data_buf_pressure  ,"Pres :%0.2f\t hPa",bmp280_sensor.pressure);
	sprintf(data_buf_temperatur,"Temp :%0.2f\t C\xB0",bmp280_sensor.temperature);
#endif

#ifdef USE_BME680
	sprintf(data_buf_pressure  ,"Pres :%0.2f\t hPa",bme680_sensor.pressure);
	sprintf(data_buf_temperatur,"Temp :%0.2f\t C\xB0",bme680_sensor.temperature);
	sprintf(data_buf_humidity,"Humy :%0.2f\t C\xB0",bme680_sensor.humidity);
#endif

#ifdef USE_DUMMY_DATA
	sprintf(data_buf_pressure  ,"Pres :%0.2f\t hPa",bme680_sensor.pressure);
	sprintf(data_buf_temperatur,"Temp :%0.2f\t C\xB0",bme680_sensor.temperature);
	sprintf(data_buf_humidity,"Humy :%0.2f\t C\xB0",bme680_sensor.humidity);
#endif
}

static void environment_draw()
{
	environment_getVal();
	u8g2_DrawStr(&u8g2_obj, 0, 10, "Environment Data");

#ifdef USE_DUMMY_DATA
	u8g2_DrawStr(&u8g2_obj, 0, 30, data_buf_pressure);
	u8g2_DrawStr(&u8g2_obj, 0, 40, data_buf_temperatur);
	u8g2_DrawStr(&u8g2_obj, 0, 50, data_buf_humidity);
#endif

#ifdef USE_DPS310
	u8g2_DrawStr(&u8g2_obj, 0, 30, data_buf_pressure);
	u8g2_DrawStr(&u8g2_obj, 0, 40, data_buf_temperatur);
#endif

#ifdef USE_BMP280
	u8g2_DrawStr(&u8g2_obj, 0, 30, data_buf_pressure);
	u8g2_DrawStr(&u8g2_obj, 0, 40, data_buf_temperatur);
#endif

#ifdef USE_BME680
	u8g2_DrawStr(&u8g2_obj, 0, 30, data_buf_pressure);
	u8g2_DrawStr(&u8g2_obj, 0, 40, data_buf_temperatur);
	u8g2_DrawStr(&u8g2_obj, 0, 50, data_buf_humidity);
#endif
	send_buffer_u8g2();
}

void environment_disp()
{
	init_environment_disp();

	while (1)
	{
		time_out();
#ifdef UNUSE_I2S
		printf("RTC_M: %d, RTC_S: %d\r\nMENIT: %d, DETIK: %d\r\n", RTC_TIME.tm_min, RTC_TIME.tm_sec, minute_timeout, second_timeout);
#endif
		if(p_command_id == KEMBALI_CMD)
		{
			THIS_PAGE = return_to_main_page;
		}

		if (THIS_PAGE == ENVI_PAGE)
		{
			environment_draw();
		}
		else if(THIS_PAGE == idx_back)
		{
			deinit_environment_disp();
			menu_page();
			break;
		}
		else
		{
			deinit_environment_disp();
			main_page();
			break;
		}
		vTaskDelay(20);
	}
	deinit_environment_disp();
}

static void prev_Cb()
{
	THIS_PAGE = idx_back; //index_back
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

	if ((RTC_TIME.tm_min - minute_timeout > (TIMEOUT+1)) && (RTC_TIME.tm_sec - second_timeout == (TIMEOUT-1)))
	{
		THIS_PAGE = return_to_main_page;
	}
}
