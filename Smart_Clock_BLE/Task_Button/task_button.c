#include "main.h"
#include "task_button.h"

button_var btn_obj[NUM_OF_BTN];

cyhal_gpio_t btn_pin[NUM_OF_BTN] = {
		BTN_UP,
		BTN_DOWN,
		BTN_ENTER,
		BTN_BACK
};

void ButtonApp()
{
	uint8_t btn_num = 0;
	while(!systemReady)
	{
		vTaskDelay(pdMS_TO_TICKS(5));
	}

	for(;;)
	{
		//Checking button pressed single press, long press, or double click
		button.check(&btn_obj[btn_num]);
		btn_num++;
		if (btn_num >= NUM_OF_BTN)
			btn_num = 0;
		vTaskDelay(1);
	}
}

void ButtonInit()
{
//Set tick at button library using xTaskGetTickCount
	button.setTick(xTaskGetTickCount);

	//Set time for long press and debounce button
	for (uint8_t i = 0; i < NUM_OF_BTN; i++)
	{
		button.setHoldTime(&btn_obj[i], 1000);
		button.setDebounceTime(&btn_obj[i],20);
	}

	//Initialize button as GPIO input
	for (uint8_t i = 0; i < NUM_OF_BTN; i++)
	{
#ifdef UNUSE_I2S
		if( button.create(&btn_obj[i], btn_pin[i], BUTTON_LOW, 200) == CY_RSLT_SUCCESS )
			printf(" button %i initialize success \r\n", i);
#endif

#ifdef USE_I2S
		button.create(&btn_obj[i], btn_pin[i], BUTTON_LOW, 200);
#endif

	}
}
