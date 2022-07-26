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

void read_temp(void)
{
	unsigned char b;
	unsigned int temp_code;
	unsigned int temp;

	if (i2c_start(0x40, I2C_WR))
		goto err_start;
	/* Send command : read temperature */
	if (i2c_write(0xE3))
		goto err_cmd;
	/* Repeated start to start a read sequence */
	i2c_start(0x40, I2C_RD);
	/* Read MS byte */
	i2c_read(&b, 1);
	temp_code = (b << 8);
	/* Read LS byte */
	i2c_read(&b, 0);
	temp_code |= b;

	i2c_stop();

	uart_puts("TEMP: ");
	uart_puthex(temp_code, 16);
	uart_puts("  ");
	temp = (17572 * temp_code);
	temp = (temp / 65536);
	temp = temp - 4685;
	uart_putdec(temp);
	uart_puts("\r\n");

	return;
err_start:
	uart_puts("TEMP: Error during START\r\n");
	goto err;
err_cmd:
	uart_puts("TEMP: Error when sending command\r\n");
err:
	//asm volatile("bkpt #0");
	return;
}

void read_rh(void)
{
	unsigned char b;
	unsigned int rh_code, temp_code;
	unsigned int rh, temp;

	if (i2c_start(0x40, I2C_WR))
		goto err_rh_start;
	/* Send command : read relative humidity */
	if (i2c_write(0xE5))
		goto err_rh_cmd;
	/* Repeated start to start a read sequence */
	if (i2c_start(0x40, I2C_RD))
		goto err_rh_restart;
	/* Read MS byte */
	i2c_read(&b, 1);
	rh_code = (b << 8);
	/* Read LS byte */
	i2c_read(&b, 0);
	rh_code |= b;
	/* End of transaction */
	i2c_stop();

	if (i2c_start(0x40, I2C_WR))
		goto err_temp;
	/* Send command : read temperature of previous RH measure */
	if (i2c_write(0xE0))
		goto err_temp;
	/* Repeated start to start a read sequence */
	if (i2c_start(0x40, I2C_RD))
		goto err_temp;
	/* Read MS byte */
	i2c_read(&b, 1);
	temp_code = (b << 8);
	/* Read LS byte */
	i2c_read(&b, 0);
	temp_code |= b;
	/* End of transaction */
	i2c_stop();

	/* Decode RH value */
	rh = (12500 * rh_code);
	rh = (rh / 65536);
	rh = rh - 6;
	/* Decode temperature value */
	temp = (17572 * temp_code);
	temp = (temp / 65536);
	temp = temp - 4685;

#ifdef VERBOSE
	uart_puts("RH: ");
	uart_putdec(rh / 100);
	uart_putc(',');
	uart_putdec(rh % 100);
	uart_puts(" (");
	uart_puthex(rh_code, 16);
	uart_puts(")    ");
	uart_puts("TEMP: ");
	uart_putdec(temp / 100);
	uart_putc(',');
	uart_putdec(temp % 100);
	uart_puts(" (");
	uart_puthex(temp_code, 16);
	uart_puts(")\r\n");
#else
	uart_putc(';');
	uart_putdec(rh / 100);
	uart_putc(',');
	uart_putdec(rh % 100);
	uart_putc(';');
	uart_putdec(temp / 100);
	uart_putc(',');
	uart_putdec(temp % 100);
	uart_puts("\r\n");
#endif

	return;
err_rh_start:
	uart_puts("RH: Error during START (RH)\r\n");
	goto err;
err_rh_cmd:
	uart_puts("RH: Error when sending RH measure command\r\n");
	goto err;
err_rh_restart:
	uart_puts("RH: Error during RESTART (RH)\r\n");
	goto err;
err_temp:
	uart_puts("RH: Error during read of temperature\r\n");
	goto err;
err:
	//asm volatile("bkpt #0");
	return;
}

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
 * @brief Interrupt service routine for Systick
 *
 */
void SysTick_Handler(void)
{
	tm_tick ++;
}

/**
 * @brief Entry point of the C code (called by reset handler)
 *
 * @return integer Result of execution ... but should never return :)
 */
int main(void)
{
	unsigned int i;

	/* Initialize clocks and low-level hardware */
	hw_init();
	time_init();
	/* Initialize peripherals */
	i2c_init();
	uart_init();

	uart_puts("PMOD-TRH: Started\r\n");

	i2c_start(0xAA, I2C_WR);
	read_temp();

	while(1)
	{
		read_rh();
		/* Wait 1sec */
		i = tm_tick;
		while ((tm_tick - i) < 1000)
			asm volatile("nop");
	}

	return(0);
}
/* EOF */
