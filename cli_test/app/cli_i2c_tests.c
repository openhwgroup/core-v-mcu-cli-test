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

#include "drivers/include/udma_i2cm_driver.h"
#include "libs/cli/include/cli.h"
#include "FreeRTOS.h"
#include "semphr.h"	// Required for configASSERT



// I2CM0 functions
static void i2cm_readbyte(const struct cli_cmd_entry *pEntry);
static void i2cm_writebyte(const struct cli_cmd_entry *pEntry);
static void i2cm_reset(const struct cli_cmd_entry *pEntry);
static void i2c_temp(const struct cli_cmd_entry *pEntry);
static void i2c_read_dev_id(const struct cli_cmd_entry *pEntry);
static void i2cm_singlebyte_test(const struct cli_cmd_entry *pEntry);

// I2CM0 menu
const struct cli_cmd_entry i2cm0_functions[] =
{

  CLI_CMD_WITH_ARG( "readbyte", 	i2cm_readbyte,	0, "i2c_addr reg_addr 			-- read register" ),
  CLI_CMD_WITH_ARG( "writebyte", 	i2cm_writebyte,	0, "i2c_addr reg_addr value 	-- read register" ),
  CLI_CMD_WITH_ARG( "singlebyte", 	i2cm_singlebyte_test,	0, "i2c_addr reg_addr	-- writes 0xA5 and then 0x5A to register and checks result" ),
  CLI_CMD_TERMINATE()

};


// I2CM1 menu
const struct cli_cmd_entry i2cm1_functions[] =
{
	CLI_CMD_WITH_ARG( "readbyte", i2cm_readbyte,	1, "i2c_addr reg_addr 	-- read register" ),
	CLI_CMD_SIMPLE ( "temp", i2c_temp,				   "read on board temperature"),
	CLI_CMD_SIMPLE ( "dev_id", i2c_read_dev_id,		    "read i2c device id"),
	CLI_CMD_TERMINATE()
};


/////////////////////////////////////////////////////////////////
//
// I2CM functions
//
/////////////////////////////////////////////////////////////////
static uint8_t i2c_buffer[256];
static void i2cm_readbyte(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	i2c_addr;
	uint32_t	reg_addr;

	CLI_uint32_required( "i2c_addr", &i2c_addr );
	CLI_uint32_required( "reg_addr", &reg_addr );

	udma_i2cm_read(pEntry->cookie, (uint8_t)i2c_addr, (uint8_t)reg_addr, 1, i2c_buffer, false);
	dbg_str_hex8("reg", (int)i2c_buffer[0]);
}

static void i2cm_writebyte(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	i2c_addr;
	uint32_t	reg_addr;
	uint32_t	reg_value;

	CLI_uint32_required( "i2c_addr", &i2c_addr );
	CLI_uint32_required( "reg_addr", &reg_addr );
	CLI_uint32_required( "reg_value", &reg_value );

	i2c_buffer[0] = (uint8_t)reg_value;
	udma_i2cm_write (pEntry->cookie, i2c_addr, reg_addr, 1, i2c_buffer,  false);
}

static void i2cm_reset(const struct cli_cmd_entry *pEntry)
{
    (void)pEntry;
    // Add functionality here

    udma_i2cm_control(pEntry->cookie, kI2cmReset, NULL);
}

static void i2cm_singlebyte_test(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	uint32_t	i2c_addr;
	uint32_t	reg_addr;
	bool		fPassed = false;

	CLI_uint32_required( "i2c_addr", &i2c_addr );
	CLI_uint32_required( "reg_addr", &reg_addr );

	i2c_buffer[0] = 0xA5;
	udma_i2cm_write (pEntry->cookie, i2c_addr, reg_addr, 1, i2c_buffer,  false);
	udma_i2cm_read(pEntry->cookie, (uint8_t)i2c_addr, (uint8_t)reg_addr, 1, i2c_buffer, false);
	dbg_str_int("First access", i2c_buffer[0]);
	if (i2c_buffer[0] == 0xA5) {
		i2c_buffer[0] = 0x5A;
		udma_i2cm_write (pEntry->cookie, i2c_addr, reg_addr, 1, i2c_buffer,  false);
		udma_i2cm_read(pEntry->cookie, (uint8_t)i2c_addr, (uint8_t)reg_addr, 1, i2c_buffer, false);
		dbg_str_int("Second access", i2c_buffer[0]);
		if (i2c_buffer[0] == 0x5A) {
			fPassed = true;
		}
	}
	if (fPassed) {
		dbg_str("<<PASSED>>");
	} else {
		dbg_str("<<FAILED>>");
	}
}

static void i2c_temp (const struct cli_cmd_entry *pEntry)
{
	char *message = 0;
	int temp;
	message  = pvPortMalloc(80);
	configASSERT (message);
	udma_i2cm_read(1, 0x96, 0x00, 2, i2c_buffer, false);
	temp = (i2c_buffer[0] << 8) + i2c_buffer[1];
	temp = ((temp *625) / 44000) + 32;
	sprintf(message," Board temp = %d F\r\n", temp);
	dbg_str(message);
	vPortFree(message);
}

static void i2c_read_dev_id(const struct cli_cmd_entry *pEntry)
{
	char *message = 0;
	int temp;
	message  = pvPortMalloc(80);
	configASSERT (message);
	udma_i2cm_read(1, 0x96, 0x0B, 1, i2c_buffer, false);

	sprintf(message," i2c_dev_id:= 0x%x \r\n", i2c_buffer[0]);
	dbg_str(message);
	if(i2c_buffer[0] == 0xCB) {
		dbg_str("Dev Id Test: <<PASSED>>\r\n");
	}
	else {
		dbg_str("Dev Id Test: <<FAILED>>\r\n");
	}
	vPortFree(message);
}

