/*
 * cam_tests.c
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
#include "drivers/include/udma_cam_driver.h"
#include "hal/include/hal_pinmux.h"

static int open(const struct cli_cmd_entry *pEntry);
static int getframe(const struct cli_cmd_entry *pEntry);

// cam menu
const struct cli_cmd_entry cam_tests[] =
{
  CLI_CMD_SIMPLE( "open", open, "Initialize Himax Sensor" ),
  CLI_CMD_WITH_ARG( "get", getframe, 0, "get image frames " ),
  CLI_CMD_TERMINATE()
};

static int getframe(const struct cli_cmd_entry *pEntry) {}

static int open(const struct cli_cmd_entry *pEntry) {

// Add functionality here
char *message;
int errors = 0;
int i, length;
message  = pvPortMalloc(80);
hal_setpinmux(13, 1);
hal_setpinmux(14, 1);
hal_setpinmux(21, 1);
hal_setpinmux(22, 1);
cam_open(0);
sprintf(message,"Himax open-- stubbed out\r\n");
dbg_str(message);

vPortFree(message);
}
