/*
 * main_ble.c
 *
 * 	Created on: 13 June 2024
 * 	    Author: Ridwan Bahari
 */

#include "main_ble.h"

#include "setting_alarm.h"
#include "timeDatePage.h"
#include "write_rtc_task.h"

/*******************************************************************************
 * Prototipe function
 *******************************************************************************/
/* GATT Event Callback and Handler Functions */
static wiced_bt_gatt_status_t app_bt_gatt_event_callback            (wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_event_data);

static wiced_bt_gatt_status_t app_bt_connect_event_handler          (wiced_bt_gatt_connection_status_t *p_conn_status);
static wiced_bt_gatt_status_t app_bt_server_event_handler           (wiced_bt_gatt_event_data_t *p_data);

static wiced_bt_gatt_status_t app_bt_write_handler                  (wiced_bt_gatt_event_data_t *p_data);
static wiced_bt_gatt_status_t app_bt_gatt_req_read_handler          (uint16_t conn_id, wiced_bt_gatt_opcode_t opcode,
		wiced_bt_gatt_read_t *p_read_req, uint16_t len_requested);
static wiced_bt_gatt_status_t app_bt_gatt_req_read_multi_handler    (uint16_t conn_id, wiced_bt_gatt_opcode_t opcode,
		wiced_bt_gatt_read_multiple_req_t *p_read_req, uint16_t len_requested);
static wiced_bt_gatt_status_t app_bt_gatt_req_read_by_type_handler  (uint16_t conn_id, wiced_bt_gatt_opcode_t opcode,
		wiced_bt_gatt_read_by_type_t *p_read_req, uint16_t len_requested);

/* Helper functions to find GATT database handles and allocate/free buffers for GATT operations */
static gatt_db_lookup_table_t 	*app_bt_find_by_handle(uint16_t handle);
static uint8_t 					*app_bt_alloc_buffer(uint16_t len);
static void 					app_bt_free_buffer(uint8_t *p_data);

static void command_exec (char *txt);

static void daily_set(char *str);
static void monthly_set(char *str_);

static void show_daily_alarm(char *str_);
static void show_monthly_alarm(char *str_);

static void set_rtc_val_cb(char *str_);
static void set_val_alarm_daily_cb(char *str_);
static void set_val_alarm_monthly_cb(char *str_);

static void airquality_pages_cb (char *str_);
static void environment_pages_cb (char *str_);

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
volatile int uxTopUsedPriority;

cyhal_pwm_t PWM_obj;
TimerHandle_t timer_led_blink;

//bool flag_bt_actived = false;
bool set_rtc = false;
bool set_new_alarm = false;

bool daily_notify_BLE = false;
bool monthly_notify_BLE = false;
bool environment_notify = false;
bool air_quality_notify = false;

uint8_t BLE_COMMAND = 0;
uint8_t advertisement_mode = 0;
uint8_t event_data_ble;

uint16_t connection_id = 0;
uint16_t reason_dic = 0;

int set_daily_alarm_data[6];
int set_monthly_alarm_data[6];

char *tail = "#";
char *reason_disconnect = "";

cmd_key cmd_bank[10] = {
		{"Day_set", daily_set},
		{"Month_set", monthly_set},

		{"RTC_SET", set_rtc_val_cb},
		{"Daily", set_val_alarm_daily_cb},
		{"Monthly", set_val_alarm_monthly_cb},

		{"Show_daily", show_daily_alarm},
		{"Show_monthly", show_monthly_alarm},
		{"Air_Quality", airquality_pages_cb},
		{"Environment", environment_pages_cb},
};

/*******************************************************************************
 * Function Name: wiced_bt_dev_status_t app_bt_management_callback(
 * 					wiced_bt_management_evt_t event,
 * 					wiced_bt_management_evt_data_t *p_event_data )
 ********************************************************************************/
wiced_result_t app_bt_management_callback( wiced_bt_management_evt_t event, wiced_bt_management_evt_data_t *p_event_data )
{
	/* Start in error state so that any unimplemented states will return error */
	wiced_result_t result = WICED_BT_ERROR;
	wiced_bt_device_address_t bda = {0};
#ifdef UNUSE_I2S
	printf("Bluetooth Management Event: 0x%x %s\n\r", event, get_bt_event_name(event));
#endif
	switch( event )
	{
	case BTM_ENABLED_EVT:								// Bluetooth Controller and Host Stack Enabled
		if( WICED_BT_SUCCESS == p_event_data->enabled.status )
		{
#ifdef UNUSE_I2S
			printf( "Bluetooth Enabled\n\r" );
#endif
			/* Set the local BDA from the value in the configurator and print it */
			wiced_bt_set_local_bdaddr((uint8_t *)cy_bt_device_address, BLE_ADDR_PUBLIC);
			wiced_bt_dev_read_local_addr( bda );

#ifdef UNUSE_I2S
			printf( "Local Bluetooth Device Address: ");
			print_bd_address(bda);
#endif

			/* Set advertisement packet and begin advertising */
			wiced_bt_ble_set_raw_advertisement_data(CY_BT_ADV_PACKET_DATA_SIZE, cy_bt_adv_packet_data);

			/* Register GATT callback and initialize database*/
			wiced_bt_gatt_register( app_bt_gatt_event_callback );
			wiced_bt_gatt_db_init( gatt_database, gatt_database_len, NULL );

			/* Disable pairing */
			wiced_bt_set_pairable_mode( WICED_FALSE, WICED_FALSE );

			//			wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );

			result = WICED_BT_SUCCESS;
		}
		else
		{
#ifdef UNUSE_I2S
			printf( "Failed to initialize Bluetooth controller and stack\n\r" );
#endif
		}
		break;

	case BTM_PAIRING_IO_CAPABILITIES_BLE_REQUEST_EVT: 		// IO capabilities request
		result = WICED_BT_SUCCESS;
		break;

	case BTM_PAIRING_COMPLETE_EVT: 							// Pairing Complete event
		result = WICED_BT_SUCCESS;
		break;

	case BTM_ENCRYPTION_STATUS_EVT: 						// Encryption Status Event
		result = WICED_BT_SUCCESS;
		break;

	case BTM_SECURITY_REQUEST_EVT: 							// Security access
		result = WICED_BT_SUCCESS;
		break;

	case BTM_PAIRED_DEVICE_LINK_KEYS_UPDATE_EVT: 			// Save link keys with app
		result = WICED_BT_SUCCESS;
		break;

	case BTM_PAIRED_DEVICE_LINK_KEYS_REQUEST_EVT: 			// Retrieve saved link keys
		/* This must return WICED_BT_ERROR if bonding information is not stored in EEPROM */
		result = WICED_BT_ERROR;
		break;

	case BTM_LOCAL_IDENTITY_KEYS_UPDATE_EVT: 				// Save keys to NVRAM
		result = WICED_BT_SUCCESS;
		break;

	case  BTM_LOCAL_IDENTITY_KEYS_REQUEST_EVT: 				// Read keys from NVRAM
		/* This should return WICED_BT_SUCCESS if not using privacy. If RPA is enabled but keys are not
               stored in EEPROM, this must return WICED_BT_ERROR so that the stack will generate new privacy keys */
		result = WICED_BT_ERROR;
		break;

	case BTM_BLE_SCAN_STATE_CHANGED_EVT: 					// Scan State Change
		result = WICED_BT_SUCCESS;
		break;

	case BTM_BLE_ADVERT_STATE_CHANGED_EVT:					// Advertising State Change
		advertisement_mode = get_bt_advert_mode_name(p_event_data->ble_advert_state_changed);
#ifdef UNUSE_I2S
		printf("Advertisement State Change: %s\n\r", get_bt_advert_mode_name(p_event_data->ble_advert_state_changed));
#endif

		event_data_ble = p_event_data->ble_advert_state_changed;
		result = WICED_BT_SUCCESS;
		if ( event_data_ble == BTM_BLE_ADVERT_OFF)
		{
			if(connection_id == 0)
			{
				/*---> When Bluetooth is disconnect with phone LED turn off <---*/
				cyhal_pwm_set_duty_cycle(&PWM_obj, 100, 2);
			}
			else
			{ //connected
				/*---> When device connected to phone LED will be active/turn on <---*/
				cyhal_pwm_set_duty_cycle(&PWM_obj, 0, 2);
			}

#ifdef UNUSE_I2S
			printf("Disconnect Reason: '%s' [%d]\n\r", reason_disconnect, reason_dic);
#endif

#ifdef UNUSE_I2S
			printf("Connection ID disconnect %d\n", connection_id);
#endif
		}
		else
		{
			/*---> When device advertisement the LED will be blink fast <---*/
			cyhal_pwm_set_duty_cycle(&PWM_obj, 90, 10);
		}
		break;

	default:
		break;
	}

	return result;
}

/*******************************************************************************
 * Function Name: wiced_bt_gatt_status_t app_bt_gatt_event_callback(
 * 					wiced_bt_gatt_evt_t event,
 * 					wiced_bt_gatt_event_data_t *p_data )
 ********************************************************************************/
static wiced_bt_gatt_status_t app_bt_gatt_event_callback( wiced_bt_gatt_evt_t event, wiced_bt_gatt_event_data_t *p_event_data )
{
	/* Start in error state so that any unimplemented states will return error */
	wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;

	/* Call the appropriate callback function based on the GATT event type, and pass the relevant event
	 * parameters to the callback function */
	switch (event)
	{
	case GATT_CONNECTION_STATUS_EVT:
		status = app_bt_connect_event_handler(&p_event_data->connection_status);
		break;

	case GATT_ATTRIBUTE_REQUEST_EVT:
		status = app_bt_server_event_handler(p_event_data);
		break;

	case GATT_GET_RESPONSE_BUFFER_EVT: /* GATT buffer request, typically sized to max of bearer mtu - 1 */
		p_event_data->buffer_request.buffer.p_app_rsp_buffer = app_bt_alloc_buffer(p_event_data->buffer_request.len_requested);
		p_event_data->buffer_request.buffer.p_app_ctxt = (void *)app_bt_free_buffer;
		status = WICED_BT_GATT_SUCCESS;
		break;

	case GATT_APP_BUFFER_TRANSMITTED_EVT: /* GATT buffer transmitted event,  check \ref wiced_bt_gatt_buffer_transmitted_t*/
	{
		pfn_free_buffer_t pfn_free = (pfn_free_buffer_t)p_event_data->buffer_xmitted.p_app_ctxt;

		/* If the buffer is dynamic, the context will point to a function to free it. */
		if (pfn_free)
		{
			pfn_free(p_event_data->buffer_xmitted.p_app_data);
		}
		status = WICED_BT_GATT_SUCCESS;
	}
	break;

	default:
#ifdef UNUSE_I2S
		printf( "Unhandled GATT Event: 0x%x (%d)\n\r", event, event );
#endif
		status = WICED_BT_GATT_SUCCESS;
		break;
	}
	return status;
}

/*******************************************************************************
 * Function Name: app_bt_connect_event_handler
 *
 * Handles GATT connection status changes.
 *
 * Param:	p_conn_status  Pointer to data that has connection details
 * Return:	wiced_bt_gatt_status_t
 * See possible status codes in wiced_bt_gatt_status_e in wiced_bt_gatt.h
 *******************************************************************************/
static wiced_bt_gatt_status_t app_bt_connect_event_handler(wiced_bt_gatt_connection_status_t *p_conn_status)
{
	wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;

	if (NULL != p_conn_status)
	{
		if (p_conn_status->connected)
		{
#ifdef UNUSE_I2S
			printf("GATT_CONNECTION_STATUS_EVT: Connect BDA ");
			print_bd_address(p_conn_status->bd_addr);
			printf("Connection ID %d\n", p_conn_status->conn_id );
#endif
			connection_id = p_conn_status->conn_id;
			/* Handle the connection */
			//			flag_bt_actived = false;
		}
		else
		{

#ifdef UNUSE_I2S
			printf("Disconnected bt_con_H: BDA " );
			print_bd_address(p_conn_status->bd_addr);
#endif
			reason_disconnect = get_bt_gatt_disconn_reason_name(p_conn_status->reason);
			reason_dic = p_conn_status->reason;

#ifdef UNUSE_I2S
			printf("Connection ID '%d', Reason '%s' %d\n\r", p_conn_status->conn_id, reason_disconnect, reason_dic);
#endif
			if(reason_dic == GATT_CONN_TERMINATE_PEER_USER || reason_dic == GATT_CONN_TIMEOUT)
				wiced_bt_start_advertisements( BTM_BLE_ADVERT_UNDIRECTED_HIGH, 0, NULL );

			cyhal_pwm_set_duty_cycle(&PWM_obj, 100, 2);
			connection_id = 0;
			set_rtc = false;
			set_new_alarm = false;
		}

		status = WICED_BT_GATT_SUCCESS;
	}

	return status;
}

/*******************************************************************************
 * Function Name: app_bt_server_event_handler
 *
 * Invoked when GATT_ATTRIBUTE_REQUEST_EVT occurs in GATT Event callback.
 *
 * Param:	p_data   				Pointer to BLE GATT request data
 * Return:	wiced_bt_gatt_status_t  BLE GATT status
 *******************************************************************************/
static wiced_bt_gatt_status_t app_bt_server_event_handler(wiced_bt_gatt_event_data_t *p_data)
{
	wiced_bt_gatt_status_t status = WICED_BT_GATT_ERROR;
	wiced_bt_gatt_attribute_request_t   *p_att_req = &p_data->attribute_request;

	switch (p_att_req->opcode)
	{

	case GATT_REQ_READ: /* Attribute read notification (attribute value internally read from GATT database) */
	case GATT_REQ_READ_BLOB:
		status = app_bt_gatt_req_read_handler(p_att_req->conn_id, p_att_req->opcode,
				&p_att_req->data.read_req, p_att_req->len_requested);
		break;

	case GATT_REQ_READ_BY_TYPE:
		status = app_bt_gatt_req_read_by_type_handler(p_att_req->conn_id, p_att_req->opcode,
				&p_att_req->data.read_by_type, p_att_req->len_requested);
		break;

	case GATT_REQ_READ_MULTI:
	case GATT_REQ_READ_MULTI_VAR_LENGTH:
		status = app_bt_gatt_req_read_multi_handler(p_att_req->conn_id, p_att_req->opcode,
				&p_att_req->data.read_multiple_req, p_att_req->len_requested);
		break;

	case GATT_REQ_WRITE:
	case GATT_CMD_WRITE:
	case GATT_CMD_SIGNED_WRITE:
		status = app_bt_write_handler(p_data);
		if ((p_att_req->opcode == GATT_REQ_WRITE) && (status == WICED_BT_GATT_SUCCESS))
		{
			wiced_bt_gatt_write_req_t *p_write_request = &p_att_req->data.write_req;
			wiced_bt_gatt_server_send_write_rsp(p_att_req->conn_id, p_att_req->opcode, p_write_request->handle);
		}
		break;

	case GATT_REQ_PREPARE_WRITE:
		status = WICED_BT_GATT_SUCCESS;
		break;

	case GATT_REQ_EXECUTE_WRITE:
		wiced_bt_gatt_server_send_execute_write_rsp(p_att_req->conn_id, p_att_req->opcode);
		status = WICED_BT_GATT_SUCCESS;
		break;

	case GATT_REQ_MTU:
		/* Application calls wiced_bt_gatt_server_send_mtu_rsp() with the desired mtu */
		status = wiced_bt_gatt_server_send_mtu_rsp(p_att_req->conn_id,
				p_att_req->data.remote_mtu,
				wiced_bt_cfg_settings.p_ble_cfg->ble_max_rx_pdu_size);
		break;

	case GATT_HANDLE_VALUE_CONF: /* Value confirmation */
		break;

	case GATT_HANDLE_VALUE_NOTIF:
		break;

	default:
#ifdef UNUSE_I2S
		printf( "Unhandled GATT Server Event: 0x%x (%d)\n\r", p_att_req->opcode, p_att_req->opcode );
#endif
		break;
	}

	return status;
}

/*******************************************************************************
 * Function Name: app_bt_write_handler
 *
 * Invoked when GATTS_REQ_TYPE_WRITE is received from the
 * client device. Handles "Write Requests" received from Client device.
 *
 * Param:	p_write_req   			Pointer to BLE GATT write request
 * Return:	wiced_bt_gatt_status_t  BLE GATT status
 *******************************************************************************/
static wiced_bt_gatt_status_t app_bt_write_handler(wiced_bt_gatt_event_data_t *p_data)
{
	wiced_bt_gatt_write_req_t *p_write_req = &p_data->attribute_request.data.write_req;;

	wiced_bt_gatt_status_t status = WICED_BT_GATT_INVALID_HANDLE;

	for (int i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
	{

		/* Check for a matching handle entry */
		if (app_gatt_db_ext_attr_tbl[i].handle == p_write_req->handle)
		{
			//			printf("len value %d \r\n", p_write_req->val_len);
			for (int a = 0; a < p_write_req->val_len; a++){
				//				printf("Pval %d \r\n",p_write_req->p_val[a]);
			}
			//             memset(app_gatt_db_ext_attr_tbl[i].p_write_req->p_val, 0x00, app_gatt_db_ext_attr_tbl[i].max_len);

			/* Detected a matching handle in the external lookup table */
			if (app_gatt_db_ext_attr_tbl[i].max_len >= p_write_req->val_len)
			{
				/* Value fits within the supplied buffer; copy over the value */
				app_gatt_db_ext_attr_tbl[i].cur_len = p_write_req->val_len;
				memset(app_gatt_db_ext_attr_tbl[i].p_data, 0x00, app_gatt_db_ext_attr_tbl[i].max_len);
				memcpy(app_gatt_db_ext_attr_tbl[i].p_data, p_write_req->p_val, app_gatt_db_ext_attr_tbl[i].cur_len);

				if (memcmp(app_gatt_db_ext_attr_tbl[i].p_data, p_write_req->p_val, app_gatt_db_ext_attr_tbl[i].cur_len) == 0)
				{
					status = WICED_BT_GATT_SUCCESS;
				}

				switch ( p_write_req->handle )
				{
				// Add action when specified handle is written
				case HDLC_PSOC_COMMAND_VALUE:
					command_exec ((char *)app_psoc_command);
				}
			}
			else
			{
				/* Value to write will not fit within the table */
				status = WICED_BT_GATT_INVALID_ATTR_LEN;
#ifdef UNUSE_I2S
				printf("Invalid attribute length during GATT write\n");
#endif
			}
			break;
		}
	}
	if (WICED_BT_GATT_SUCCESS != status)
	{
#ifdef UNUSE_I2S
		printf("GATT write failed: %d\n", status);
#endif
	}

	return status;
}

/*******************************************************************************
 * Function Name: app_bt_gatt_req_read_handler
 *
 * This Function handles GATT read and read blob events
 *
 * Params: 	conn_id       			Connection ID
 * 			opcode        			BLE GATT request type opcode
 * 			p_read_req    			Pointer to read request containing the handle to read
 * 			len_requested 			Length of data requested
 * Return: 	wiced_bt_gatt_status_t  BLE GATT status
 *******************************************************************************/
static wiced_bt_gatt_status_t app_bt_gatt_req_read_handler(uint16_t conn_id, wiced_bt_gatt_opcode_t opcode,
		wiced_bt_gatt_read_t *p_read_req, uint16_t len_requested)
{
	gatt_db_lookup_table_t *puAttribute;
	uint16_t attr_len_to_copy, to_send;
	uint8_t *from;

	if ((puAttribute = app_bt_find_by_handle(p_read_req->handle)) == NULL)
	{
		wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_req->handle, WICED_BT_GATT_INVALID_HANDLE);
		return WICED_BT_GATT_INVALID_HANDLE;
	}

	attr_len_to_copy = puAttribute->cur_len;

	if (p_read_req->offset >= puAttribute->cur_len)
	{
		wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_req->handle, WICED_BT_GATT_INVALID_OFFSET);
		return WICED_BT_GATT_INVALID_OFFSET;
	}

	switch ( p_read_req->handle )
	{
	// Add action when specified handle is read
	case HDLC_PSOC_COMMAND_VALUE:
#ifdef UNUSE_I2S
		printf( "LED is %s\r\n", app_psoc_command[0] ? "ON" : "OFF" );
#endif
		break;
	}

	to_send = MIN(len_requested, attr_len_to_copy - p_read_req->offset);
	from = puAttribute->p_data + p_read_req->offset;
	return wiced_bt_gatt_server_send_read_handle_rsp(conn_id, opcode, to_send, from, NULL); /* No need for context, as buff not allocated */
}

/*******************************************************************************
 * Function Name: app_bt_gatt_req_read_by_type_handler
 *
 * Process read-by-type request from peer device
 *
 * Params:	conn_id       			Connection ID
 * 			opcode        			BLE GATT request type opcode
 * 			p_read_req    			Pointer to read request containing the handle to read
 * 			len_requested 			Length of data requested
 * Return:	wiced_bt_gatt_status_t	BLE GATT status
 *******************************************************************************/
static wiced_bt_gatt_status_t app_bt_gatt_req_read_by_type_handler(uint16_t conn_id, wiced_bt_gatt_opcode_t opcode,
		wiced_bt_gatt_read_by_type_t *p_read_req, uint16_t len_requested)
{
	gatt_db_lookup_table_t *puAttribute;
	uint16_t attr_handle = p_read_req->s_handle;
	uint8_t *p_rsp = app_bt_alloc_buffer(len_requested);
	uint8_t pair_len = 0;
	int used = 0;

	if (p_rsp == NULL)
	{
		wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, attr_handle, WICED_BT_GATT_INSUF_RESOURCE);
		return WICED_BT_GATT_INSUF_RESOURCE;
	}

	/* Read by type returns all attributes of the specified type, between the start and end handles */
	while (WICED_TRUE)
	{
		attr_handle = wiced_bt_gatt_find_handle_by_type(attr_handle, p_read_req->e_handle, &p_read_req->uuid);

		if (attr_handle == 0)
			break;

		if ((puAttribute = app_bt_find_by_handle(attr_handle)) == NULL)
		{
			wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_req->s_handle, WICED_BT_GATT_ERR_UNLIKELY);
			app_bt_free_buffer(p_rsp);
			return WICED_BT_GATT_INVALID_HANDLE;
		}

		{
			int filled = wiced_bt_gatt_put_read_by_type_rsp_in_stream(p_rsp + used, len_requested - used, &pair_len,
					attr_handle, puAttribute->cur_len, puAttribute->p_data);
			if (filled == 0)
			{
				break;
			}
			used += filled;
		}

		/* Increment starting handle for next search to one past current */
		attr_handle++;
	}

	if (used == 0)
	{
		wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, p_read_req->s_handle, WICED_BT_GATT_INVALID_HANDLE);
		app_bt_free_buffer(p_rsp);
		return WICED_BT_GATT_INVALID_HANDLE;
	}

	switch ( p_read_req->s_handle )
	{
	// Add action when specified handle is read
	case HDLC_PSOC_COMMAND_VALUE:
#ifdef UNUSE_I2S
		printf( "LED is %s\r\n", app_psoc_command[0] ? "ON" : "OFF" );
#endif
		break;
	}

	/* Send the response */
	wiced_bt_gatt_server_send_read_by_type_rsp(conn_id, opcode, pair_len, used, p_rsp, (void *)app_bt_free_buffer);

	return WICED_BT_GATT_SUCCESS;
}

/*******************************************************************************
 * Function Name: app_bt_gatt_req_read_multi_handler
 *
 * Process write read multi request from peer device
 *
 * Params:	conn_id       			Connection ID
 * 			opcode        			BLE GATT request type opcode
 * 			p_read_req    			Pointer to read request containing the handle to read
 * 			len_requested 			Length of data requested
 * Return:	wiced_bt_gatt_status_t  BLE GATT status
 *******************************************************************************/
static wiced_bt_gatt_status_t app_bt_gatt_req_read_multi_handler(uint16_t conn_id, wiced_bt_gatt_opcode_t opcode,
		wiced_bt_gatt_read_multiple_req_t *p_read_req, uint16_t len_requested)
{
	gatt_db_lookup_table_t *puAttribute;
	uint8_t *p_rsp = app_bt_alloc_buffer(len_requested);
	int used = 0;
	int xx;
	uint16_t handle = wiced_bt_gatt_get_handle_from_stream(p_read_req->p_handle_stream, 0);

	if (p_rsp == NULL)
	{
		wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, handle, WICED_BT_GATT_INSUF_RESOURCE);
		return WICED_BT_GATT_INVALID_HANDLE;
	}

	/* Read by type returns all attributes of the specified type, between the start and end handles */
	for (xx = 0; xx < p_read_req->num_handles; xx++)
	{
		handle = wiced_bt_gatt_get_handle_from_stream(p_read_req->p_handle_stream, xx);
		if ((puAttribute = app_bt_find_by_handle(handle)) == NULL)
		{
			wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, *p_read_req->p_handle_stream, WICED_BT_GATT_ERR_UNLIKELY);
			app_bt_free_buffer(p_rsp);
			return WICED_BT_GATT_ERR_UNLIKELY;
		}

		{
			int filled = wiced_bt_gatt_put_read_multi_rsp_in_stream(opcode, p_rsp + used, len_requested - used,
					puAttribute->handle, puAttribute->cur_len, puAttribute->p_data);
			if (!filled)
			{
				break;
			}
			used += filled;
		}
	}

	if (used == 0)
	{
		wiced_bt_gatt_server_send_error_rsp(conn_id, opcode, *p_read_req->p_handle_stream, WICED_BT_GATT_INVALID_HANDLE);
		return WICED_BT_GATT_INVALID_HANDLE;
	}

	switch ( *p_read_req->p_handle_stream )
	{
	// Add action when specified handle is read
	case HDLC_PSOC_COMMAND_VALUE:
#ifdef UNUSE_I2S
		printf( "LED is %s\r\n", app_psoc_command[0] ? "ON" : "OFF" );
#endif
		break;
	}

	/* Send the response */
	wiced_bt_gatt_server_send_read_multiple_rsp(conn_id, opcode, used, p_rsp, (void *)app_bt_free_buffer);

	return WICED_BT_GATT_SUCCESS;
}


/*******************************************************************************
 * Function Name: app_bt_find_by_handle
 *
 * Finds attribute location by handle
 *
 * Param:  handle    				handle to look up
 * Return: gatt_db_lookup_table_t   	pointer to location containing handle data
 ********************************************************************************/
static gatt_db_lookup_table_t *app_bt_find_by_handle(uint16_t handle)
{
	int i;
	for (i = 0; i < app_gatt_db_ext_attr_tbl_size; i++)
	{
		if (app_gatt_db_ext_attr_tbl[i].handle == handle)
		{
			return (&app_gatt_db_ext_attr_tbl[i]);
		}
	}
	return NULL;
}

/*******************************************************************************
 * Function Name: app_bt_alloc_buffer
 *
 * This Function allocates the buffer of requested length
 *
 * Param:  len			Length of buffer
 * Return: uint8_t*      Pointer to allocated buffer
 ********************************************************************************/
static uint8_t *app_bt_alloc_buffer(uint16_t len)
{
	uint8_t *p = (uint8_t *)malloc(len);
	return p;
}

/*******************************************************************************
 * Function Name: app_bt_free_buffer
 *
 * This Function frees the buffer requested
 *
 * Param:  p_data		Pointer to buffer to be freed
 ********************************************************************************/
static void app_bt_free_buffer(uint8_t *p_data)
{
	if (p_data != NULL)
	{
		free(p_data);
	}
}

/*------------> Callback function from command <------------*/
//Flag untuk menampilkan data air quality menggunakan BLE Notify
static void airquality_pages_cb (char *str_)
{
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
	air_quality_notify = true;

	daily_notify_BLE = false;
	monthly_notify_BLE = false;
	environment_notify = false;
}

//Flag untuk menampilkan data environment menggunakan BLE Notify
static void environment_pages_cb (char *str_)
{
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
	environment_notify = true;

	daily_notify_BLE = false;
	monthly_notify_BLE = false;
	air_quality_notify = false;
}

//Digunakan untuk mengisi buffer dengan waktu alarm akan aktif di setiap hari
static void show_daily_alarm(char *str_)
{
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
	daily_notify_BLE = true;

	monthly_notify_BLE = false;
	air_quality_notify = false;
	environment_notify = false;
}

//Digunakan untuk mengisi buffer dengan tanggal dan waktu alarm akan aktif di setiap bualan
static void show_monthly_alarm(char *str_)
{
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
	monthly_notify_BLE = true;

	daily_notify_BLE = false;
	air_quality_notify = false;
	environment_notify = false;
}

//Digunakan untuk mengconfigur alarm harian
static void daily_set(char *str)
{
	daily = true;
	monthly = false;
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
}

//Digunakan untuk mengconfigur alarm bulanan
static void monthly_set(char *str_)
{
	monthly = true;
	daily = false;
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
}

//Digunakan untuk mengsetting waktu baru pada RTC
static void set_rtc_val_cb(char *str_)
{
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
	int set_rtc_data[6];
	char* token_rtc = strtok(str_, "RTC_SET ");

	while (token_rtc != NULL)
	{
		for(uint8_t i = 0; i < 6; i++)
		{
			set_rtc_data[i] = atoi(token_rtc);
			token_rtc = strtok(NULL, "RTC_SET ");
			vTaskDelay(10);
		}
	}
	if(token_rtc == NULL)
	{
#ifdef UNUSE_I2S
		printf("Data set RTC = %d, %d, %d, %d, %d, %d\r\n",set_rtc_data[0], set_rtc_data[1], set_rtc_data[2], set_rtc_data[3], set_rtc_data[4], set_rtc_data[5]);
#endif
		RTC_Setup.hour 	= (uint8_t)set_rtc_data[0];
		RTC_Setup.min 	= (uint8_t)set_rtc_data[1];
		RTC_Setup.sec 	= (uint8_t)set_rtc_data[2];
		RTC_Setup.mday 	= (uint8_t)set_rtc_data[3];
		RTC_Setup.month	= (uint8_t)set_rtc_data[4];
		RTC_Setup.Year 	= (uint16_t)set_rtc_data[5];
		set_new_time();
	}
}

//Digunakan untuk mengsetting nilai alarm harian
static void set_val_alarm_daily_cb(char *str_)
{
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
	char* token_daily_alarm = strtok(str_, "Daily ");

	while (token_daily_alarm != NULL)
	{
		for(uint8_t i = 0; i < 2; i++)
		{
			set_daily_alarm_data[i] = atoi(token_daily_alarm);
			token_daily_alarm = strtok(NULL, "Daily ");
			vTaskDelay(10);
		}
	}
	if(token_daily_alarm == NULL)
	{
#ifdef UNUSE_I2S
		printf("Data set daily = %d : %d\r\n",set_daily_alarm_data[0], set_daily_alarm_data[1]);
#endif
		daily_alarm.hour 	= (uint8_t)set_daily_alarm_data[0];
		daily_alarm.min 	= (uint8_t)set_daily_alarm_data[1];

		daily_alarm_show.hour 	= (uint8_t)set_daily_alarm_data[0];
		daily_alarm_show.minute = (uint8_t)set_daily_alarm_data[1];

		set_alarm();
	}
}

//Digunakan untuk mengsetting nilai alarm bulanan
static void set_val_alarm_monthly_cb(char *str_)
{
#ifdef UNUSE_I2S
	printf("= is command\r\n");
#endif
	char* token_monthly_alarm = strtok(str_, "Monthly ");

	while (token_monthly_alarm != NULL)
	{
		for(uint8_t i = 0; i < 3; i++)
		{
			set_monthly_alarm_data[i] = atoi(token_monthly_alarm);
			token_monthly_alarm = strtok(NULL, "Monthly ");
			vTaskDelay(10);
		}
	}
	if(token_monthly_alarm == NULL)
	{
#ifdef UNUSE_I2S
		printf("Data set monthly = %d : %d, %d\r\n",set_monthly_alarm_data[0], set_monthly_alarm_data[1], set_monthly_alarm_data[2]);
#endif
		montly_alarm.hour 	= (uint8_t)set_monthly_alarm_data[0];
		montly_alarm.min 	= (uint8_t)set_monthly_alarm_data[1];

		monthly_alarm_show.hour 	= (uint8_t)set_monthly_alarm_data[0];
		monthly_alarm_show.minute 	= (uint8_t)set_monthly_alarm_data[1];
		monthly_alarm_show.date		= (uint8_t)set_monthly_alarm_data[2];

		set_alarm();
	}
}

/*------------> Eliminate char for end of command and store string then compare string with command bank using strtok <------------*/
static void command_exec (char *txt)
{
	/* Geting length of bank command for compare command bank with data received */
	uint16_t len_bank = getSize (cmd_bank);
	//  printf (" bank found %d \r\n", len_bank);

	/* Eliminate "\r" for end of string data using strtok and store string onto token */
	char *token = strtok (txt, tail);

	/* Check token is not Null */
	while (token != NULL)
	{
		for (uint16_t i = 0; i < len_bank; i++)
		{
#ifdef UNUSE_I2S
		printf (" %s\t", token);
#endif
			/* Checking token is equal with command bank */
			if (strstr (token, cmd_bank[i].cmd) != NULL)
			{

				/* When token is equal with command bank, store the token value to bank callback function */
				cmd_bank[i].cb (token);
				break;
			}
		}
#ifdef UNUSE_I2S
		printf (" not key !\r\n ");
#endif
		token = strtok (NULL, tail);
	}
}
