/**
 * @file  main.c
 * @brief Entry point of the C code (main)
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
#include "i2c.h"
#include "uart.h"

/**
 * @brief Entry point of the C code (called by reset handler)
 *
 * @return integer Result of execution ... but should never return :)
 */
int main(void)
{
	/* Initialize clocks and low-level hardware */
	hw_init();
	/* Initialize peripherals */
	i2c_init();
	uart_init();

	uart_puts("PMOD-TRH: Started\r\n");

	while(1)
	{
		/* We will do something here later :p */
	}

	return(0);
}
/* EOF */
