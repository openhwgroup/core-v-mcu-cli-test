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
#include "hal/include/hal_udma_qspi_reg_defs.h"

#include <drivers/include/udma_qspi_driver.h>




SemaphoreHandle_t  qspim_semaphores_rx[N_QSPIM];
SemaphoreHandle_t  qspim_semaphores_tx[N_QSPIM];
SemaphoreHandle_t  qspim_semaphores_cmd[N_QSPIM];
SemaphoreHandle_t  qspim_semaphores_eot[N_QSPIM];

static uint8_t aucclkdiv;
static unsigned int isr_count;
void spi_tx_isr() {
	isr_count += 0x1;
}
void spi_rx_isr() {
	isr_count += 0x100;
}
void spi_cmd_isr() {
	isr_count += 0x10000;
}
void spi_eot_isr() {
	isr_count += 0x1000000;
}
uint16_t udma_qspim_open (uint8_t qspim_id, uint32_t clk_freq) {
	volatile UdmaCtrl_t*		pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;
	UdmaQspi_t*					pqspim_regs = (UdmaQspi_t*)(UDMA_CH_ADDR_QSPIM + qspim_id * UDMA_CH_SIZE);
	uint32_t					clk_divisor;

	/* See if already initialized */
	if (qspim_semaphores_rx[qspim_id] != NULL || qspim_semaphores_tx[qspim_id] != NULL) {
		return 1;
	}
	/* Enable reset and enable uart clock */
	pudma_ctrl->reg_rst |= (UDMA_CTRL_QSPIM0_CLKEN << qspim_id);
	pudma_ctrl->reg_rst &= ~(UDMA_CTRL_QSPIM0_CLKEN << qspim_id);
	pudma_ctrl->reg_cg |= (UDMA_CTRL_QSPIM0_CLKEN << qspim_id);

	/* Set semaphore */
	SemaphoreHandle_t shSemaphoreHandle;		// FreeRTOS.h has a define for xSemaphoreHandle, so can't use that
	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	qspim_semaphores_rx[qspim_id] = shSemaphoreHandle;

	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	qspim_semaphores_tx[qspim_id] = shSemaphoreHandle;

	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	qspim_semaphores_cmd[qspim_id] = shSemaphoreHandle;

	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	qspim_semaphores_eot[qspim_id] = shSemaphoreHandle;

	/* Set handlers. */
	isr_count = 0;
	pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_RX(qspim_id), spi_rx_isr, qspim_semaphores_rx[qspim_id]);
	pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_TX(qspim_id), spi_tx_isr, qspim_semaphores_tx[qspim_id]);
	pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_CMD(qspim_id), spi_cmd_isr, qspim_semaphores_cmd[qspim_id]);
	pi_fc_event_handler_set(SOC_EVENT_UDMA_SPIM_EOT(qspim_id), spi_eot_isr, qspim_semaphores_eot[qspim_id]);

	/* Enable SOC events propagation to FC. */
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_RX(qspim_id));
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_TX(qspim_id));
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_CMD(qspim_id));
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_SPIM_EOT(qspim_id));

	/* configure */
	aucclkdiv = 5000000/clk_freq;

	return 0;
}

uint16_t udma_qspim_control(uint8_t qspim_id, udma_qspim_control_type_t control_type, void* pparam) {
	volatile UdmaCtrl_t*		pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;
	UdmaQspi_t*					pqspim_regs = (UdmaQspi_t*)(UDMA_CH_ADDR_QSPIM + qspim_id * UDMA_CH_SIZE);

	switch(control_type) {
	case kQSPImReset:
		pudma_ctrl->reg_rst |= (UDMA_CTRL_QSPIM0_CLKEN << qspim_id);
		pudma_ctrl->reg_rst &= ~(UDMA_CTRL_QSPIM0_CLKEN << qspim_id);
		break;
	default:
		configASSERT(0);
	}
	return 0;
}

static uint32_t auccmd[16];

void udma_qspim_read(uint8_t qspim_id, uint8_t cs, uint16_t read_len, uint8_t* read_buffer) {
	UdmaQspi_t*	pqspim_regs = (UdmaQspi_t*)(UDMA_CH_ADDR_QSPIM + qspim_id * UDMA_CH_SIZE);
	uint32_t*	pcmd = auccmd;

		configASSERT(read_len < 256);

		SemaphoreHandle_t shSemaphoreHandle = qspim_semaphores_rx[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		shSemaphoreHandle = qspim_semaphores_cmd[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		shSemaphoreHandle = qspim_semaphores_eot[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );

		pqspim_regs->rx_cfg_b.en = 0;
		pqspim_regs->tx_cfg_b.en = 0;
		pqspim_regs->cmd_cfg_b.en = 0;

		*pcmd++ = kSPIm_Cfg | aucclkdiv;
		*pcmd++ = kSPIm_SOT | cs;  //cs 1
		*pcmd++ = kSPIm_RxData | (0x00470000 | (read_len-1)) ; // 4 words recieved
		*pcmd++ = kSPIm_EOT  | 1; // generate event

		pqspim_regs->rx_saddr = read_buffer;
		pqspim_regs->rx_size = read_len;
		pqspim_regs->rx_cfg_b.en = 1;

		pqspim_regs->cmd_saddr = auccmd;
		pqspim_regs->cmd_size = (uint32_t)(pcmd - auccmd)*4;
		pqspim_regs->cmd_cfg_b.en = 1;

		// Block until UDMA operation is complete
		shSemaphoreHandle = qspim_semaphores_rx[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );
		shSemaphoreHandle = qspim_semaphores_cmd[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );
}


void udma_qspim_write (uint8_t qspim_id, uint8_t cs, uint16_t write_len, uint8_t *write_data) {
	UdmaQspi_t*	pqspim_regs = (UdmaQspi_t*)(UDMA_CH_ADDR_QSPIM + qspim_id * UDMA_CH_SIZE);
	uint32_t*	pcmd = auccmd;
	uint32_t tmp_size;
		configASSERT(write_len < 256);

		SemaphoreHandle_t shSemaphoreHandle = qspim_semaphores_tx[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		shSemaphoreHandle = qspim_semaphores_cmd[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		shSemaphoreHandle = qspim_semaphores_eot[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );

		pqspim_regs->rx_cfg_b.clr = 1;
		pqspim_regs->tx_cfg_b.clr = 1;
		pqspim_regs->cmd_cfg_b.clr = 1;

		*pcmd++ = kSPIm_Cfg | aucclkdiv;
		*pcmd++ = kSPIm_SOT | cs;
		*pcmd++ = kSPIm_TxData | 0x0470000 | write_len -1;
		*pcmd++ = kSPIm_EOT | 1; // generate event


		pqspim_regs->tx_saddr = write_data;
		pqspim_regs->tx_size = write_len-1;
		pqspim_regs->tx_cfg_b.datasize = 2;
		pqspim_regs->tx_cfg_b.en = 1;

		pqspim_regs->cmd_saddr = auccmd;
		pqspim_regs->cmd_size = (uint32_t)(pcmd - auccmd)*4;
		pqspim_regs->cmd_cfg_b.en = 1;





		// Block until UDMA operation is complete
		shSemaphoreHandle = qspim_semaphores_tx[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );
		shSemaphoreHandle = qspim_semaphores_cmd[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );
		shSemaphoreHandle = qspim_semaphores_eot[qspim_id];
		configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
		configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );
}

