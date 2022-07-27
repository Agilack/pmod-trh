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
#include "si7021.h"
#include "time.h"
#include "types.h"
#include "uart.h"

/**
 * @brief Entry point of the C code (called by reset handler)
 *
 * @return integer Result of execution ... but should never return :)
 */
int main(void)
{
	unsigned char id[8];
	unsigned int rh;
	int temp;
	u32 tm;
	int i;

	/* Initialize clocks and low-level hardware */
	hw_init();
	time_init();
	/* Initialize peripherals */
	i2c_init();
	uart_init();
	/* Initialize sensor driver */
	si7021_init();

	uart_puts("PMOD-TRH: Started\r\n");

	if (si7021_read_id(id) == 0)
	{
		uart_puts(" * Si7021 serial number ");
		for(i = 0; i < 8; i++)
			uart_puthex(id[i], 8);
		uart_puts("\r\n");
	}

	if (si7021_temp(&temp) == 0)
	{
		uart_puts("TEMP: ");
		uart_putdec(temp);
		uart_puts("\r\n");
	}
	else
	{
		uart_puts("TEMP: ");
		uart_puts((char *)si7021_strerror(0));
		uart_puts("\r\n");
	}

	while(1)
	{
		/* Read current relative humidity */
		uart_puts("RH=");
		if (si7021_rh(&rh) == 0)
		{
			uart_putdec(rh / 100);
			uart_putc(',');
			uart_putdec(rh % 100);
		}
		else
			uart_puts("ERROR");
		/* Get temperature captured during RH measurement */
		uart_puts(" TEMP=");
		if (si7021_temp_last(&temp) == 0)
		{
			uart_putdec(temp / 100);
			uart_putc(',');
			uart_putdec(temp % 100);
		}
		else
			uart_puts("ERROR");
		uart_puts("\r\n");

		/* Wait 1sec */
		tm = time_now();
		while (time_since(tm) < 1000)
			asm volatile("nop");
	}

	return(0);
}
/* EOF */
