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
#include "hal/include/hal_fc_event.h"


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

static void gpioISR() {

}
static int open(const struct cli_cmd_entry *pEntry) {

// Add functionality here
char *message;
int errors = 0;
int i, length;
message  = pvPortMalloc(80);
hal_setpinmux(9,2);
hal_set_gpio_mode(2,1);
hal_set_gpio_interrupt(2,1,1);
pi_fc_event_handler_set(130, gpioISR, NULL);
/* Enable SOC events propagation to FC. */
hal_soc_eu_set_fc_mask(130);
hal_toggle_gpio(2);
hal_toggle_gpio(2);
//cam_open(0);
sprintf(message,"Himax opened--ID = ");
dbg_str(message);
//sprintf(message,"%04x\r\n",udma_cam_control(kCamID, NULL));
//dbg_str(message);
//udma_cam_control(kCamInit, NULL);
vPortFree(message);
}
