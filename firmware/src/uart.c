/**
 * @file  uart.c
 * @brief Handle UART port (using sercom peripheral)
 *
 * @author Saint-Genest Gwenael <gwen@cowlab.fr>
 * @copyright Cowlab (c) 2017-2022
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
#include "uart.h"

#define UART_BAUD    9600
#define UART_GCLK 8000000
#define CONF_BAUD  (65536 - ((65536 * 16.0f * UART_BAUD) / UART_GCLK))

static const u8 hex[16] = "0123456789ABCDEF";

/**
 * @brief Initialize and configure UART port
 *
 */
void uart_init(void)
{
	/* 1) Enable peripheral and set clocks */

	/* Enable SERCOM1 clock (APBCMASK) */
	reg_set(PM_ADDR + 0x20, (1 << 3));
	/* Set GCLK for SERCOM1 (generic clock generator 0) */
	reg16_wr (GCLK_ADDR + 0x02, (1 << 14) | (0 << 8) | 15);

	/* 2) Initialize UART block   */

	/* Reset UART (set SWRST)     */
	reg_wr((UART_ADDR + 0x00), 0x01);
	/* Wait end of software reset */
	while( reg_rd(UART_ADDR + 0x00) & 0x01)
		;
	/* Configure UART */
	reg_wr(UART_ADDR + 0x00, 0x40310004);
	reg_wr(UART_ADDR + 0x04, 0x00030000);
	/* Configure Baudrate */
	reg16_wr(UART_ADDR + 0x0C, CONF_BAUD);
	/* Set ENABLE into CTRLA */
	reg_set( (UART_ADDR + 0x00), (1 << 1) );

	/* 3) Configure pins (IOs) */

	/* PINCFG: Enable PMUX for RX/TX pins */
	reg8_wr(0x60000000 + 0x58, 0x01); /* PA24 : TX */
	reg8_wr(0x60000000 + 0x59, 0x01); /* PA25 : RX */
	/* Set peripheral function C (SERCOM) for PA24 and PA25 */
	reg8_wr(0x60000000 + 0x3C, (0x02 << 4) | (0x02 << 0));
}

/**
 * @brief Send a single byte over UART
 *
 * @param c Character (or binary byte) to send
 */
void uart_putc(unsigned char c)
{
	/* Read INTFLAG and wait DRE (Data Register Empty) */
	while ( (reg_rd(UART_ADDR + 0x18) & 0x01) == 0)
		;
	/* Write data */
	reg_wr((UART_ADDR + 0x28), c);
}

/**
 * @brief Print a numerical value in decimal
 *
 * @param v Value to display
 */
void uart_putdec(const u32 v)
{
	unsigned int  decade = 1000000000;
	unsigned long n;
	char str[16];
	char *d;
	int  count = 0;
	int  i;

	// First, convert the value to text string
	n = v;
	d = str;
	for (i = 0; i < 9; i++)
	{
		if ((n > (decade - 1)) || count)
		{
			*d = (u8)(n / decade) + '0';
			n -= ((n / decade) * decade);
			d++;
			count++;
		}
		decade = (decade / 10);
	}
	*d = (u8)(n + '0');
	count ++;
	// Insert a strig terminaison byte
	d[1] = 0;

	// Then, print string
	uart_puts(str);
}

/**
 * @brief Send a text-string over UART
 *
 * @param s Pointer to the null terminated text string
 */
void uart_puts(char *s)
{
	/* Loop to process each character */
	while(*s)
	{
		/* Send one byte */
		uart_putc(*s);
		/* Move pointer to next byte */
		s++;
	}
}

/**
 * @brief Send the hexadecimal representation of an integer
 *
 * @param c Binary word (32 bits) to show as hex
 * @param len Number of bits to decode
 */
void uart_puthex(const u32 c, const uint len)
{
	if (len > 28)
		uart_putc( hex[(c >> 28) & 0xF] );
	if (len > 24)
		uart_putc( hex[(c >> 24) & 0xF] );
	if (len > 20)
		uart_putc( hex[(c >> 20) & 0xF] );
	if (len > 16)
		uart_putc( hex[(c >> 16) & 0xF] );
	if (len > 12)
		uart_putc( hex[(c >> 12) & 0xF] );
	if (len >  8)
		uart_putc( hex[(c >>  8) & 0xF] );
	if (len >  4)
		uart_putc( hex[(c >>  4) & 0xF] );
	if (len >  0)
		uart_putc( hex[(c >>  0) & 0xF] );
}
/* EOF */
