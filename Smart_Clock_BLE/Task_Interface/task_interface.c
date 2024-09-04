#include "task_interface.h"
#include "menu.h"
#include "main_page.h"
#include "main_ble.h"
#include "timeDatePage.h"
#include "rtc_sc.h"

#define ENABLE (1)

#define infineon_logo_width 128
#define infineon_logo_height 64

static unsigned char infineon_logo_bits[] = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xfd, 0x57,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x78, 0xfe, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xfe, 0x57, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xfe, 0x04, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
		0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xfc, 0x7d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xb7, 0x00, 0x00, 0x80, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0, 0x2e,
		0x00, 0x00, 0xf0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0xf0, 0x0b, 0x38, 0x00, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb8, 0x03, 0x38, 0x00, 0x70, 0x1c,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x6e, 0x00,
		0x38, 0x00, 0x70, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x7d, 0x00, 0x38, 0x00, 0x70, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00, 0x38, 0x6a, 0x78, 0x0c,
		0x35, 0x70, 0xc0, 0x42, 0x0d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x00,
		0x38, 0xfe, 0xf9, 0x1d, 0x7f, 0xf8, 0xf1, 0xc7, 0x1f, 0x00, 0x00, 0x00,
		0x00, 0xc0, 0x0d, 0x00, 0x38, 0xfe, 0x78, 0x1c, 0xf7, 0xdc, 0x71, 0xc7,
		0x3b, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1d, 0x00, 0x38, 0xce, 0x71, 0x1c,
		0xe7, 0xdc, 0x79, 0xce, 0x39, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x17, 0x00,
		0x38, 0xce, 0x71, 0x1c, 0x77, 0xfc, 0x33, 0xcf, 0x39, 0x00, 0x00, 0x00,
		0x00, 0x80, 0x1e, 0x00, 0x38, 0xce, 0x71, 0x1c, 0xe7, 0xfc, 0x79, 0xce,
		0x39, 0x00, 0x00, 0x00, 0x00, 0x80, 0x1b, 0x00, 0x38, 0xce, 0x71, 0x3c,
		0xf7, 0x1c, 0x31, 0xcf, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x80, 0x7e, 0x00,
		0x38, 0xce, 0x71, 0x1c, 0xe7, 0xdc, 0x73, 0xcf, 0x39, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x5b, 0x00, 0x38, 0xce, 0x71, 0x9c, 0x77, 0xfc, 0xf1, 0xc7,
		0x39, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfa, 0x00, 0x38, 0xce, 0x31, 0x1c,
		0xe3, 0xf0, 0xc0, 0xc3, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0xdc, 0x03,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x70, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0xd0, 0x5d, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x80, 0xfb,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x00, 0x00,
		0x00, 0x00, 0x00, 0xde, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0xe0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0, 0xff, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x5e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80,
		0xde, 0x17, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfa, 0x0b, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0xb8, 0xff, 0x97, 0x00, 0x00, 0x00, 0xf5, 0xff,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0xfd, 0xff, 0xff,
		0xff, 0xff, 0x7f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x40, 0xdd, 0xff, 0xb6, 0x7f, 0x4b, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0xa4, 0x6d, 0x84, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00 };

static bool RTC_ENABLE = false;

u8g2_t u8g2_obj;

void displayOled()
{
	while (!systemReady)
	{
		vTaskDelay(5);
	}

	u8g2_SetBitmapMode(&u8g2_obj, 0);
	u8g2_DrawXBM(&u8g2_obj, 0, 0, infineon_logo_width, infineon_logo_height, infineon_logo_bits);
	u8g2_SendBuffer(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);
	vTaskDelay(2000);

#ifdef UNUSE_I2S
	printf("Ready\r\n");
#endif
	RTC_ENABLE = cyhal_rtc_is_enabled(&rtc_obj);
	if(RTC_ENABLE != ENABLE)
	{
#ifdef UNUSE_I2S
		printf("Set RTC First\r\n");
#endif
		if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
			wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
		rtc_set_first();
	}

#ifdef UNUSE_I2S
	printf("Stop advtsm\r\n");
#endif

	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );

	while(1)
	{
		main_page();
		vTaskDelay(10);
	}
}

void init_u8g2()
{

	mtb_ssd1306_init_i2c(&i2c);

	/* Initialize the U8 Display */
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2_obj, U8G2_R0, u8x8_byte_hw_i2c, u8x8_gpio_and_delay_cb);

	/* Send init sequence to the display, display is in sleep mode after this */
	u8g2_InitDisplay(&u8g2_obj);

	/* Wake up display */
	u8g2_SetPowerSave(&u8g2_obj, 0);

	/* Prepare display for printing */
	u8g2_SetFont(&u8g2_obj, u8g2_font_6x12_tf);
	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);
}

void send_buffer_u8g2(){
	if (semphr_i2c_dev != NULL)
	{
		if(xSemaphoreTake(semphr_i2c_dev, ( TickType_t ) 10))
		{
			u8g2_SendBuffer(&u8g2_obj);
			u8g2_ClearBuffer(&u8g2_obj);
			xSemaphoreGive(semphr_i2c_dev);
		}
	}
}
