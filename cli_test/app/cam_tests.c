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
#include "include/estruct.h"


static int open(const struct cli_cmd_entry *pEntry);
static int getframe(const struct cli_cmd_entry *pEntry);
static int set_clock(const struct cli_cmd_entry *pEntry);

// cam menu
const struct cli_cmd_entry cam_tests[] =
{
  CLI_CMD_SIMPLE( "open", open, "Initialize Himax Sensor" ),
  CLI_CMD_WITH_ARG( "get", getframe, 0, "get image frames " ),
  CLI_CMD_WITH_ARG( "setclock", set_clock, 0, "set external clock for camera" ),
  CLI_CMD_TERMINATE()
};
static uint8_t picture[244*324];
static int getframe(const struct cli_cmd_entry *pEntry) {
	int i, j;
	char *message;
	message  = pvPortMalloc(80);
	for (i = 0; i < 244; i++)
		for (j = 0; j< 324; j++)
			picture[i*324+j] = 0;
	udma_cam_control(kCamFrame, picture);
	for (i = 0; i < 244; i++){
		sprintf(message,"\nrow[%3d]:",i);
		dbg_str(message);
			for (j = 0; j< 324; j++) {
				sprintf(message," %02x",picture[i*324+j]);
				dbg_str(message);
			}
	}
	vPortFree(message);
}

static int set_clock(const struct cli_cmd_entry *pEntry) {
	apb_adv_timer_typedef *adv_timer;
	unsigned int retval;
	adv_timer = (apb_adv_timer_typedef*) APB_ADV_TIMER_BASE_ADDR;
	adv_timer->T0_CMD = 0x8; // reset
	adv_timer->T0_CONFIG = 0; // FLL, up/done no prescaler
	adv_timer->T0_THRESHOLD = 0x20000;
	adv_timer->T0_TH_CH0 = 0x30001;
	adv_timer->CG = 0x1; // enable clock for timer0
	adv_timer->T0_CMD = 1; //start


	return retval;
}

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
hal_setpinmux(25, 3); //gpio28 -- timer0_ch0
hal_setpinmux(26, 3); // cam v-sync
hal_setpinmux(27, 3); // cam h-sync
hal_setpinmux(28, 3); // cam clk
hal_setpinmux(32, 3); // cam data0
hal_setpinmux(33, 3); // cam data1
hal_setpinmux(34, 3); // cam data2
hal_setpinmux(35, 3); // cam data3
hal_setpinmux(36, 3); // cam data4
hal_setpinmux(37, 3); // cam data5
hal_setpinmux(38, 3); // cam data6
hal_setpinmux(39, 3); // cam data7
cam_open(0);
sprintf(message,"Himax opened--ID = ");
dbg_str(message);
sprintf(message,"%04x\r\n",udma_cam_control(kCamID, NULL));
dbg_str(message);
udma_cam_control(kCamInit, NULL);
vPortFree(message);
}
