#include "write_rtc_task.h"

#include "main_ble.h"
#include "rtc_sc.h"
#include "task_sensor.h"

char buffer_notify[STRING_BUFFER_SIZE];

void notify_task(){

	while(1)
	{

		if(true == environment_notify)
		{
			memset(buffer_notify, '\0', sizeof(buffer_notify));
			sprintf(buffer_notify, "P: %0.2f, T: %0.0f, H: %0.0f", bme680_sensor.pressure, bme680_sensor.temperature, bme680_sensor.humidity);
		}
		else if(true == air_quality_notify)
		{
			memset(buffer_notify, '\0', sizeof(buffer_notify));
			sprintf(buffer_notify, "Gas: %0.2f",bme680_sensor.gas);
		}
		else if(true == daily_notify_BLE)
		{
			memset(buffer_notify, '\0', sizeof(buffer_notify));
			sprintf(buffer_notify,"Daily alarm at %d:%d",daily_alarm_show.hour, daily_alarm_show.minute);
		}
		else if(true == monthly_notify_BLE)
		{
			memset(buffer_notify, '\0', sizeof(buffer_notify));
			sprintf(buffer_notify,"Monthly alarm on the %dth at %d:%d",monthly_alarm_show.date, monthly_alarm_show.hour, monthly_alarm_show.minute);
		}

		//Send Notify to AIROC
		if(connection_id != 0)
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
