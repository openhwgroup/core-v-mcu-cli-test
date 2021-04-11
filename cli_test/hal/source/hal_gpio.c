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

 #include "FreeRTOS.h"
 #include "FreeRTOSConfig.h"
 #include "kernel/include/task.h"		// Needed for configASSERT

 #include "target/core-v-mcu/include/core-v-mcu-config.h"
 #include "hal/include/hal_apb_gpio_reg_defs.h"
 #include "hal/include/hal_gpio.h"

void hal_write_gpio(uint8_t gpio_num, uint8_t value) {
	if (value) {
		hal_set_gpio(gpio_num);
	} else {
		hal_clr_gpio(gpio_num);
	}
}

void hal_set_gpio(uint8_t gpio_num) {
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_START_ADDR;

	papbgpio->setgpio_b.gpio_num = gpio_num;
}

void hal_clr_gpio(uint8_t gpio_num) {
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_START_ADDR;

	papbgpio->clrgpio_b.gpio_num = gpio_num;
}

void hal_toggle_gpio(uint8_t gpio_num) {
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_START_ADDR;

	papbgpio->toggpio_b.gpio_num = gpio_num;
}

void hal_set_gpio_num(uint8_t gpio_num){}

void hal_read_gpio_status(uint8_t gpio_num, uint8_t* input_value, uint8_t* output_value, uint8_t* interrupt_type, uint8_t* gpio_mode){
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_START_ADDR;

	papbgpio->setsel_b.gpio_num = gpio_num;		// Set address for following reads
	*input_value = papbgpio->rdstat_b.input;
	*output_value = papbgpio->rdstat_b.output;
	*interrupt_type = papbgpio->rdstat_b.inttype;
	*gpio_mode = papbgpio->rdstat_b.mode;
}

void hal_read_gpio_status_raw(uint8_t gpio_num, uint32_t* register_value){
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_START_ADDR;

	papbgpio->setsel_b.gpio_num = gpio_num;		// Set address for following reads
	*register_value = papbgpio->rdstat;
}

void hal_set_gpio_mode(uint8_t gpio_num, uint8_t gpio_mode){
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_START_ADDR;

	papbgpio->setmode_b.gpio_num = gpio_num;  //ToDo: is there a race here -- do we need to write both at same time?
	papbgpio->setmode_b.mode = gpio_mode;
}

void hal_set_gpio_interrupt(uint8_t gpio_num, uint8_t interrupt_type, uint8_t interrupt_enable){}
void hal_enable_gpio_interrupt(uint8_t gpio_num){}
void hal_disable_gpio_interrupt(uint8_t gpio_num){}
