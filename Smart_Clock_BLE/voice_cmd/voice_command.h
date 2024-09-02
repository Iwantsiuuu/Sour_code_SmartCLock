/*
 * voice_command.h
 *
 *  Created on: 12 June 2024
 *      Author: Ridwan Bahari
 */

#pragma once

#ifndef VOICE_COMMAND_H_
#define VOICE_COMMAND_H_

#include "cyhal.h"
#include "cybsp.h"
#include "stdlib.h"
#include "cy_retarget_io.h"

#include "main.h"
#include "cyberon_asr.h"

#define FRAME_SIZE                  (480u)
#define SAMPLE_RATE_HZ              (16000u)
#define DECIMATION_RATE             (96u)
#define AUDIO_SYS_CLOCK_HZ          (24576000u)
//#define AUDIO_SYS_CLOCK_HZ          (98000000u)
#define PDM_DATA                    (P10_5)
#define PDM_CLK                     (P10_4)


/*format voice command:
 * Voice trigger:
 * 				 Hello Jam || Hallo Jam || Hi Jam || Hai Jam || Hey Jam || Hay Jam [Map Id = 101]
 *
 * Voice Command:
 * 				 Default Mode	[Map Id = 199]
 * 				 Bluetooth Mode [Map Id = 200]
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

#define	LINGKUNGAN_CMD 	(200)
#define	UDARA_CMD 		(201)
#define	STOPWATCH_CMD 	(202)
#define	HARIAN_CMD 		(203)
#define BULANAN_CMD		(204)
#define MULAI_CMD		(205)
#define	BERHENTI_CMD	(206)
#define ULANG_CMD		(207)
#define	KEMBALI_CMD 	(208)

void init_i2s(void);
void clock_init(void);
void init_pdm_pcm(void);
void voice_command_task(void);

extern uint32_t p_command_id;

#ifdef USE_I2S
extern cyhal_i2s_t i2s;
#endif

#endif
