/*******************************************************************************
 * Header Files
 *******************************************************************************/
#include "main.h"

/*******************************************************************************
 * Macros
 *******************************************************************************/
#define LDO_ENABLE P7_1

#define TASK_STACK_SIZE (1024u)
#define taskPriority    (6)
#define OLED_ADDRESS 0x3c

/*******************************************************************************
 * Global Variables
 *******************************************************************************/
TaskHandle_t voiceHandle;
TaskHandle_t notifyHandle;
TaskHandle_t buttonHandle;
TaskHandle_t sensorHandle;
TaskHandle_t displayHandle;

SemaphoreHandle_t semphr_i2c_dev;

bool systemReady = false;

int main(void)
{
	cy_rslt_t result;
	wiced_result_t wicedRslt;

	/*----> Set year from 2000th <----*/
	RTC_Setup.Year = 2000;
	uint8_t call_oled;

#if defined (CY_DEVICE_SECURE)
	cyhal_wdt_t wdt_obj;

	/* Clear watchdog timer so that it doesn't trigger a reset */
	result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
	CY_ASSERT(CY_RSLT_SUCCESS == result);
	cyhal_wdt_free(&wdt_obj);
#endif

	/* Initialize the device and board peripherals */
	result = cybsp_init();
	/* Board init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
		CY_ASSERT(0);
	}

	/* Enable global interrupts */
	__enable_irq();

	/*----> Clock initialization for i2s and PDM <----*/
	clock_init();

	/*----> Initialization i2c for communication to sensor and Oled LCD <----*/
	init_i2c();

	/*Initialization RTC*/
	result = cyhal_rtc_init(&rtc_obj);
	if(result == CY_RSLT_SUCCESS)
	{
		// Register a callback function to handle the alarm event
		cyhal_rtc_register_callback(&rtc_obj, (cyhal_rtc_event_callback_t)cyhal_rtc_alarm_interrupt_handler, RTC_CALLBACK_ARG);

		// Enable the alarm event to trigger an interrupt
		cyhal_rtc_enable_event(&rtc_obj, CYHAL_RTC_ALARM, RTC_INTERRUPT_PRIORITY, true);
	}

	//	init_power_mode();

#ifdef UNUSE_I2S
	/*----> Initialization retarget_io when using printf for display the value or debugging using serial terminal <----*/
	cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX, CY_RETARGET_IO_BAUDRATE);
#endif

	/*----> Initialization and set pin 7_1 ON for enable the Low-Dropout regulator(LDO) on expansion board development smart clock (interface board) <-------*/
	result = cyhal_gpio_init(LDO_ENABLE, CYHAL_GPIO_DIR_OUTPUT, CYHAL_GPIO_DRIVE_OPENDRAINDRIVESHIGH, CYBSP_LED_STATE_ON);
	if(result == CY_RSLT_SUCCESS)
		cyhal_gpio_write(LDO_ENABLE, 1u);

	/*----> Initialization Button for control smart clock manually <----*/
	ButtonInit();

#ifdef USE_I2S
	init_i2s();
#endif

	/*----> Initialization PDM for MEMS Microphone*/
	init_pdm_pcm();

#ifdef UNUSE_I2S
	printf("**********Application Start*****************\n\r");
#endif

	/*----> Initialization CYBSP_USER_LED1 as PWM for BlueTooth advertisement indicator <----*/
	result = cyhal_pwm_init(&PWM_obj, CYBSP_USER_LED, NULL);
	cyhal_pwm_start(&PWM_obj);

	/* Call oled address for trigger oled before init LCD */
	result = cyhal_i2c_master_write(&i2c, OLED_ADDRESS, &call_oled, 0,100,true);
	if(result == CY_RSLT_SUCCESS)
	{
		/*----> Initialization library u8g2 for LCD Oled 128 x 64 <------*/
		init_u8g2();
	}
	else
	{
		CY_ASSERT(0);
	}

	/*----> Using semaphore for manage transaction on I2C <----*/
	semphr_i2c_dev = xSemaphoreCreateMutex();
	if (semphr_i2c_dev != NULL)
		xSemaphoreGive(semphr_i2c_dev);

	/* Configure platform specific settings for the BT device */
	cybt_platform_config_init(&cybsp_bt_platform_cfg);

	/* Initialize stack and register the callback function */
	wicedRslt = wiced_bt_stack_init (app_bt_management_callback, &wiced_bt_cfg_settings);

#ifdef USE_SENSOR
	/*----> Initialization sensor used for environment and air quality monitoring <----*/
	if(sensorInit() == CY_RSLT_SUCCESS)
		/* After all initialization success, set systemReady to true for run all system*/
		systemReady = true;
	else
		systemReady = false;
#endif

#ifdef USE_DUMMY_DATA
	if(wicedRslt == CY_RSLT_SUCCESS)
		systemReady = true;
	else
		systemReady = false;
#endif

	/*----> Create Task for Button <----*/
	xTaskCreate (
			ButtonApp,
			"Task Button",
			(TASK_STACK_SIZE),
			NULL,
			taskPriority,
			&buttonHandle
	);

	/*----> Create Task for Oled LCD <----*/
	xTaskCreate (
			displayOled,
			"Task Interface",
			(TASK_STACK_SIZE*2),
			NULL,
			(taskPriority-1),
			&displayHandle
	);

	/*----> Create Task for all sensor used <----*/
	xTaskCreate (
			sensor_Reading,
			"Task Sensor",
			(TASK_STACK_SIZE),
			NULL,
			(taskPriority-2),
			&sensorHandle
	);

	/*----> Create Task for BLE Notify and read <----*/
	xTaskCreate (
			notify_task,
			"Bluetooth_Notify",
			(TASK_STACK_SIZE*2),
			NULL,
			(taskPriority-3),
			&notifyHandle
	);

	/*----> Create Task for Speech Command <----*/
	xTaskCreate (
			voice_command_task,
			"Speech_Command",
			(TASK_STACK_SIZE*2),
			NULL,
			(taskPriority-4),
			&voiceHandle
	);

	/* Start the FreeRTOS scheduler */
	vTaskStartScheduler();

	/* Should never get here */
	CY_ASSERT(0);

}
