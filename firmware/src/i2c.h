/**
 * @file  i2c.h
 * @brief Definitions and prototypes for I2C functions
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
#ifndef I2C_H
#define I2C_H
#include "hardware.h"
#include "types.h"

#define I2C_ADDR      SERCOM0_ADDR

#define I2C_RD 1
#define I2C_WR 0

#define I2C_ST_WAIT 0x20000
#define I2C_RD_WAIT 0x20000

void i2c_init(void);
int  i2c_start(unsigned short addr, int rw);
int  i2c_stop (void);
int  i2c_read (unsigned char *data, int again);
int  i2c_write(unsigned char data);

#endif
/* EOF */
