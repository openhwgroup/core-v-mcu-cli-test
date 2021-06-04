/*==========================================================
 * Copyright 2021 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "semphr.h"	// Required for configASSERT

#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include "libs/cli/include/cli.h"
#include "libs/utils/include/dbg_uart.h"
#include "hal/include/hal_apb_soc_ctrl_regs.h"
#include "hal/include/hal_pinmux.h"
#include "hal/include/hal_gpio.h"
#include "hal/include/hal_fc_event.h"
#include "include/gpio_tests.h"


// EFPGAIO functions
static void efpgaio_set(const struct cli_cmd_entry *pEntry);
static void efpgaio_clr(const struct cli_cmd_entry *pEntry);
static void efpgaio_toggle(const struct cli_cmd_entry *pEntry);
static void efpgaio_get_status(const struct cli_cmd_entry *pEntry);
//static void gpio_event_test(const struct cli_cmd_entry *pEntry);
//static void apb_gpio_tests(const struct cli_cmd_entry *pEntry);
//static void apb_gpio_event_tests(const struct cli_cmd_entry *pEntry);


// EPGPAIO menu
const struct cli_cmd_entry efpgaio_functions[] =
{
		CLI_CMD_SIMPLE( "set", 	efpgaio_set,         		"gpio_num	-- set to one" ),
		CLI_CMD_SIMPLE( "clr", 	efpgaio_clr,         		"gpio_num	-- clear to zero" ),
		CLI_CMD_SIMPLE( "toggle",	efpgaio_toggle,        "gpio_num	-- toggle state of gpio" ),
		CLI_CMD_SIMPLE( "status",	efpgaio_get_status,   "gpio_num	-- read status of gpio: in, out, interrupt type and mode" ),
		//CLI_CMD_SIMPLE( "event",	gpio_event_test,       "io_num, mux_sel, gpio_num, gpio_int_type	-- set interrupt of gpio" ),
		//CLI_CMD_SIMPLE( "evnt",	efpga_io_events,        "None	-- All events of gpio" ),
		//CLI_CMD_SIMPLE( "all",	efpga_io_tests,         "None	-- All gpio tests " ),
		CLI_CMD_TERMINATE()
};


// GPIO functions
static void efpgaio_set(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	efpgaio_num;

	CLI_uint32_required( "efgpaio_num", &efpgaio_num );
	hal_efpgaio_outen((uint8_t)efpgaio_num,SET);
	hal_efpgaio_output((uint8_t)efpgaio_num,SET);
	dbg_str("<<DONE>>");
}

static void efpgaio_clr(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	efpgaio_num;

	CLI_uint32_required( "efgpaio_num", &efpgaio_num );
	hal_efpgaio_output((uint8_t)efpgaio_num, CLEAR);
	dbg_str("<<DONE>>");
}

static void efpgaio_toggle(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	efpgaio_num;

	CLI_uint32_required( "efgpaio_num", &efpgaio_num );
	hal_efpgaio_output((uint8_t)efpgaio_num,TOGGLE);
	dbg_str("<<DONE>>");
}

static void efpgaio_get_status(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	gpio_hal_typedef efpgaio;
	// Add functionality here
	uint32_t efpgaio_num;
	uint8_t	input_value;
	uint8_t	output_value;
	uint8_t	interrupt_type;
	uint8_t	gpio_mode;

	CLI_uint32_required( "gpio_num", &efpgaio_num );
	efpgaio.number = efpgaio_num;
	hal_efpgaio_status(&efpgaio);
	dbg_str_hex8("input", (uint32_t)efpgaio.in_val);
	dbg_str_hex8("output", (uint32_t)efpgaio.out_val);
	dbg_str_hex8("output_en ", (uint32_t)efpgaio.mode);
	dbg_str_hex8("event ", (uint32_t)efpgaio.int_en);
	dbg_str("<<DONE>>");
}
