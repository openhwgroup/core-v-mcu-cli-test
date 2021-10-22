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
#include "hal/include/hal_udma_sdio_reg_defs.h"
#include <drivers/include/udma_sdio_driver.h>


uint16_t udma_sdio_open (uint8_t sdio_id)
{
	int i = 0;
	volatile UdmaCtrl_t*		pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;
	UdmaSdio_t*					psdio_regs = (UdmaSdio_t*)(UDMA_CH_ADDR_SDIO + sdio_id * UDMA_CH_SIZE);

	/* Enable reset and enable uart clock */
	pudma_ctrl->reg_rst |= (UDMA_CTRL_SDIO0_CLKEN << sdio_id);
	pudma_ctrl->reg_rst &= ~(UDMA_CTRL_SDIO0_CLKEN << sdio_id);
	pudma_ctrl->reg_cg |= (UDMA_CTRL_SDIO0_CLKEN << sdio_id);

	psdio_regs->clk_div_b.clk_div = 5;
	psdio_regs->clk_div_b.valid = 1;
	//Restore pin muxes
	for(i=0; i<6; i++ )
	{
		//set pin muxes to sdio functionality
		 hal_setpinmux(37+i, 0);
	}
	return 0;
}

uint16_t udma_sdio_control(uint8_t sdio_id, udma_sdio_control_type_t control_type, void* pparam) {
	volatile UdmaCtrl_t*		pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;
	UdmaSdio_t*					psdio_regs = (UdmaSdio_t*)(UDMA_CH_ADDR_SDIO + sdio_id * UDMA_CH_SIZE);

	switch(control_type) {
	case kSDIOmReset:
		pudma_ctrl->reg_rst |= (UDMA_CTRL_SDIO0_CLKEN << sdio_id);
		pudma_ctrl->reg_rst &= ~(UDMA_CTRL_SDIO0_CLKEN << sdio_id);
		break;
	default:
		configASSERT(0);
	}
	return 0;
}

uint8_t udma_sdio_sendCmd(uint8_t sdio_id, uint8_t aCmdOpCode, uint8_t aRspType, uint32_t aCmdArgument, uint32_t *aResponseBuf)
{
	uint8_t lSts = 0;
	UdmaSdio_t *psdio_regs = (UdmaSdio_t*)(UDMA_CH_ADDR_SDIO + sdio_id * UDMA_CH_SIZE);

	psdio_regs->cmd_op_b.cmd_op = ( aCmdOpCode & 0x3F );
	psdio_regs->cmd_op_b.cmd_rsp_type = ( aRspType & 0x07 );
	psdio_regs->cmd_arg_b.cmd_arg = aCmdArgument;

	psdio_regs->start_b.start = 1;

	while( psdio_regs->status_b.eot == 0 );

	psdio_regs->status_b.eot = 1;	//Write 1 to EOT bit to clear it.

	if( psdio_regs->status_b.error == 1 )
	{
		lSts = psdio_regs->status_b.cmd_err_status;
	}

	if( aResponseBuf )
	{
		aResponseBuf[0] = psdio_regs->rsp0;
		aResponseBuf[1] = psdio_regs->rsp1;
		aResponseBuf[2] = psdio_regs->rsp2;
		aResponseBuf[3] = psdio_regs->rsp3;

	}
	return lSts;
}
#if 0
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

#endif
