/*
 * voice_command.c
 *
 * 	Created on: 13 June 2024
 * 	    Author: Ridwan Bahari
 */

/*License CYBLE-416045-02
 *
	.pushsection license_data, "ax", %progbits
	.incbin "data/F52F2A1F028CAFE41718160741_License.bin"
	.popsection
 */

/*License PSoC CYW9P62S1-43438CAR-01
 *
	.pushsection license_data, "ax", %progbits
	.incbin "data/EF2F17571896353D1718158130_License.bin"
	.popsection
 *
 */

/*format voice command:
 * Voice trigger:
 * 				 Hello dek | Hallo dek | Hi dek | Hai dek| Hey dek | Hay dek [Map Id = 101]
 *
 * Voice Command:
 * 				 Ganti Mode 	[Map Id = 200]
 * 				 Menu 			[Map Id = 201]
 * 				 Atur jam 		[Map Id = 202]
 * 				 Lingkungan 	[Map Id = 203]
 * 				 Udara 			[Map Id = 204]
 * 				 Stopwatch 		[Map Id = 205]
 * 				 Set alarm 		[Map Id = 206]
 * 				 Kembali 		[Map Id = 207]
 * 				 Start			[Map Id = 208]
 * 				 Stop			[Map Id = 209]
 * 				 Reset			[Map Id = 210]
 * */

#include "voice_command.h"

#ifdef USE_I2S
static void i2s_isr_handler(void *arg, cyhal_i2s_event_t event);
#endif

static void asr_callback(const char *function, char *message, char *parameter);
static void pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event);

#ifdef USE_I2S
cyhal_i2s_t i2s;
#endif

cyhal_pdm_pcm_t pdm_pcm;

cyhal_clock_t   audio_clock;
cyhal_clock_t   pll_clock;

volatile bool pdm_pcm_flag = false;

int16_t pdm_pcm_ping[FRAME_SIZE] = {0};
int16_t pdm_pcm_pong[FRAME_SIZE] = {0};
int16_t *pdm_pcm_buffer = &pdm_pcm_ping[0];
int16_t *pdm_pcm_buffer1 = &pdm_pcm_pong[0];

uint32_t p_command_id = 0;

const cyhal_pdm_pcm_cfg_t pdm_pcm_cfg =
{
		.sample_rate     = SAMPLE_RATE_HZ,
		.decimation_rate = DECIMATION_RATE,
		.mode            = CYHAL_PDM_PCM_MODE_LEFT,
		.word_length     = 16,  /* bits */
		.left_gain       = CYHAL_PDM_PCM_MAX_GAIN,   /* dB */
		.right_gain      = CYHAL_PDM_PCM_MAX_GAIN,   /* dB */
};

#ifdef USE_I2S
const cyhal_i2s_pins_t i2s_pins =
{
		.sck  = P5_1,
		.ws   = P5_2,
		.data = P5_3,
		.mclk = NC,
};

const cyhal_i2s_config_t i2s_config =
{
		.is_tx_slave    = false,    /* TX is Master */
		.is_rx_slave    = false,    /* RX not used */
		.mclk_hz        = 0,        /* External MCLK not used */
		.channel_length = 32,       /* In bits */
		.word_length    = 16,       /* In bits */
		.sample_rate_hz = 16000,    /* In Hz */

};
#endif

void clock_init(void)
{
	cyhal_clock_reserve(&pll_clock, &CYHAL_CLOCK_PLL[0]);
	cyhal_clock_set_frequency(&pll_clock, AUDIO_SYS_CLOCK_HZ, NULL);
	cyhal_clock_set_enabled(&pll_clock, true, true);

	cyhal_clock_reserve(&audio_clock, &CYHAL_CLOCK_HF[1]);

	cyhal_clock_set_source(&audio_clock, &pll_clock);
	cyhal_clock_set_enabled(&audio_clock, true, true);
}

#ifdef USE_I2S
void init_i2s(void)
{
	cyhal_i2s_init(&i2s, &i2s_pins, NULL, &i2s_config, &audio_clock);
	cyhal_i2s_register_callback(&i2s, i2s_isr_handler, NULL);
	cyhal_i2s_enable_event(&i2s, CYHAL_I2S_ASYNC_TX_COMPLETE, CYHAL_ISR_PRIORITY_DEFAULT, true);
}
#endif

void init_pdm_pcm(void)
{
	cyhal_pdm_pcm_init(&pdm_pcm, PDM_DATA, PDM_CLK, &audio_clock, &pdm_pcm_cfg);
	cyhal_pdm_pcm_register_callback(&pdm_pcm, pdm_pcm_isr_handler, NULL);
	cyhal_pdm_pcm_enable_event(&pdm_pcm, CYHAL_PDM_PCM_ASYNC_COMPLETE, CYHAL_ISR_PRIORITY_DEFAULT, true);
	cyhal_pdm_pcm_start(&pdm_pcm);
	cyhal_pdm_pcm_read_async(&pdm_pcm, pdm_pcm_buffer, FRAME_SIZE);
}

void voice_command_task(void)
{

#ifdef UNUSE_I2S
	uint64_t uid;
#endif

	while(!systemReady)
	{
		vTaskDelay(5);
	}

#ifdef UNUSE_I2S
	uid = Cy_SysLib_GetUniqueId();
	printf("uniqueIdHi: 0x%08lX, uniqueIdLo: 0x%08lX\r\n", (uint32_t)(uid >> 32), (uint32_t)(uid << 32 >> 32));
#endif

	if(!cyberon_asr_init(asr_callback))
	{
		while(1);
	}

#ifdef UNUSE_I2S
	printf("\r\nAwaiting voice input trigger command (\"Hello Box/Hi box/Hey box\"):\r\n");
#endif

	while(true)
	{
//#ifdef UNUSE_I2S
//		printf("Data Map Id: %d\r\n", *p_command_id);
//#endif
		if(pdm_pcm_flag)
		{
			pdm_pcm_flag = 0;
			cyberon_asr_process(pdm_pcm_buffer, FRAME_SIZE);
		}
	}
}

void pdm_pcm_isr_handler(void *arg, cyhal_pdm_pcm_event_t event)
{
	static bool ping_pong = false;

	(void) arg;
	(void) event;

	if(ping_pong)
	{
		cyhal_pdm_pcm_read_async(&pdm_pcm, pdm_pcm_ping, FRAME_SIZE);
		pdm_pcm_buffer = &pdm_pcm_pong[0];
	}
	else
	{
		cyhal_pdm_pcm_read_async(&pdm_pcm, pdm_pcm_pong, FRAME_SIZE);
		pdm_pcm_buffer = &pdm_pcm_ping[0];
	}

	ping_pong = !ping_pong;
	pdm_pcm_flag = true;
}

static void asr_callback(const char *function, char *message, char *parameter)
{

#ifdef UNUSE_I2S
	printf("[%s][%s] = %s\r\n", function, message, parameter);
#endif

	char* param = strstr(message, (const char*)"Map");
	if(param != NULL)
	{
		int p_cmd_id = 0;
		p_cmd_id = atoi(parameter);
		p_command_id = (uint32_t)p_cmd_id;

		/* Turn OFF LED when device received Trigger Command */
		//cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
	}
	else
	{
		param = strstr(message, (const char*)"Timeout");
		if( param != NULL )
		{
			p_command_id = 0;
			/* Turn ON LED for indicated device needed for trigger Command again */
			//			cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_ON);
		}
	}
}

#ifdef USE_I2S
static void i2s_isr_handler(void *arg, cyhal_i2s_event_t event)
{
	(void) arg;
	(void) event;

	//	flag_play = false;

	/* Stop the I2S TX */
	cyhal_i2s_stop_tx(&i2s);

	/* Turn off the led when data already transfer */
	cyhal_gpio_write(CYBSP_USER_LED, CYBSP_LED_STATE_OFF);
}
#endif
