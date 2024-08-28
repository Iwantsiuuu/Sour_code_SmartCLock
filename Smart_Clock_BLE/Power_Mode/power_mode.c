/*
 * power_mode.c
 *
 *  Created on: 21 Jun 2024
 *      Author: bahariridwan
 */

#include "power_mode.h"

cyhal_clock_t system_clock;

bool flag_normal_power_mode = false;

cy_rslt_t init_power_mode()
{
    cy_rslt_t result;

    /* Reserve, take ownership of, the specified clock instance*/
    result = cyhal_clock_reserve(&system_clock, &CYHAL_CLOCK_FLL);

    /* If the clock is not already enabled, enable it*/
    if (!cyhal_clock_is_enabled(&system_clock))
    {
        result = cyhal_clock_set_enabled(&system_clock, true, true);
    }

    cyhal_syspm_callback_data_t clk_callback = {clk_power_callback,             /* Callback function */
                                               (cyhal_syspm_callback_state_t)
                                               (CYHAL_SYSPM_CB_CPU_SLEEP |
                                                CYHAL_SYSPM_CB_SYSTEM_NORMAL |
                                                CYHAL_SYSPM_CB_SYSTEM_LOW),     /* Power States supported */
                                               (cyhal_syspm_callback_mode_t)
                                               (CYHAL_SYSPM_CHECK_READY |
                                                CYHAL_SYSPM_CHECK_FAIL),        /* Modes to ignore */
                                                NULL,                           /* Callback Argument */
                                                NULL};                          /* For internal use */

    cyhal_syspm_callback_data_t display_callback = {display_power_callback,     /* Callback function */
                                               (cyhal_syspm_callback_state_t)
                                               (CYHAL_SYSPM_CB_CPU_SLEEP |
                                                CYHAL_SYSPM_CB_SYSTEM_NORMAL |
                                                CYHAL_SYSPM_CB_SYSTEM_LOW),     /* Power States supported */
                                               (cyhal_syspm_callback_mode_t)
                                               (CYHAL_SYSPM_CHECK_READY |
                                                CYHAL_SYSPM_CHECK_FAIL),        /* Modes to ignore */
                                                NULL,                           /* Callback Argument */
                                                NULL};                          /* For internal use */

    /* Initialize the System Power Management */
    result = cyhal_syspm_init();

    /* Power Management Callback registration */
    cyhal_syspm_register_callback(&clk_callback);
    cyhal_syspm_register_callback(&display_callback);

    return result;
}

static bool clk_power_callback(cyhal_syspm_callback_state_t state, cyhal_syspm_callback_mode_t mode, void *arg)
{
    (void) arg;

    if (mode == CYHAL_SYSPM_AFTER_TRANSITION)
    {
        if (state == CYHAL_SYSPM_CB_SYSTEM_NORMAL)
        {
            /* Set the system clock to 100 MHz */
            cyhal_clock_set_frequency(&system_clock, CLOCK_100_MHZ, NULL);
        }
    }
    else if (mode == CYHAL_SYSPM_BEFORE_TRANSITION)
    {
        if (state == CYHAL_SYSPM_CB_SYSTEM_LOW)
        {
            /* Set the System Clock to 50 MHz */
            cyhal_clock_set_frequency(&system_clock, CLOCK_50_MHZ, NULL);
        }
    }

    return true;
}

static bool display_power_callback(cyhal_syspm_callback_state_t state, cyhal_syspm_callback_mode_t mode, void *arg)
{
    (void) arg;

    if (mode == CYHAL_SYSPM_BEFORE_TRANSITION)
    {
        if (state == CYHAL_SYSPM_CB_CPU_SLEEP)
        {
            /* Check if the device is in Low Power Mode */
            if (cyhal_syspm_get_system_state() == CYHAL_SYSPM_SYSTEM_LOW)
            {
                flag_normal_power_mode = false;
            }
            else
            {
                flag_normal_power_mode = true;
            }
        }
    }
    else if (mode == CYHAL_SYSPM_AFTER_TRANSITION)
    {
        switch (state)
        {
            case CYHAL_SYSPM_CB_CPU_SLEEP:
                flag_normal_power_mode = false;
            	break;

            case CYHAL_SYSPM_CB_SYSTEM_NORMAL:
                flag_normal_power_mode = true;

                break;

            case CYHAL_SYSPM_CB_SYSTEM_LOW:
                flag_normal_power_mode = false;
                break;

            default:
                break;
        }

    }

    return true;
}
