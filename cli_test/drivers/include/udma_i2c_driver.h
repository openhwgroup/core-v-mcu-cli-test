/*
 * Copyright 2021 QuickLogic
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef DRIVERS_INCLUDE_UDMA_I2C_DRIVER_H_
#define DRIVERS_INCLUDE_UDMA_I2C_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#include "hal/include/hal_udma_ctrl_reg_defs.h"

typedef enum {
		kDataValid
} udma_i2c_control_type_t;

typedef enum {
	kI2cCmdStart 	= 0x00,
	kI2cCmdStop		= 0x20,
	kI2cCmdRdAck	= 0x40,
	kI2cCmdRdNack	= 0x60,
	kI2cCmdWr		= 0x80,
	kI2cCmdWait		= 0xA0,
	kI2cCmdRpt		= 0xC0,
	kI2cCmdCfg		= 0xE0,
	kI2cCmdWaitEvt	= 0x10,
} i2c_cmd_t;

uint16_t udma_i2c_open (uint8_t i2c_id, uint32_t i2c_clk_freq);
uint16_t udma_i2c_close (uint8_t i2c_id);
void udma_i2c_write(uint8_t i2c_id, uint8_t i2c_addr, uint8_t reg_addr, uint16_t write_len, uint8_t* write_data, bool more_follows);
void udma_i2c_read(uint8_t i2c_id, uint8_t i2c_addr, uint8_t reg_addr, uint16_t read_len, uint8_t* read_buffer, bool more_follows);


// helper functions
void _udma_i2c_write_addr_plus_regaddr (uint8_t i2c_id, uint8_t i2c_addr, uint8_t reg_addr);
void _udma_i2c_read(uint8_t i2c_id, uint8_t i2c_addr, uint16_t read_len, uint8_t* read_buffer, bool more_follows);
void _udma_i2c_send_stop(uint8_t i2c_id);

#endif /* DRIVERS_INCLUDE_UDMA_I2C_DRIVER_H_ */
