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

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "semphr.h"

#include "target/core-v-mcu/include/core-v-mcu-config.h"

#include "hal/include/hal_fc_event.h"
#include "hal/include/hal_udma_ctrl_reg_defs.h"
#include "hal/include/hal_udma_i2c_reg_defs.h"

#include <drivers/include/udma_i2cm_driver.h>




SemaphoreHandle_t  i2cm_semaphores_rx[N_I2CM];
SemaphoreHandle_t  i2cm_semaphores_tx[N_I2CM];

static uint8_t aucclkdiv[2];

uint16_t udma_i2cm_open (uint8_t i2cm_id, uint32_t clk_freq) {
	volatile UdmaCtrl_t*		pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;
	UdmaI2c_t*					pi2cm_regs = (UdmaI2c_t*)(UDMA_CH_ADDR_I2CM + i2cm_id * UDMA_CH_SIZE);
	uint32_t					clk_divisor;

	/* See if already initialized */
	if (i2cm_semaphores_rx[i2cm_id] != NULL || i2cm_semaphores_tx[i2cm_id] != NULL) {
		return 1;
	}
	/* Enable reset and enable uart clock */
	pudma_ctrl->reg_rst |= (UDMA_CTRL_I2CM0_CLKEN << i2cm_id);
	pudma_ctrl->reg_rst &= ~(UDMA_CTRL_I2CM0_CLKEN << i2cm_id);
	pudma_ctrl->reg_cg |= (UDMA_CTRL_I2CM0_CLKEN << i2cm_id);

	/* Set semaphore */
	SemaphoreHandle_t shSemaphoreHandle;		// FreeRTOS.h has a define for xSemaphoreHandle, so can't use that
	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	i2cm_semaphores_rx[i2cm_id] = shSemaphoreHandle;

	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	i2cm_semaphores_tx[i2cm_id] = shSemaphoreHandle;

	/* Set handlers. */
	pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_RX(i2cm_id), NULL, i2cm_semaphores_rx[i2cm_id]);
	pi_fc_event_handler_set(SOC_EVENT_UDMA_I2C_TX(i2cm_id), NULL, i2cm_semaphores_tx[i2cm_id]);
	/* Enable SOC events propagation to FC. */
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_RX(i2cm_id));
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_I2C_TX(i2cm_id));

	/* configure */
	clk_divisor = 5000000/clk_freq;
	aucclkdiv[0] = (clk_divisor >> 0) & 0xFF;
	aucclkdiv[1] = (clk_divisor >> 8) & 0xFF;

	return 0;
}

uint16_t udma_i2cm_control(uint8_t i2cm_id, udma_i2cm_control_type_t control_type, void* pparam) {
	volatile UdmaCtrl_t*		pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;
	UdmaI2c_t*					pi2cm_regs = (UdmaI2c_t*)(UDMA_CH_ADDR_I2CM + i2cm_id * UDMA_CH_SIZE);

	switch(control_type) {
	case kI2cmReset:
		pudma_ctrl->reg_rst |= (UDMA_CTRL_I2CM0_CLKEN << i2cm_id);
		pudma_ctrl->reg_rst &= ~(UDMA_CTRL_I2CM0_CLKEN << i2cm_id);
		break;
	default:
		configASSERT(0);
	}
	return 0;
}

static uint8_t auccmd_rx[16];

void udma_i2cm_read(uint8_t i2cm_id, uint8_t i2cm_addr, uint8_t reg_addr, uint16_t read_len, uint8_t* read_buffer, bool more_follows) {
	_udma_i2cm_write_addr_plus_regaddr(i2cm_id, i2cm_addr, reg_addr);
	_udma_i2cm_read(i2cm_id, i2cm_addr, read_len, read_buffer, more_follows);
}

static uint8_t auccmd_tx[16];
void udma_i2cm_write (uint8_t i2cm_id, uint8_t i2cm_addr, uint8_t reg_addr, uint16_t write_len, uint8_t *write_data,  bool more_follows) {
	UdmaI2c_t*					pi2cm_regs = (UdmaI2c_t*)(UDMA_CH_ADDR_I2CM + i2cm_id * UDMA_CH_SIZE);
	uint8_t*					pcmd = auccmd_tx;

	configASSERT(write_len < 256);

	SemaphoreHandle_t shSemaphoreHandle = i2cm_semaphores_rx[i2cm_id];
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) != pdTRUE );
	shSemaphoreHandle = i2cm_semaphores_tx[i2cm_id];
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) != pdTRUE );

	_udma_i2cm_write_addr_plus_regaddr (i2cm_id, i2cm_addr, reg_addr);
	configASSERT(xSemaphoreTake( shSemaphoreHandle, 1000000 ) != pdTRUE ); // Wait for any prior transmission to complete

	*pcmd++ = kI2cmCmdCfg;
	*pcmd++ = aucclkdiv[1];
	*pcmd++ = aucclkdiv[0];
	*pcmd++ = kI2cmCmdStart;		// Put Start transaction on I2C bus
	*pcmd++ = kI2cmCmdWr;		// Write device's address (next byte)
	*pcmd++ = i2cm_addr & 0xfe; 	// Clear R/WRbar bit from i2c device's address to indicate write
	if (write_len > 0) {
		*pcmd++ = 0xC0; 		// i2cm_CMD_RPT
		*pcmd++ = (uint8_t)write_len;
	}
	*pcmd++ = kI2cmCmdWr; 		// I2C CMD_WR
	pi2cm_regs->tx_saddr = auccmd_tx;
	pi2cm_regs->tx_size = (uint32_t)(pcmd - auccmd_tx);
	pi2cm_regs->tx_cfg_b.en = 1;

	configASSERT(xSemaphoreTake( shSemaphoreHandle, 1000000 ) != pdTRUE );	// Wait for this trans to complete

	if (!more_follows) {
		_udma_i2cm_send_stop(i2cm_id);
	}
}

void _udma_i2cm_write_addr_plus_regaddr (uint8_t i2cm_id, uint8_t i2cm_addr, uint8_t reg_addr) {
	UdmaI2c_t*					pi2cm_regs = (UdmaI2c_t*)(UDMA_CH_ADDR_I2CM + i2cm_id * UDMA_CH_SIZE);
	uint8_t*					pcmd = auccmd_tx;

	SemaphoreHandle_t shSemaphoreHandle = i2cm_semaphores_tx[i2cm_id];
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );

	pi2cm_regs->tx_cfg_b.en = 0;
	*pcmd++ = kI2cmCmdCfg;
	*pcmd++ = aucclkdiv[1];
	*pcmd++ = aucclkdiv[0];
	*pcmd++ = kI2cmCmdStart;		// Put Start transaction on I2C bus
	*pcmd++ = kI2cmCmdWr;		// Write device's address (next byte)
	*pcmd++ = i2cm_addr & 0xfe; 	// Clear R/WRbar bit from i2c device's address to indicate write
	*pcmd++ = kI2cmCmdWr; 		// I2C CMD_WR
	pi2cm_regs->tx_saddr = auccmd_tx;
	pi2cm_regs->tx_size = (uint32_t)(pcmd - auccmd_tx);
	pi2cm_regs->tx_cfg_b.en = 1;

	// Block until UDMA operation is completed
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );

	//pi2cm_regs->tx_cfg_b.en = 0;
	pcmd = auccmd_tx;
	*pcmd++ = reg_addr;
	pi2cm_regs->tx_saddr = auccmd_tx;
	pi2cm_regs->tx_size = (uint32_t)(pcmd - auccmd_tx);
	pi2cm_regs->tx_cfg_b.en = 1;

	// Block until UDMA operation is completed
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
	configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );

}

void _udma_i2cm_read(uint8_t i2cm_id, uint8_t i2cm_addr, uint16_t read_len, uint8_t* read_buffer, bool more_follows) {
	UdmaI2c_t*					pi2cm_regs = (UdmaI2c_t*)(UDMA_CH_ADDR_I2CM + i2cm_id * UDMA_CH_SIZE);
	uint8_t*					pcmd = auccmd_rx;

	configASSERT(read_len < 256);

	SemaphoreHandle_t shSemaphoreHandle = i2cm_semaphores_rx[i2cm_id];
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
	shSemaphoreHandle = i2cm_semaphores_tx[i2cm_id];
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );

	pi2cm_regs->tx_cfg_b.en = 0;
	*pcmd++ = kI2cmCmdCfg;
	*pcmd++ = aucclkdiv[1];
	*pcmd++ = aucclkdiv[0];
	*pcmd++ = kI2cmCmdStart;		// Put Start transaction on I2C bus
	*pcmd++ = kI2cmCmdWr;		// Write device's address (next byte)
	*pcmd++ = i2cm_addr | 0x01;	// Device's address with read bit set
	if (read_len > 1) {				// Do len-1 reads with ACK, and follow by 1 read with NACK
		*pcmd++ = kI2cmCmdRpt;		// Tell controller to repeat the following command
		*pcmd++ = (uint8_t)(read_len - 1);		// len-1 times
		*pcmd++ = kI2cmCmdRdAck;		// command to repeat is read with ack
	}
	*pcmd++ = kI2cmCmdRdNack;	// Read last byte with NACK to indicate the end of the read

	//
	pi2cm_regs->rx_saddr = read_buffer;
	pi2cm_regs->rx_size = read_len;
	pi2cm_regs->rx_cfg_b.en = 1;

	pi2cm_regs->tx_saddr = auccmd_rx;
	pi2cm_regs->tx_size = (uint32_t)(pcmd - auccmd_rx);
	pi2cm_regs->tx_cfg_b.en = 1;

	// Block until UDMA operation is complete
	shSemaphoreHandle = i2cm_semaphores_rx[i2cm_id];
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
	configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );
	shSemaphoreHandle = i2cm_semaphores_tx[i2cm_id];
	configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
	configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );

	if (!more_follows) {
		_udma_i2cm_send_stop(i2cm_id);
	}
}

static uint8_t auci2cm_stop_seq[] = {
		kI2cmCmdStop,	kI2cmCmdWait, 0xff
};

void _udma_i2cm_send_stop(uint8_t i2cm_id) {
	UdmaI2c_t*			pi2cm_regs = (UdmaI2c_t*)(UDMA_CH_ADDR_I2CM + i2cm_id * UDMA_CH_SIZE);
	SemaphoreHandle_t 	shSemaphoreHandle = i2cm_semaphores_tx[i2cm_id];
	configASSERT(xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );

	pi2cm_regs->tx_saddr = auci2cm_stop_seq;
	pi2cm_regs->tx_size = sizeof(auci2cm_stop_seq);
	pi2cm_regs->tx_cfg_b.en = 1;

	// Block until UDMA transaction is completed
	configASSERT(xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
	configASSERT(xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );
}
