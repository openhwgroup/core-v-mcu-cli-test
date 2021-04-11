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
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_CTRL_START_ADDR;

	papbgpio->setgpio_b.gpio_addr = gpio_num;
}

void hal_clr_gpio(uint8_t gpio_num) {
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_CTRL_START_ADDR;

	papbgpio->clrgpio_b.gpio_addr = gpio_num;
}

void hal_toggle_gpio(uint8_t gpio_num) {
	ApbGpio_t*	papbgpio = (ApbGpio_t*)GPIO_CTRL_START_ADDR;

	papbgpio->toggpio_b.gpio_addr = gpio_num;
}
