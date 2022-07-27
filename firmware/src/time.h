/**
 * @file  time.h
 * @brief Headers and definitions for the time management module
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
#ifndef TIME_H
#define TIME_H
#include "types.h"

void time_init (void);
u32  time_now  (void);
u32  time_since(u32 ref);

#endif
