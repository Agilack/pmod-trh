/**
 * @file  si7021.c
 * @brief This module is a driver for the si7021 temp/humidity chip
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
#include "i2c.h"
#include "si7021.h"
#include "uart.h"

#ifdef SI7021_INFO
static int si7021_errno;

static const char err_null[]    = "";
static const char err_start[]   = "Error during I2C START";
static const char err_restart[] = "Error during I2C repeated START";
static const char err_cmd[]     = "Error when sending command";
#endif

/**
 * @brief Initialize the si7021 driver
 *
 */
void si7021_init(void)
{
#ifdef SI7021_INFO
	si7021_errno = 0;
#endif
}

/**
 * @brief Get an error message (string) for last or specified error code
 *
 * @param error Code of the error (0 for last)
 * @return char* Pointer to a string with error message
 */
const char *si7021_strerror(int error)
{
#ifdef SI7021_INFO
	int ecode;

	ecode = (error == 0) ? si7021_errno : error;
	switch(ecode)
	{
		case -2: return(err_start);
		case -3: return(err_restart);
		case -4: return(err_cmd);
		default: return(err_null);
	}
#else
	return(0);
#endif
}

/**
 * @brief Read the si7021 device id (serial number)
 *
 * @param id Pointer to an array where readed ID can be stored
 * @return integer On success zero is returned, other values are errors
 */
int si7021_read_id(unsigned char *id)
{
	unsigned char tab[8];
	int i;
#ifdef SI7021_INFO
	si7021_errno = 0;
#else
	int si7021_errno;
#endif

	if (i2c_start(0x40, I2C_WR))
		goto err_start;
	/* Send command : Read ID #1 */
	if (i2c_write(0xFA))
		goto err_cmd;
	if (i2c_write(0x0F))
		goto err_cmd;
	/* Repeated start to start a read sequence */
	if (i2c_start(0x40, I2C_RD))
		goto err_restart;
	for (i = 0; i < 7; i++)
		i2c_read(&tab[i], 1);
	i2c_read(&tab[7], 0);
	/* End of transaction */
	i2c_stop();

	/* Copy SNAx values into result */
	if (id)
	{
		*id++ = tab[0]; /* SNA3 */
		*id++ = tab[2]; /* SNA2 */
		*id++ = tab[4]; /* SNA1 */
		*id++ = tab[6]; /* SNA0 */
	}

	if (i2c_start(0x40, I2C_WR))
		goto err_start;
	/* Send command : Read ID #2 */
	if (i2c_write(0xFC))
		goto err_cmd;
	if (i2c_write(0xC9))
		goto err_cmd;
	/* Repeated start to start a read sequence */
	if (i2c_start(0x40, I2C_RD))
		goto err_restart;
	for (i = 0; i < 5; i++)
		i2c_read(&tab[i], 1);
	i2c_read(&tab[5], 0);
	/* End of transaction */
	i2c_stop();

	/* Copy SNBx values into result */
	if (id)
	{
		*id++ = tab[0]; /* SMB3 (model) */
		*id++ = tab[1]; /* SNB2 */
		*id++ = tab[3]; /* SNB1 */
		*id++ = tab[4]; /* SNB0 */
	}
	
	return(0);

err_start:
	si7021_errno = -2;
	goto err;
err_restart:
	si7021_errno = -3;
	goto err;
err_cmd:
	si7021_errno = -4;
err:
	return(si7021_errno);
}

/**
 * @brief Send a reset command to si7021 sensor
 *
 * @return integer On success zero is returned, other values are errors
 */
int si7021_reset(void)
{
#ifdef SI7021_INFO
	si7021_errno = 0;
#else
	int si7021_errno;
#endif

	if (i2c_start(0x40, I2C_WR))
		goto err_start;
	/* Send command : reset */
	if (i2c_write(0xFE))
		goto err_cmd;
	/* End of transaction */
	i2c_stop();

	return(0);

err_start:
	si7021_errno = -2;
	goto err;
err_cmd:
	si7021_errno = -4;
err:
	return(si7021_errno);
}

/**
 * @brief Read the current relative humidity from si7021
 *
 * @param temp Pointer to a variable where readed humidity can be stored
 * @return integer Zero is returned on success, other values are errors
 */
int si7021_rh(unsigned int *rh)
{
	unsigned char b;
	unsigned int rh_code;
	unsigned int result;

#ifdef SI7021_INFO
	si7021_errno = 0;
#else
	int si7021_errno;
#endif

	if (i2c_start(0x40, I2C_WR))
		goto err_start;
	/* Send command : read relative humidity */
	if (i2c_write(0xE5))
		goto err_cmd;
	/* Repeated start to start a read sequence */
	if (i2c_start(0x40, I2C_RD))
		goto err_restart;
	/* Read MS byte */
	i2c_read(&b, 1);
	rh_code = (b << 8);
	/* Read LS byte */
	i2c_read(&b, 0);
	rh_code |= b;
	/* End of transaction */
	i2c_stop();

	/* Decode RH value */
	result = (12500 * rh_code);
	result = (result / 65536);
	result = result - 6;
	if (rh)
		*rh = result;
	return(0);

err_start:
	si7021_errno = -2;
	goto err;
err_restart:
	si7021_errno = -3;
	goto err;
err_cmd:
	si7021_errno = -4;
err:
	return(si7021_errno);
}

/**
 * @brief Read the current temperature from si7021
 *
 * @param temp Pointer to a variable where readed temperature can be stored
 * @return integer Zero is returned on success, other values are errors
 */
int si7021_temp(int *temp)
{
	unsigned char b;
	unsigned int temp_code;
	int result;

#ifdef SI7021_INFO
	si7021_errno = 0;
#else
	int si7021_errno;
#endif

	if (i2c_start(0x40, I2C_WR))
		goto err_start;
	/* Send command : read temperature */
	if (i2c_write(0xE3))
		goto err_cmd;
	/* Repeated start to start a read sequence */
	if (i2c_start(0x40, I2C_RD))
		goto err_restart;
	/* Read MS byte */
	i2c_read(&b, 1);
	temp_code = (b << 8);
	/* Read LS byte */
	i2c_read(&b, 0);
	temp_code |= b;

	i2c_stop();

	/* Compute temperature value */
	result = (17572 * temp_code);
	result = (result / 65536);
	result = result - 4685;
	/* If caller want the result, copy it */
	if (temp)
		*temp = result;

	return(0);

err_start:
	si7021_errno = -2;
	goto err;
err_restart:
	si7021_errno = -3;
	goto err;
err_cmd:
	si7021_errno = -4;
err:
	return(si7021_errno);
}

/**
 * @brief Get the temperature captured during last humidity measurement
 *
 * @param temp Pointer to a variable where readed temperature can be stored
 * @return integer Zero is returned on success, other values are errors
 */
int si7021_temp_last(int *temp)
{
	unsigned char b;
	unsigned int  temp_code;
	int result;

#ifdef SI7021_INFO
	si7021_errno = 0;
#else
	int si7021_errno;
#endif

	if (i2c_start(0x40, I2C_WR))
		goto err_start;
	/* Send command : read temperature of previous RH measure */
	if (i2c_write(0xE0))
		goto err_cmd;
	/* Repeated start to start a read sequence */
	if (i2c_start(0x40, I2C_RD))
		goto err_restart;
	/* Read MS byte */
	i2c_read(&b, 1);
	temp_code = (b << 8);
	/* Read LS byte */
	i2c_read(&b, 0);
	temp_code |= b;

	i2c_stop();

	/* Decode temperature value */
	result = (17572 * temp_code);
	result = (result / 65536);
	result = result - 4685;
	/* If caller want the result, copy it */
	if (temp)
		*temp = result;

	return(0);

err_start:
	si7021_errno = -2;
	goto err;
err_restart:
	si7021_errno = -3;
	goto err;
err_cmd:
	si7021_errno = -4;
err:
	return(si7021_errno);
}
/* EOF */
