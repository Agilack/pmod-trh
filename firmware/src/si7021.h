/**
 * @file  si7021.h
 * @brief Headers and definitions for the si7021 driver
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
#ifndef SI7021_H
#define SI7021_H
#include "types.h"

#define SI7021_INFO

const char *si7021_strerror(int error);
void  si7021_init(void);
int   si7021_read_id(unsigned char *id);
int   si7021_reset(void);
int   si7021_rh(unsigned int *temp);
int   si7021_temp(int *temp);
int   si7021_temp_last(int *temp);

#endif
