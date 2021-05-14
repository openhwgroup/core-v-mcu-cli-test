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
#include "hal/include/hal_udma_cam_reg_defs.h"

#include <drivers/include/udma_cam_driver.h>
#include <drivers/include/udma_i2cm_driver.h>

#include "drivers/include/himax.h"

SemaphoreHandle_t  cam_semaphore_rx;
static uint8_t cam;

void cam_open (uint8_t cam_id) {
	volatile UdmaCtrl_t*		pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;
	UdmaCamera_t*					pcam_regs = (UdmaCamera_t*)(UDMA_CH_ADDR_CAM);
uint8_t i2c_buffer[8];

	/* See if already initialized */
	if (cam_semaphore_rx != NULL ){
		return 1;
	}
	/* Enable reset and enable uart clock */
	pudma_ctrl->reg_cg |= (UDMA_CTRL_CAM0_CLKEN);

	/* Set semaphore */
	SemaphoreHandle_t shSemaphoreHandle;		// FreeRTOS.h has a define for xSemaphoreHandle, so can't use that
	shSemaphoreHandle = xSemaphoreCreateBinary();
	configASSERT(shSemaphoreHandle);
	xSemaphoreGive(shSemaphoreHandle);
	cam_semaphore_rx = shSemaphoreHandle;


	/* Set handlers. */
	pi_fc_event_handler_set(SOC_EVENT_UDMA_CAM_RX(0), NULL, cam_semaphore_rx);
	/* Enable SOC events propagation to FC. */
	hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_CAM_RX(cam_id));

	/* configure */
	cam = 0x48; // Himax address
	udma_cam_control(kCamReset, NULL);

	return 0;
}
uint16_t udma_cam_control(udma_cam_control_type_t control_type, void* pparam) {
	switch (control_type) {
	case kCamReset:
		_himaxRegWrite(SW_RESET, HIMAX_RESET);
	}


}

void _himaxRegWrite(unsigned int addr, unsigned char value){
	uint8_t naddr;
	uint16_t data;
	naddr = (addr>>8) & 0xff;
	data = ((addr & 0xff) << 8) + value;
	udma_i2cm_write (0, cam, naddr, 2, &data, 0);
   //     i2c_16write8(cam,addr,value);
}
