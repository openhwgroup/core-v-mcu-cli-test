/*
 * sdio_tests.c
 *
 *  Created on: 20-Oct-2021
 *      Author: somesh
 */

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

#include "hal/include/hal_fc_event.h"
#include "hal/include/hal_pinmux.h"
#include "drivers/include/udma_sdio_driver.h"

static void sdio_init(const struct cli_cmd_entry *pEntry);
static void sdio_write(const struct cli_cmd_entry *pEntry);
static void sdio_test (const struct cli_cmd_entry *pEntry);


// EFPGA menu
const struct cli_cmd_entry sdio_cli_tests[] =
{
  CLI_CMD_SIMPLE( "init", sdio_init, "sdio init" ),
  CLI_CMD_WITH_ARG( "write", sdio_write, 0, "sdio write" ),
  CLI_CMD_WITH_ARG( "test", sdio_test, 0, "read flash id" ),
  CLI_CMD_TERMINATE()
};

static void sdio_init(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	udma_sdio_open(0);
	//CLI_printf("[0x%02x]/[0x%02x]/[0x%02x]/[0x%02x]\n",lRead_data[0], lRead_data[1], lRead_data[2], lRead_data[3]);

}


static void sdio_write(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here

}

static void sdio_test(const struct cli_cmd_entry *pEntry)
{
	uint8_t lSts = 0;
	(void)pEntry;
	// Add functionality here
	lSts = udma_sdio_sendCmd(0, 0x40, 0, 0, NULL);
	CLI_printf("DONE 0x%02x\n", lSts);
}

