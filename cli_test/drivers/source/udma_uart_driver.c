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

#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include <string.h>
#include "hal/include/hal_fc_event.h"
#include "hal/include/hal_udma_ctrl_reg_defs.h"
#include "hal/include/hal_udma_uart_reg_defs.h"

#include <stdint.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include "drivers/include/udma_uart_driver.h"

SemaphoreHandle_t  uart_semaphores_rx[N_UART];
SemaphoreHandle_t  uart_semaphores_tx[N_UART];

uint16_t udma_uart_open (uint8_t uart_id, uint32_t xbaudrate) {
	UdmaUart_t*				puart;
	volatile UdmaCtrl_t*		pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;

	/* See if already initialized */
	if (uart_semaphores_rx[uart_id] != NULL || uart_semaphores_tx[uart_id] != NULL) {
		return 1;
	}
	/* Enable reset and enable uart clock */
	pudma_ctrl->reg_rst |= (UDMA_CTRL_UART0_CLKEN << uart_id);
	pudma_ctrl->reg_cg |= (UDMA_CTRL_UART0_CLKEN << uart_id);

	/* Set semaphore */
	SemaphoreHandle_t shSemaphoreHandle;		// FreeRTOS.h has a define for xSemaphoreHandle, so can't use that
	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	uart_semaphores_rx[uart_id] = shSemaphoreHandle;

	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	uart_semaphores_tx[uart_id] = shSemaphoreHandle;

	/* Set handlers. */
	pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_RX(uart_id), NULL, uart_semaphores_rx[uart_id]);
	pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_TX(uart_id), NULL, uart_semaphores_tx[uart_id]);
	/* Enable SOC events propagation to FC. */
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_RX(uart_id));
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_TX(uart_id));

	/* configure */
	puart = (UdmaUart_t*)(UDMA_CH_ADDR_UART + uart_id * UDMA_CH_SIZE);
	puart->uart_setup_b.div = (uint16_t)(5000000/xbaudrate);
	puart->uart_setup_b.bits = 3; // 8-bits
	puart->uart_setup_b.rx_polling_en = 1;
	puart->uart_setup_b.en_tx = 1;
	puart->uart_setup_b.en_rx = 1;
	
	return 0;
}

uint16_t udma_uart_writeraw(uint8_t uart_id, uint16_t write_len, uint8_t* write_buffer) {
	UdmaUart_t*				puart = (UdmaUart_t*)(UDMA_CH_ADDR_UART + uart_id * UDMA_CH_SIZE);

	SemaphoreHandle_t shSemaphoreHandle = uart_semaphores_tx[uart_id];
	if( xSemaphoreTake( shSemaphoreHandle, 1000000 ) != pdTRUE ) {
		return 1;
	}

	while (puart->status_b.tx_busy) {  // ToDo: Why is this necessary?  Thought the semaphore should have protected
	}

	puart->tx_saddr = (uint32_t)write_buffer;
	puart->tx_size = write_len;
	puart->tx_cfg_b.en = 1; //enable the transfer

	return 0;
}

uint16_t udma_uart_read(uint8_t uart_id, uint16_t read_len, uint8_t* read_buffer) {
	uint16_t ret = 0;
	uint8_t last_char = 0;
	UdmaUart_t*				puart = (UdmaUart_t*)(UDMA_CH_ADDR_UART + uart_id * UDMA_CH_SIZE);


	while ( (ret < (read_len - 2)) && (last_char != 0xd)) {
		if (puart->valid_b.rx_data_valid == 1) {
			last_char = (uint8_t)(puart->data_b.rx_data & 0xff);
			if (last_char == 0xd)  // if cr add
				read_buffer[ret++] = 0xa;  // linefeed
			read_buffer[ret++] = last_char;
		}
	}
	read_buffer[ret] = '\0';
	return ret--;
}

uint16_t udma_uart_readraw(uint8_t uart_id, uint16_t read_len, uint8_t* read_buffer) {
	uint16_t ret = 0;
	uint8_t last_char = 0;
	UdmaUart_t*				puart = (UdmaUart_t*)(UDMA_CH_ADDR_UART + uart_id * UDMA_CH_SIZE);

	while ( ret < read_len ) {
		if (puart->valid_b.rx_data_valid == 1) {
			last_char = (uint8_t)(puart->data_b.rx_data & 0xff);
			read_buffer[ret++] = last_char;
		}
	}
	return ret--;
}

uint16_t udma_uart_getchar(uint8_t uart_id) {
	UdmaUart_t*				puart = (UdmaUart_t*)(UDMA_CH_ADDR_UART + uart_id * UDMA_CH_SIZE);

	while (puart->valid_b.rx_data_valid == 0) {
	}
	return (puart->data_b.rx_data & 0xff);
}

uint16_t udma_uart_control(uint8_t uart_id, udma_uart_control_type_t control_type, void* pparam) {
	UdmaUart_t*				puart = (UdmaUart_t*)(UDMA_CH_ADDR_UART + uart_id * UDMA_CH_SIZE);

	switch(control_type) {
	case kDataValid:
		return puart->valid_b.rx_data_valid;
	default:
		return 0xFFFF;
	}
	return 0;
}
