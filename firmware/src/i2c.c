/**
 * @file  i2c.c
 * @brief Handle I2C port (using sercom peripheral)
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

/* Bus frequency : ~100kHz with 8MHz GCLK */
#define CONF_I2C_BAUD (35 & 0xFF)

#define I2C_DEBUG

/**
 * @brief Initialize I2C driver
 *
 * This function initialize the sercom peripheral to use it as an I2C bus
 * master. For this driver to work properly, this function must be called
 * before any other i2c functions.
 */
void i2c_init(void)
{
	/* 1) Enable peripheral and set clocks */

	/* Enable SERCOM0 clock (APBCMASK) */
	reg_set(PM_ADDR + 0x20, (1 << 2));
	/* Set GCLK for SERCOM0 (generic clock generator 0) */
	reg16_wr (GCLK_ADDR + 0x02, (1 << 14) | (0 << 8) | 14);

	/* 2) Initialize sercom/I2C block   */

	/* Reset sercom (set SWRST)     */
	reg_wr((I2C_ADDR + 0x00), 0x01);
	/* Wait end of software reset */
	while( reg_rd(I2C_ADDR + 0x00) & 0x01)
		;
	/* Configure interface */
	reg_wr(I2C_ADDR + 0x00, (5 << 2));
	reg_wr(I2C_ADDR + 0x04, 0);
	/* Configure Baudrate */
	reg16_wr(I2C_ADDR + 0x0C, CONF_I2C_BAUD);
	/* Set ENABLE into CTRLA */
	reg_set(I2C_ADDR + 0x00, (1 << 1));

	/* 3) Configure pins (IOs) */

	/* PINCFG: Enable PMUX for SCL/SDA pins */
	reg8_wr(0x60000000 + 0x4E, 0x01); /* PA14 : SDA */
	reg8_wr(0x60000000 + 0x4F, 0x01); /* PA15 : SCL */
	/* Set peripheral function C (SERCOM) for PA14 and PA15 */
	reg8_wr(0x60000000 + 0x37, (0x02 << 4) | (0x02 << 0));
}

/**
 * @brief Read one byte received from I2C bus
 *
 * @param data  Pointer to a variable where readed value can be stored
 * @param again Boolean flag used to request another byte after current
 * @return integer Zero is returned on success, other values are errors
 */
int i2c_read (unsigned char *data, int again)
{
	unsigned long v;
	unsigned int  i;

#ifdef I2C_DEBUG
	/* Verify that I2C sercom is enabled and ready */
	if ((reg_rd(I2C_ADDR + 0x00) & (1 << 1)) == 0)
		return(-9);
#endif

	/* Wait end of transmission (SB or ERROR) */
	for (i = 0; i < I2C_RD_WAIT; i++)
	{
		/* Read INTFLAG */
		v = reg8_rd(I2C_ADDR + 0x18);
		if (v & 0x83)
			break;
		/* Read STATUS */
		v = reg16_rd(I2C_ADDR + 0x1A);
	}
	/* In case of timeout during wait, abort */
	if (i == I2C_RD_WAIT)
		return(-1);
	/* In case of an error, abort */
	if (v & 0x81)
		return(-2);

	/* Read received byte */
	v = reg16_rd(I2C_ADDR + 0x28);
	if (data)
		*data = (v & 0xFF);

	/* If "again" flag is set, initiate another read */
	if (again)
		reg_wr(I2C_ADDR + 0x04, (2 << 16));
	/* Else (last byte) set the acknowledge value to NACK */
	else
		reg_wr(I2C_ADDR + 0x04, (1 << 18));

	return(0);
}

/**
 * @brief Initiate communication with a slave byt sending START and address
 *
 * @param addr Address of the slave device
 * @param rw   Specify if transaction is a read (1) or a write (0)
 * @return integer Zero is returned on success, other values are errors
 */
int i2c_start(unsigned short addr, int rw)
{
	unsigned long v;
	unsigned int  i;

#ifdef I2C_DEBUG
	/* Verify that I2C sercom is enabled and ready */
	if ((reg_rd(I2C_ADDR + 0x00) & (1 << 1)) == 0)
		return(-9);
#endif

	/* If a previous error has not been cleared */
	v = reg8_rd(I2C_ADDR + 0x18);
	if (v & 0x80)
	{
		/* Read STATUS */
		v = reg16_rd(I2C_ADDR + 0x1A);
		/* Clear it now ! */
		reg8_wr(I2C_ADDR + 0x18, 0x80);
	}

	/* Send START, slave address and rw bit */
	v = ((addr << 1) & 0x7FE) | (rw & 1);
	reg_wr(I2C_ADDR + 0x24, v);

	/* Wait for MB or ERROR */
	for (i = 0; i < I2C_ST_WAIT; i++)
	{
		v = reg8_rd(I2C_ADDR + 0x18);
		if (v & 0x83)
			break;
	}
	/* In case of timeout during wait, abort */
	if (i == I2C_ST_WAIT)
		goto err;

	/* Read STATUS */
	v = reg16_rd(I2C_ADDR + 0x1A);
	if (v & 0x07)
		goto err;

	return(0);
err:
	/* Send a STOP condition */
	reg_wr(I2C_ADDR + 0x04, (0x03 << 16));
	/* Clear the ERROR bit */
	reg8_wr(I2C_ADDR + 0x18, 0x80);
	return(-1);
}

/**
 * @brief Terminate a transaction by sending a STOP condition on bus
 *
 * @return integer Zero is returned on success, other values are errors
 */
int i2c_stop(void)
{
	unsigned long v;

#ifdef I2C_DEBUG
	/* Verify that I2C sercom is enabled and ready */
	if ((reg_rd(I2C_ADDR + 0x00) & (1 << 1)) == 0)
		return(-9);
#endif

	/* Read the current acknowledge value */
	v = reg_rd(I2C_ADDR + 0x04) & (1 << 18);

	/* Send a STOP condition */
	reg_wr(I2C_ADDR + 0x04, v | (0x03 << 16));

	return(0);
}

/**
 * @brief Send a byte to the slave device
 *
 * @param data Value to send (byte) to the remote slave device
 * @return integer Zero is returned on success, other values are errors
 */
int i2c_write(unsigned char data)
{
	unsigned long v;

#ifdef I2C_DEBUG
	/* Verify that I2C sercom is enabled and ready */
	if ((reg_rd(I2C_ADDR + 0x00) & (1 << 1)) == 0)
		return(-9);
#endif

	v = reg8_rd(I2C_ADDR + 0x18);

	reg16_wr(I2C_ADDR + 0x28, data);

	/* Wait for MB or ERROR */
	do
		v = reg8_rd(I2C_ADDR + 0x18);
	while ( (v & 0x81) == 0);

	if (v & 0x80)
		return(-1);

	return(0);
}
/* EOF */
