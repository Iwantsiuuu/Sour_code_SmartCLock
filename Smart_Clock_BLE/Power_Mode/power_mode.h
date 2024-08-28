/*
 * power_mode.h
 *
 *  Created on: 21 Jun 2024
 *      Author: bahariridwan
 */
#pragma once

#ifndef POWER_MODE_POWER_MODE_H_
#define POWER_MODE_POWER_MODE_H_

#include "cyhal.h"
#include "cybsp.h"
#include "cyhal_clock.h"

/* Clock frequency constants (in Hz) */
#define CLOCK_50_MHZ            50000000u
#define CLOCK_100_MHZ           100000000u

cy_rslt_t init_power_mode();
static bool clk_power_callback(cyhal_syspm_callback_state_t state, cyhal_syspm_callback_mode_t mode, void *arg);
static bool display_power_callback(cyhal_syspm_callback_state_t state, cyhal_syspm_callback_mode_t mode, void *arg);

extern bool flag_display_power_mode;

#endif /* POWER_MODE_POWER_MODE_H_ */
