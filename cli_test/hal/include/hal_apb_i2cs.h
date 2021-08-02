/*
 * This is a generated file
 * 
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

#ifndef __HAL_APB_I2CS_H__
#define __HAL_APB_I2CS_H__

#include "stdint.h"

uint8_t hal_get_apb_i2cs_slave_address(void);
void hal_set_apb_i2cs_slave_address(uint8_t aSlaveAddress);
uint8_t hal_set_apb_i2cs_slave_on_off(uint8_t aStatus);
uint8_t hal_get_i2cs_fifo_i2c_apb_read_data_port(void);
void hal_set_i2cs_fifo_apb_i2c_write_data_port(uint8_t aData);
uint8_t hal_get_i2cs_fifo_i2c_apb_write_flags(void);
uint8_t hal_get_i2cs_fifo_i2c_apb_read_flags(void);
uint8_t hal_get_i2cs_fifo_apb_i2c_read_flags(void);
uint8_t hal_get_i2cs_fifo_apb_i2c_write_flags(void);
uint8_t hal_get_i2cs_msg_i2c_apb(void);
uint8_t hal_get_i2cs_msg_i2c_apb_status(void);
void hal_set_i2cs_msg_apb_i2c(uint8_t aData);
uint8_t hal_get_i2cs_msg_apb_i2c_status(void);
uint8_t hal_i2cs_fifo_i2c_apb_flush(void);



#endif // __HAL_APB_I2CS_H__
