/**
 * @file  time.c
 * @brief This module contains functions to deal with time and delay
 *
 * @author Saint-Genest Gwenael <gwen@cowlab.fr>
 * @copyright Cowlab (c) 2022
 *
 * @page License
 * This firmware is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3 as published
 * by the Free Software Foundation. You should have received a copy of the
 * GNU General Public License along with this program, see LICENSE.md file
 * for more details.
 * This program is distributed WITHOUT ANY WARRANTY.
 */
#include "hardware.h"
#include "time.h"

static volatile u32 tm_tick;

/**
 * @brief Initialize time module
 *
 * The time module use Systick to create a 1kHz (1ms) time reference. To have a
 * precise value, tick is incremented on interrupt. This module *must* be unload
 * before modifying VTOR.
 */
void time_init(void)
{
	tm_tick = 0;

	/* Configure and start SysTick */
	reg_wr((u32)0xE000E014, 8000);
	reg_wr((u32)0xE000E010, (1 << 2) | (1 << 1) | 1);
}

/**
 * @brief Return the current time (in ms) based on internal counter
 *
 * @return u32 Time (in 1ms) since module started
 */
u32 time_now(void)
{
	return tm_tick;
}

/**
 * @brief Compute the time elapsed from a reference
 *
 * @param  ref Reference time (in ms)
 * @return uint32 Number of ms since "ref"
 */
u32 time_since(u32 ref)
{
	u32 tm_diff;

	tm_diff = (time_now() - ref);

	return(tm_diff);
}

/**
 * @brief Interrupt service routine for Systick
 *
 */
void SysTick_Handler(void)
{
	tm_tick ++;
}
/* EOF */
