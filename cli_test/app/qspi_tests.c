/*
 * qspi_tests.c
 *
 *  Created on: Apr 12, 2021
 *      Author: gregmartin
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include "libs/cli/include/cli.h"
#include "libs/utils/include/dbg_uart.h"
#include "drivers/include/udma_qspi_driver.h"

static void qspi_read(const struct cli_cmd_entry *pEntry);
static void qspi_write(const struct cli_cmd_entry *pEntry);

// EFPGA menu
const struct cli_cmd_entry qspi_cli_tests[] =
{
  CLI_CMD_SIMPLE( "read", qspi_read, "qspi read" ),
  CLI_CMD_WITH_ARG( "write", qspi_write, 0, "qspi write" ),
  CLI_CMD_TERMINATE()
};




static void qspi_read(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		int errors = 0;
		int i;
		char read_data[8] = {0xff,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80};
		message  = pvPortMalloc(80);
		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		sprintf(message,"Qspi Read tests");

		dbg_str(message);
		udma_qspim_read(0, 0, 4, read_data);
		sprintf(message,"Read data = %d",read_data[0]);
		dbg_str(message);
		sprintf(message," %d",read_data[1]);
		dbg_str(message);
		sprintf(message," %d",read_data[2]);
		dbg_str(message);
		sprintf(message," %d\r\n",read_data[3]);
		dbg_str(message);
		vPortFree(message);
}

static void qspi_write(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		int errors = 0;
		int i, length;
		CLI_uint32_required( "length", &length );
		message  = pvPortMalloc(80);
		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		sprintf(message,"Qspi Write tests");
		dbg_str(message);
		message[0] = 0x01;
		message[1] = 0x03;
		message[2] = 0x07;
		message[3] = 0x0f;
		message[4] = 0x1f;
		message[5] = 0x3f;
		message[6] = 0x7f;
		message[7] = 0xff;
		message[8] = 0xfe;
		message[9] = 0xfc;
		message[10] = 0xf8;
		message[11] = 0xf0;
		message[12] = 0xe0;
		message[13] = 0xc0;
		message[14] = 0x80;
		message[15] = 0x00;
		udma_qspim_write(0, 0, length, message);
		udma_qspim_write(0, 1, length, message);
		udma_qspim_write(0, 2, length, message);
		udma_qspim_write(0, 3, length, message);
		vPortFree(message);
}
