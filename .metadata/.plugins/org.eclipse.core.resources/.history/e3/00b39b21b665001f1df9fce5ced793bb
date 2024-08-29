#include "stopWatchPage.h"
#include "menuDisp.h"
#include "main_ble.h"
#include "voice_command.h"

#define SW_PAGE 1

/*--------------< Prototype function >---------------*/
static void BackSW_Cb();
static void stopwatch();
static void startSW_Cb();
static void pauseSW_Cb();
static void resetSW_Cb();
static void stopWatch_draw();
static void start_advertisement();
static void init_stopWatch_disp();
static void deinit_stopWatch_disp();
//static void ble_command(uint8_t ble_cmd);
static void speech_stopwatch_cmd(uint32_t cmd);

/*--------------< Global function >----------------*/
static bool countSW = false;
static char buf_stopWatch[100];
static uint8_t THIS_PAGE = 0;
static uint8_t idx_back = SW_PAGE+1;
static uint16_t hour = 0;
static uint16_t minute = 0;
static uint16_t second = 0;
static uint16_t mili_second = 0;
static uint16_t over;
static uint32_t firstT, time_now, dtStopWatch;

void init_stopWatch_disp()
{
	button.attachPressed(&btn_obj[BUTTON_ENTER],startSW_Cb);
	button.attachPressed(&btn_obj[BUTTON_DOWN],pauseSW_Cb);
	button.attachPressed(&btn_obj[BUTTON_UP],resetSW_Cb);
	button.attachPressed(&btn_obj[BUTTON_BACK],BackSW_Cb);

	button.attachHeld(&btn_obj[BUTTON_UP], start_advertisement);

	u8g2_ClearDisplay(&u8g2_obj);
	u8g2_ClearBuffer(&u8g2_obj);

	THIS_PAGE = SW_PAGE;
	p_command_id = 0;
//	BLE_COMMAND = IDLE_BLE;
}

void deinit_stopWatch_disp()
{

	//	Melakukan deattach button
	for (uint8_t i = 0; i < 4; i++)
		button.dettachPressed(&btn_obj[i]);
	button.dettachHeld(&btn_obj[BUTTON_UP]);
}

void stopwatch()
{
	if (countSW)
	{
		time_now = xTaskGetTickCount();
		dtStopWatch = time_now - firstT;

		hour 		= dtStopWatch / 3600000;
		over 		= dtStopWatch % 3600000;
		minute 		= over / 60000;
		over 		= over % 60000;
		second 		= over / 1000;
		mili_second = over % 1000;
	}
	else if (countSW == false && dtStopWatch == 0)
	{
		hour = 0, minute = 0, second = 0, mili_second = 0;
	}
	sprintf(buf_stopWatch,"%d:%d:%d:%d\r\n",hour,minute,second,mili_second);
}

void stopWatch_draw()
{
	stopwatch();
	u8g2_DrawStr(&u8g2_obj, 0, 15, "StopWatch");
	u8g2_DrawStr(&u8g2_obj, 0, 25, buf_stopWatch);
	send_buffer_u8g2();
}

void stopWatch_disp()
{
	init_stopWatch_disp();

	while (1)
	{
//		ble_command(BLE_COMMAND);
		speech_stopwatch_cmd(p_command_id);

		if (THIS_PAGE == SW_PAGE)
		{
			stopWatch_draw();
		}
		else
		{
			deinit_stopWatch_disp();
			menu_disp_oled();
		}
		vTaskDelay(20);
	}
	deinit_stopWatch_disp();
}

static void start_advertisement()
{
	if(connection_id == 0 && advertisement_mode != BTM_BLE_ADVERT_UNDIRECTED_HIGH)
		wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );
}

static void startSW_Cb()
{
	firstT = xTaskGetTickCount() - dtStopWatch;
	countSW = true;
}

static void pauseSW_Cb()
{
	countSW = false;
}

static void resetSW_Cb()
{
	countSW = false;
	dtStopWatch = 0;
}

static void BackSW_Cb()
{
	THIS_PAGE = idx_back; //index_back
}

static void speech_stopwatch_cmd(uint32_t cmd)
{
	switch(cmd)
	{
	case START_CMD:
		firstT = xTaskGetTickCount() - dtStopWatch;
		countSW = true;
		break;

	case STOP_CMD:
		countSW = false;
		break;

	case RESET_CMD:
		countSW = false;
		dtStopWatch = 0;
		break;

	case BACK_CMD:
		THIS_PAGE = idx_back; //index_back
		break;
	default:
		break;
	}
}

//static void ble_command(uint8_t ble_cmd)
//{
//	switch(ble_cmd)
//	{
//	case START_BLE:
//		firstT = xTaskGetTickCount() - dtStopWatch;
//		countSW = true;
//		break;
//	case STOP_BLE:
//		countSW = false;
//		break;
//	case RESET_BLE:
//		countSW = false;
//		dtStopWatch = 0;
//		break;
//	case BACK_BLE:
//		THIS_PAGE = idx_back; //index_back
//		break;
//	default:
//		break;
//	}
//}
