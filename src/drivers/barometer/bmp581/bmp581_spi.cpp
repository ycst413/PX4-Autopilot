/****************************************************************************
 *
 *   Copyright (c) 2024 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file bmp581_spi.cpp
 *
 * SPI interface for BMP 581 (NOTE: untested!)
 */

#include <drivers/device/spi.h>

#include "bmp581.h"

/* SPI protocol address bits */
#define DIR_READ			(1<<7)  //for set
#define DIR_WRITE			~(1<<7) //for clear

class BMP581_SPI: public device::SPI, public IBMP581
{
public:
	BMP581_SPI(uint8_t bus, uint32_t device, int bus_frequency, spi_mode_e spi_mode);
	virtual ~BMP581_SPI() = default;

	int init();

	uint8_t get_reg(uint8_t addr);
	int get_reg_buf(uint8_t addr, uint8_t *buf, uint8_t len);
	int set_reg(uint8_t value, uint8_t addr);

	uint32_t get_device_id() const override { return device::SPI::get_device_id(); }

	uint8_t get_device_address() const override { return device::SPI::get_device_address(); }
};

IBMP581 *bmp581_spi_interface(uint8_t busnum, uint32_t device, int bus_frequency, spi_mode_e spi_mode)
{
	return new BMP581_SPI(busnum, device, bus_frequency, spi_mode);
}

BMP581_SPI::BMP581_SPI(uint8_t bus, uint32_t device, int bus_frequency, spi_mode_e spi_mode) :
	SPI(DRV_BARO_DEVTYPE_BMP581, MODULE_NAME, bus, device, spi_mode, bus_frequency)
{
}

int BMP581_SPI::init()
{
	return SPI::init();
}

uint8_t BMP581_SPI::get_reg(uint8_t addr)
{
	uint8_t cmd[2] = { (uint8_t)(addr | DIR_READ), 0}; //set MSB bit
	transfer(&cmd[0], &cmd[0], 2);
	return cmd[1];
}

int BMP581_SPI::get_reg_buf(uint8_t addr, uint8_t *buf, uint8_t len)
{
	uint8_t cmd[len + 1] = {(uint8_t)(addr | DIR_READ)};
	int ret;

	ret = transfer(&cmd[0], &cmd[0], (len + 1));
	memcpy(buf, &cmd[1], len);

	return ret;
}

int BMP581_SPI::set_reg(uint8_t value, uint8_t addr)
{
	uint8_t cmd[2] = { (uint8_t)(addr & DIR_WRITE), value}; //clear MSB bit
	return transfer(&cmd[0], nullptr, 2);
}
