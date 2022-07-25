/**
 * @file  uart.h
 * @brief Definitions and prototypes for UART functions
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
#ifndef UART_H
#define UART_H
#include "hardware.h"
#include "types.h"

#define UART_ADDR      SERCOM1_ADDR

void uart_init(void);
/* Basic IOs */
void uart_putc(unsigned char c);
/* Send structured content */
void uart_puts(char *s);
void uart_putdec(const u32 v);
void uart_puthex(const u32 c, const uint len);

#endif
/* EOF */
