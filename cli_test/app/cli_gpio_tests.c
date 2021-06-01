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


// IO functions
static void io_setmux(const struct cli_cmd_entry *pEntry);
static void io_getmux(const struct cli_cmd_entry *pEntry);

// GPIO functions
static void gpio_set(const struct cli_cmd_entry *pEntry);
static void gpio_clr(const struct cli_cmd_entry *pEntry);
static void gpio_toggle(const struct cli_cmd_entry *pEntry);
static void gpio_read_status(const struct cli_cmd_entry *pEntry);
static void gpio_set_mode(const struct cli_cmd_entry *pEntry);
static void gpio_event_test(const struct cli_cmd_entry *pEntry);
static void apb_gpio_tests(const struct cli_cmd_entry *pEntry);
static void apb_gpio_event_tests(const struct cli_cmd_entry *pEntry);




// IO menu
const struct cli_cmd_entry io_functions[] =
{
		CLI_CMD_SIMPLE( "setmux", io_setmux,         	"ionum mux_sel 	-- set mux_sel for ionum " ),
		CLI_CMD_SIMPLE( "getmux", io_getmux,         	"ionum  		-- get mux_sel for ionum" ),
		CLI_CMD_TERMINATE()
};

// GPIO menu
const struct cli_cmd_entry gpio_functions[] =
{
		CLI_CMD_SIMPLE( "set", 	gpio_set,         		"gpio_num	-- set to one" ),
		CLI_CMD_SIMPLE( "clr", 	gpio_clr,         		"gpio_num	-- clear to zero" ),
		CLI_CMD_SIMPLE( "toggle",	gpio_toggle,        "gpio_num	-- toggle state of gpio" ),
		CLI_CMD_SIMPLE( "status",	gpio_read_status,   "gpio_num	-- read status of gpio: in, out, interrupt type and mode" ),
		CLI_CMD_SIMPLE( "mode",	gpio_set_mode,       	"gpio_num gpio_mode	-- set mode of gpio" ),
		CLI_CMD_SIMPLE( "event",	gpio_event_test,    "io_num, mux_sel, gpio_num, gpio_int_type	-- set interrupt of gpio" ),
		CLI_CMD_SIMPLE( "evnt",	apb_gpio_event_tests,        "None	-- All events of gpio" ),
		CLI_CMD_SIMPLE( "all",	apb_gpio_tests,         "None	-- All gpio tests " ),
		CLI_CMD_TERMINATE()
};



// IO functions
static void io_setmux(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	mux_sel;
	uint32_t	ionum;

	CLI_uint32_required( "ionum", &ionum );
	CLI_uint32_required( "mux_sel", &mux_sel);
	hal_setpinmux(ionum, mux_sel);
	dbg_str("<<DONE>>");
}

static void io_getmux(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	ionum;
	uint32_t	mux_sel;

	CLI_uint32_required( "ionum", &ionum );
	mux_sel = hal_getpinmux(ionum);
	dbg_str_hex32("mux_sel", mux_sel);
	dbg_str("<<DONE>>");
}

// GPIO functions
static void gpio_set(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	gpio_num;

	CLI_uint32_required( "gpio_num", &gpio_num );
	hal_set_gpio((uint8_t)gpio_num);
	dbg_str("<<DONE>>");
}

static void gpio_clr(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	gpio_num;

	CLI_uint32_required( "gpio_num", &gpio_num );
	hal_clr_gpio((uint8_t)gpio_num);
	dbg_str("<<DONE>>");
}

static void gpio_toggle(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	gpio_num;

	CLI_uint32_required( "gpio_num", &gpio_num );
	hal_toggle_gpio((uint8_t)gpio_num);
	dbg_str("<<DONE>>");
}

static void gpio_read_status(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	gpio_num;
	uint8_t	input_value;
	uint8_t	output_value;
	uint8_t	interrupt_type;
	uint8_t	gpio_mode;

	CLI_uint32_required( "gpio_num", &gpio_num );
	//hal_read_gpio_status(gpio_num, &input_value, &output_value, &interrupt_type, &gpio_mode);
	dbg_str_hex8("input", (uint32_t)input_value);
	dbg_str_hex8("output", (uint32_t)output_value);
	dbg_str_hex8("interrupt_type", (uint32_t)interrupt_type);
	dbg_str_hex8("gpio_mode", (uint32_t)gpio_mode);

	uint32_t register_value;
	hal_read_gpio_status_raw(gpio_num, &register_value);
	dbg_str_hex32("rdstatus", register_value);
	dbg_str("<<DONE>>");
}

static void gpio_set_mode(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	gpio_num;
	uint32_t	gpio_mode;

	CLI_uint32_required( "gpio_num", &gpio_num );
	CLI_uint32_required( "gpio_mode", &gpio_mode );
	hal_set_gpio_mode((uint8_t)gpio_num, (uint8_t)gpio_mode);
	dbg_str("<<DONE>>");
}

volatile unsigned int event_flag = 0;

void isr_gpio_handler(void) {
	//dbg_str("gpio event occured \r\n");
	event_flag = 1;
	hal_soc_eu_clear_fc_mask(132);
	hal_set_gpio_interrupt(4,2,0);
}

static void gpio_event_test(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	io_num;
	uint32_t	mux_sel;
	uint32_t	gpio_num;
	uint32_t	gpio_int_type;
	//uint32_t	gpio_int_en = 1;


	//hal_soc_eu_set_fc_mask(128);
	//hal_soc_eu_clear_fc_mask();
	CLI_uint32_required( "io_num", &io_num );
	CLI_uint32_required( "mux_sel", &mux_sel );
	CLI_uint32_required( "gpio_num", &gpio_num );
	CLI_uint32_required( "gpio_int_type", &gpio_int_type );
	//CLI_uint32_required( "gpio_int_en", &gpio_int_en );
	hal_clr_gpio((uint8_t)gpio_num);
	hal_setpinmux((uint8_t)io_num,(uint8_t)mux_sel);
	hal_set_gpio_mode((uint8_t)gpio_num,1);

	hal_set_gpio_interrupt((uint8_t)gpio_num, (uint8_t)gpio_int_type, 1);

	uint8_t	input_value;
	uint8_t	output_value;
	uint8_t	interrupt_type;
	uint8_t	gpio_mode;

	//hal_read_gpio_status(4, &input_value, &output_value, &interrupt_type, &gpio_mode);
	dbg_str_hex8("input", (uint32_t)input_value);
	dbg_str_hex8("output", (uint32_t)output_value);
	dbg_str_hex8("interrupt_type", (uint32_t)interrupt_type);
	dbg_str_hex8("gpio_mode", (uint32_t)gpio_mode);

	uint32_t register_value;
	hal_read_gpio_status_raw(gpio_num, &register_value);
	dbg_str_hex32("rdstatus", register_value);

	pi_fc_event_handler_set(128 + (uint8_t)gpio_num, isr_gpio_handler, NULL);
	hal_soc_eu_set_fc_mask(128 + (uint8_t)gpio_num);
	hal_toggle_gpio((uint8_t)gpio_num);
	//hal_toggle_gpio((uint8_t)gpio_num);


	//vTaskDelay(1000);
	while(event_flag == 0){
		dbg_str("NO GPIO Interrupt triggered \r\n");
	}
	dbg_str("YES GPIO Interrupt triggered \r\n");

	dbg_str("<<DONE>>\r\n");
}

static unsigned int gpio_set_clr_toggle_mode_test(gpio_struct_typedef *gpio) {

	gpio_struct_typedef lgpio;
	gpio_hal_typedef hgpio;
	uint32_t error = 0;
	char *message;
	message = pvPortMalloc(80);

	hal_setpinmux(gpio->io_num, gpio->mux_sel);
	hal_clr_gpio((uint8_t)gpio->number);

	hgpio.number = gpio->number;
	hal_read_gpio_status(&hgpio);
	gpio->number = hgpio.number;
	gpio->in_val = hgpio.in_val;
	gpio->out_val = hgpio.out_val;

#if GPIO_TEST
	sprintf(message,"Gpio No:0x%x,Io No:0x%x, Mux No: 0x%x,Out Val:0x%x, In Val:0x%x, Mode :0x%x  \r\n",
			gpio->number,gpio->io_num,gpio->mux_sel,gpio->out_val, gpio->in_val,hgpio.mode);
	dbg_str(message);
#endif

	hal_set_gpio_mode((uint8_t)(gpio->number), (uint8_t)(gpio->mode));
	switch(gpio->type) {
		case GPIO_SET:
			hal_set_gpio((uint8_t)gpio->number);
			break;
		case GPIO_CLR:
			hal_set_gpio((uint8_t)gpio->number);
			hal_clr_gpio((uint8_t)gpio->number);
			break;
		case GPIO_TOGGLE_H:
			hal_set_gpio((uint8_t)gpio->number);
			hal_toggle_gpio((uint8_t)gpio->number);
			break;
		case GPIO_TOGGLE_L:
			hal_clr_gpio((uint8_t)gpio->number);
			hal_toggle_gpio((uint8_t)gpio->number);
			break;
		default:
			break;
	}
	hal_read_gpio_status(&hgpio);
	lgpio.mux_sel = hal_getpinmux(gpio->io_num);
	lgpio.io_num = gpio->io_num;
	lgpio.number = hgpio.number;
	lgpio.out_val = hgpio.out_val;
	lgpio.in_val = hgpio.in_val;
	lgpio.mode = hgpio.mode;

#if GPIO_TEST
	sprintf(message, "Gpio No:0x%x,Io No:0x%x, Mux No: 0x%x, Out Value:0x%0x, In Val: 0x%x, Mode: 0x%x \r\n",
			lgpio.number,lgpio.io_num,lgpio.mux_sel,lgpio.out_val,lgpio.in_val,lgpio.mode );
	dbg_str(message);
#endif

	if((lgpio.mux_sel == gpio->mux_sel) && (lgpio.out_val == gpio->result)) {
		error = 0;
	}else {
		error = 1;
	}
	vPortFree(message);
	return error;
}


static void apb_gpio_tests(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	gpio_struct_typedef gpio;
	unsigned int err = 0;
	unsigned int t_type;
	gpio.io_num = 11;
	gpio.mux_sel = 2;
	gpio.mode = 0;
	for(t_type = 0; t_type <= GPIO_TOGGLE_L; t_type ++) {
		switch(t_type) {
			case GPIO_SET:
				dbg_str("GPIO Set Test :");
				gpio.result = 1;
				gpio.type = GPIO_SET;
				break;
			case GPIO_CLR:
				dbg_str("GPIO Clear Test :");
				gpio.result = 0;
				gpio.type = GPIO_CLR;
				break;
			case GPIO_TOGGLE_H:
				dbg_str("GPIO Toggle High Test :");
				gpio.result = 0;
				gpio.type = GPIO_TOGGLE_H;
				break;
			case GPIO_TOGGLE_L:
				dbg_str("GPIO Toggle Low Test :");
				gpio.result = 1;
				gpio.type = GPIO_TOGGLE_L;
				break;
			default:
				break;
		}
		for(gpio.number = 4; gpio.number <= 40; gpio.number++ ) {
			err =+ gpio_set_clr_toggle_mode_test(&gpio);
			gpio.io_num++;
		}
		gpio.io_num = 11;
		gpio.number = 4;
		if (!err) {
			dbg_str("<<PASSED>>\r\n");
		}
		else {
			dbg_str("<<FAILED>>\r\n");
		}
	}
}



void event_gpio_handler(void) {
	event_flag = 1;
}

static unsigned int gpio_even_tests(gpio_struct_typedef *gpio) {

	gpio_struct_typedef lgpio;
	gpio_hal_typedef hgpio;
	uint32_t error = 0;
	char *message;
	message = pvPortMalloc(80);

	hal_setpinmux(gpio->io_num, gpio->mux_sel);
	hal_clr_gpio((uint8_t)gpio->number);

	hgpio.number = gpio->number;
	hal_read_gpio_status(&hgpio);
	gpio->number = hgpio.number;
	gpio->in_val = hgpio.in_val;
	gpio->out_val = hgpio.out_val;

#if GPIO_TEST
	sprintf(message,"Io No:0x%x, Mux No: 0x%x, Gpio No:0x%x, Out Val:0x%x, In Val:0x%x, Mode :0x%x,Int Type:0x%x, Int En:0x%x \r\n",
			gpio->io_num,gpio->mux_sel,gpio->number,gpio->out_val, gpio->in_val,hgpio.mode,hgpio.int_type,hgpio.int_en);
	dbg_str(message);
#endif

	hal_set_gpio_mode((uint8_t)(gpio->number), (uint8_t)(gpio->mode));
	hal_set_gpio_interrupt((uint8_t)(gpio->number), (uint8_t)gpio->int_type,(uint8_t)gpio->int_en);
	hal_read_gpio_status(&hgpio);
	lgpio.mux_sel = hal_getpinmux(gpio->io_num);
	lgpio.number = hgpio.number;
	lgpio.io_num = gpio->io_num;
	lgpio.out_val = hgpio.out_val;
	lgpio.in_val = hgpio.in_val;
	lgpio.mode = hgpio.mode;
	lgpio.int_type = hgpio.int_type;
	lgpio.int_en = hgpio.int_en;

	#if GPIO_TEST
		sprintf(message, "Io No:0x%x, Mux No: 0x%x,Gpio No:0x%x, Out Value:0x%0x, In Val: 0x%x, Mode: 0x%x, Int Type:0x%x, Int En:0x%x \r\n",
				lgpio.io_num,lgpio.mux_sel,lgpio.number,lgpio.out_val,lgpio.in_val,lgpio.mode, lgpio.int_type,lgpio.int_en );
		dbg_str(message);
	#endif

	pi_fc_event_handler_set(128 + (uint8_t)gpio->number, event_gpio_handler, NULL);
	hal_soc_eu_set_fc_mask(128 + (uint8_t)gpio->number);

	switch(gpio->event) {
		case FALLING_EDGE:
			hal_toggle_gpio((uint8_t)gpio->number);
			hal_toggle_gpio((uint8_t)gpio->number);
			break;
		case RISING_EDGE:
			hal_toggle_gpio((uint8_t)gpio->number);
			break;
		case ANY_EDGE:
			hal_toggle_gpio((uint8_t)gpio->number);
			break;
		case ACTIVE_HIGH:
			//hal_toggle_gpio((uint8_t)gpio->number);
			hal_clr_gpio((uint8_t)gpio->number);
			hal_set_gpio((uint8_t)gpio->number);

			break;
		case ACTIVE_LOW:
			hal_set_gpio((uint8_t)gpio->number);
			hal_clr_gpio((uint8_t)gpio->number);

			break;

		default:
			break;
	}
	while(event_flag == 0){
		dbg_str("NO GPIO Interrupt triggered \r\n");
	}
	if(event_flag == 0x1) {
		hal_soc_eu_clear_fc_mask(128 + (uint8_t)gpio->number);
		hal_set_gpio_interrupt((uint8_t)(gpio->number), 0x0, 0x0); //(uint8_t)gpio->int_en);
		error = 0;
		event_flag = 0;
	}
	else {
		error = 1;
	}

	vPortFree(message);
	return error;
}

static void apb_gpio_event_tests(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	gpio_struct_typedef gpio;
	unsigned int err = 0;
	unsigned int int_type;
	gpio.io_num = 11;
	gpio.mux_sel = 2;
	gpio.mode = 1;
	gpio.int_en = 1;
	gpio.number = 4;

	int_type = RISING_EDGE;
	for(int_type = FALLING_EDGE; int_type <= ACTIVE_HIGH; int_type ++) {
		switch(int_type) {
			case FALLING_EDGE:
				dbg_str("Gpio Int Falling Edge Test  :");
				gpio.int_type = FALLING_EDGE;
				gpio.event = FALLING_EDGE;
				break;
			case RISING_EDGE:
				dbg_str("Gpio Int Rising Edge Test :");
				gpio.int_type = RISING_EDGE;
				gpio.event = RISING_EDGE;
				break;
			case ANY_EDGE:
				dbg_str("Gpio Int Any Edge Test :");
				gpio.int_type = ANY_EDGE;
				gpio.event = ANY_EDGE;
				break;

			case ACTIVE_LOW:
				dbg_str("Gpio Int Active Low Test :");
				gpio.int_type = ACTIVE_LOW;
				gpio.event = ACTIVE_LOW;
				break;

			case ACTIVE_HIGH:
				dbg_str("Gpio Int Active High Test :");
				gpio.int_type = ACTIVE_HIGH;
				gpio.event = ACTIVE_HIGH;
				break;

			default:
				break;
		}
		if(int_type != 4) {
			for(gpio.number = 4; gpio.number <= 31; gpio.number++ ) {
				err =+ gpio_even_tests(&gpio);
				gpio.io_num++;
			}

			gpio.io_num = 11;
			gpio.number = 4;
			if (!err) {
				dbg_str("<<PASSED>>\r\n");
			}
			else {
				dbg_str("<<FAILED>>\r\n");
			}
		}
	}
}

