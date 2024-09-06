#include "ble_notify_task.h"

#include "main.h"
#include "rtc_sc.h"
#include "main_ble.h"
#include "task_sensor.h"

char buffer_notify[STRING_BUFFER_SIZE];

void notify_task(){
	//wait for systems to be ready to ensure all systems start up correctly
	while(!systemReady)
	{
		vTaskDelay(pdMS_TO_TICKS(5));
	}

	while(1)
	{

		if(true == environment_notify)	//Display environment data to BLE scanning application on smart phone
		{
			memset(buffer_notify, '\0', sizeof(buffer_notify));
			sprintf(buffer_notify, "P: %0.2f, T: %0.0f, H: %0.0f", bme680_sensor.pressure, bme680_sensor.temperature, bme680_sensor.humidity);
		}
		else if(true == air_quality_notify)	//Display air quality data to BLE scanning application on smart phone
		{
			memset(buffer_notify, '\0', sizeof(buffer_notify));
			sprintf(buffer_notify, "Gas: %0.2f",bme680_sensor.gas);
		}
		else if(true == daily_notify_BLE)	//Displays daily alarm data to BLE scanning application on smart phone
		{
			memset(buffer_notify, '\0', sizeof(buffer_notify));
			sprintf(buffer_notify,"Daily alarm at %d:%d",daily_alarm_show.hour, daily_alarm_show.minute);
		}
		else if(true == monthly_notify_BLE)	//Displays daily alarm data to BLE scanning application on smart phone
		{
			memset(buffer_notify, '\0', sizeof(buffer_notify));
			sprintf(buffer_notify,"Monthly alarm on the %dth at %d:%d",monthly_alarm_show.date, monthly_alarm_show.hour, monthly_alarm_show.minute);
		}

		//Send Notify to AIROC
		if(connection_id != 0) //checking device is connected
		{
			if(app_psoc_command_client_char_config[0] & GATT_CLIENT_CONFIG_NOTIFICATION)
			{
				wiced_bt_gatt_server_send_notification( connection_id, HDLC_PSOC_COMMAND_VALUE, strlen(buffer_notify), (uint8_t *)buffer_notify, NULL);
				memset(buffer_notify, '\0', sizeof(buffer_notify));
			}
		}
		vTaskDelay(pdMS_TO_TICKS(100));
	}
}
