#include "airQualityPage.h"

#include "stdio.h"
#include "string.h"
#include "main_ble.h"
#include "menuDisp.h"
#include "main_page.h"
#include "voice_command.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
#define AQ_PAGE 1
#define TIMEOUT  (1)

static void time_out();

static bool timeout_flag = false;

static uint8_t minute_timeout = 0;
static uint8_t second_timeout = 0;
/*******************************************************************************
 * Prototipe function
 *******************************************************************************/
static void prev_Cb();
static void airQuality_draw();
static void airQuality_getVal();
static void start_advertisement();
static void init_airQuality_disp();
static void deinit_airQuality_disp();

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
#ifdef USE_BMP280
static char data_buf_pressure[20];
static char data_buf_temperatur[20];
#endif

#ifdef USE_DPS310
static char data_buf_pressure[20];
static char data_buf_temperatur[20];
#endif

#ifdef USE_BME680
static char data_buf_gas[20];
#endif

#ifdef USE_DUMMY_DATA
static char data_buf_gas[20];
#endif

static uint8_t THIS_PAGE = 0;
static uint8_t idx_back = AQ_PAGE+1;
static uint8_t return_to_main_page = AQ_PAGE + 2;

void airQuality_disp()
{
	init_airQuality_disp();

	while (1)
	{
		time_out();
#ifdef UNUSE_I2S
		printf("RTC_M: %d, RTC_S: %d\r\nMENIT: %d, DETIK: %d\r\n", RTC_TIME.tm_min, RTC_TIME.tm_sec, minute_timeout, second_timeout);
#endif
		//Return to main page by voice command
		if(p_command_id == KEMBALI_CMD)
		{
			THIS_PAGE = return_to_main_page;
		}

		if (THIS_PAGE == AQ_PAGE)
		{
			airQuality_draw();
		}
		else if(THIS_PAGE == idx_back)
		{
			deinit_airQuality_disp();
			menu_page();
			break;
		}
		else
		{
			deinit_airQuality_disp();
			main_page();
			break;
		}
		vTaskDelay(20);
	}
	deinit_airQuality_disp();
}

//Register the callback function on each button, clear buffer and LCD display, and resume voice command task
static void init_airQuality_disp()
{
	button.attachPressed(&btn_obj[BUTTON_BACK],prev_Cb);			//Back to menu page by pressed back button
	button.attachHeld(&btn_obj[BUTTON_BACK],start_advertisement);	//Start advertisement when the device is not advertising by holding the back button for 1 second

	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);

	timeout_flag = true;

	THIS_PAGE = AQ_PAGE;

	vTaskResume(voiceHandle);
}

//Clearing callback function each button and LCD display
static void deinit_airQuality_disp()
{
	button.dettachPressed(&btn_obj[BUTTON_BACK]);//BACK
	button.dettachHeld(&btn_obj[BUTTON_BACK]);
	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);
}

static void airQuality_getVal()
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
	sprintf(data_buf_gas,"Gas :%0.2f\t hPa",bme680_sensor.gas);
#endif

#ifdef USE_DUMMY_DATA
	sprintf(data_buf_gas,"Gas :%0.2f\t hPa",bme680_sensor.gas);
#endif

}

//Display environmental data from the BME680 readout sensor
static void airQuality_draw()
{
	airQuality_getVal();
#ifdef USE_DPS310
	u8g2_DrawStr(&u8g2_obj, 0, 10, "Environment field");
	u8g2_DrawStr(&u8g2_obj, 0, 30, data_buf_pressure);
	u8g2_DrawStr(&u8g2_obj, 0, 40, data_buf_temperatur);
#endif

#ifdef USE_BMP280
	u8g2_DrawStr(&u8g2_obj, 0, 10, "Environment field");
	u8g2_DrawStr(&u8g2_obj, 0, 30, data_buf_pressure);
	u8g2_DrawStr(&u8g2_obj, 0, 40, data_buf_temperatur);
#endif

#ifdef USE_BME680
	u8g2_DrawStr(&u8g2_obj, 0, 10, "Gas Data field");
	u8g2_DrawStr(&u8g2_obj, 0, 30, data_buf_gas);
#endif

#ifdef USE_DUMMY_DATA
	u8g2_DrawStr(&u8g2_obj, 0, 10, "Gas Data field");
	u8g2_DrawStr(&u8g2_obj, 0, 30, data_buf_gas);
#endif

	send_buffer_u8g2();
}

//Start advertisement by holding back button for 1 second
static void start_advertisement()
{
	timeout_flag = true;
	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
}

//Back to menu page
static void prev_Cb()
{
	THIS_PAGE = idx_back; //index_back
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
