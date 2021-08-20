/*
 * Copyright 2020 ETH Zurich
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
 * Author: Robert Balas (balasr@iis.ee.ethz.ch)
 */

#include <stdint.h>
#include <assert.h>

#include <FreeRTOS.h>
#include "FreeRTOSConfig.h"

#include <target/core-v-mcu/include/core_pulp_cluster.h>
#include <target/core-v-mcu/include/core-v-mcu-config.h>

//#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "hal/include/hal_fc_event.h"
/* TODO: weird include */
#include "target/core-v-mcu/include/core-v-mcu-properties.h"
#include "hal/include/hal_irq.h"
#include "hal/include/hal_soc_eu.h"
#include "hal/include/hal_apb_soc_ctrl_reg_defs.h"

#include "drivers/include/udma_uart_driver.h"
#include "drivers/include/udma_i2cm_driver.h"
#include "drivers/include/udma_qspi_driver.h"

#include "../../app/N25Q_16Mb-1Gb_Device_Driver V2.1/N25Q.h"
#include "hal/include/hal_apb_i2cs.h"

FLASH_DEVICE_OBJECT gFlashDeviceObject;

/* test some assumptions we make about compiler settings */
static_assert(sizeof(uintptr_t) == 4,
	      "uintptr_t is not 4 bytes. Make sure you are using -mabi=ilp32*");

/* Allocate heap to special section. Note that we have no references in the
 * whole program to this variable (since its just here to allocate space in the
 * section for our heap), so when using LTO it will be removed. We force it to
 * stay with the "used" attribute
 */
__attribute__((section(".heap"), used)) uint8_t ucHeap[configTOTAL_HEAP_SIZE];

/* Inform linker script about .heap section size. Note: GNU ld seems to
 * internally represent integers with the bfd_vma type, that is a type that can
 * contain memory addresses (typdefd to some int type depending on the
 * architecture). uint32_t seems to me the most fitting candidate for rv32.
 */
uint32_t __heap_size = configTOTAL_HEAP_SIZE;

volatile uint32_t system_core_clock = DEFAULT_SYSTEM_CLOCK;

/* FreeRTOS task handling */
BaseType_t xTaskIncrementTick(void);
void vTaskSwitchContext(void);

/* interrupt handling */
void timer_irq_handler(uint32_t mcause);
void undefined_handler(uint32_t mcause);
extern void fc_soc_event_handler1 (uint32_t mcause);
void (*isr_table[32])(uint32_t);

/**
 * Board init code. Always call this before anything else.
 */
int handler_count[32];
void system_init(void)
{
	SocCtrl_t *soc=APB_SOC_CTRL_ADDR;
	soc->soft_reset = 1;
	uint32_t val = 0;
	timer_irq_disable();
	/* init flls */
	for (int i = 0; i < ARCHI_NB_FLL; i++) {
		pi_fll_init(i, 0);
	}

	/* make sure irq (itc) is a good state */
//	irq_init();

	/* Hook up isr table. This table is temporary until we figure out how to
	 * do proper vectored interrupts.
	 */
for (int i = 0 ; i < 32 ; i ++){
	isr_table[i] = undefined_handler;
	handler_count[i] = 0;
}
	isr_table[0x7] = timer_irq_handler;
	isr_table[0xb] = (void(*)(uint32_t))fc_soc_event_handler1; // 11 for cv32

	/* mtvec is set in crt0.S */

	/* deactivate all soc events as they are enabled by default */
	pulp_soc_eu_event_init();

	/* Setup soc events handler. */
	//pi_fc_event_handler_init(FC_SOC_EVENT);
	pi_fc_event_handler_init(11);

	/* TODO: I$ enable*/
	/* enable core level interrupt (mie) */
	irq_clint_enable();

	val = csr_read(CSR_MIE);

	/* TODO: enable uart */
	for (uint8_t id = 0; id != N_UART; id++) {
		udma_uart_open(id, 115200);
	}
	for (uint8_t id = 0; id != N_I2CM; id++) {
		udma_i2cm_open(id, 400000);  //200000
	}
	udma_qspim_open(0,5000000);

	hal_set_apb_i2cs_slave_on_off(1);
	if( hal_get_apb_i2cs_slave_address() !=  MY_I2C_SLAVE_ADDRESS )
			hal_set_apb_i2cs_slave_address(MY_I2C_SLAVE_ADDRESS);

}

void system_core_clock_update(void)
{
	system_core_clock = pi_fll_get_frequency(FLL_SOC, 0);
}

void system_core_clock_get(void)
{
	system_core_clock_update();
	return ;
}

void timer_irq_handler(uint32_t mcause)
{
#warning requires critical section if interrupt nesting is used.
	if (xTaskIncrementTick() != 0) {
		vTaskSwitchContext();
	}
}

void undefined_handler(uint32_t mcause)
{
#ifdef __PULP_USE_LIBC
	abort();
#else
//	taskDISABLE_INTERRUPTS();
//	for(;;);
	handler_count[mcause]++;
#endif
}

void vPortSetupTimerInterrupt(void)
{
	extern int timer_irq_init(uint32_t ticks);

	/* No CLINT so use the PULP timer to generate the tick interrupt. */
	/* TODO: configKERNEL_INTERRUPT_PRIORITY - 1 ? */
	timer_irq_init(ARCHI_FPGA_FREQUENCY / configTICK_RATE_HZ);
	/* TODO: allow setting interrupt priority (to super high(?)) */
	//irq_enable(IRQ_FC_EVT_TIMER0_HI); // not needed as timer comes in irq7
//	irq_enable (IRQ_FC_EVT_SW7);  // enable MTIME
}

void vSystemIrqHandler(uint32_t mcause)
{
	uint32_t val = 0;
//	extern void (*isr_table[32])(uint32_t);
	isr_table[mcause & 0x1f](mcause & 0x1f);

}
