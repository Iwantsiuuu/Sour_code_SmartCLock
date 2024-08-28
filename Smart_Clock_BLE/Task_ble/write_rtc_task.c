#include "write_rtc_task.h"

#include "main_ble.h"
#include "task_sensor.h"

char buffer_notify[STRING_BUFFER_SIZE];

void notify_task(){

	cy_rslt_t rslt;
	struct tm date_time;

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
