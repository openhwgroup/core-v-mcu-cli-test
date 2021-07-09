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

#include "libs/cli/include/cli.h"
#include "FreeRTOS.h"
#include "semphr.h"	// Required for configASSERT

extern uint8_t hal_get_apb_i2c_slave_address(void);
extern void hal_set_apb_i2c_slave_address(uint8_t aSlaveAddress);
extern uint8_t hal_set_apb_i2c_slave_on_off(uint8_t aStatus);

static void i2cs_readSlaveAddress(const struct cli_cmd_entry *pEntry);
static void i2cs_writeSlaveAddress(const struct cli_cmd_entry *pEntry);
static void i2cs_on(const struct cli_cmd_entry *pEntry);
static void i2cs_off(const struct cli_cmd_entry *pEntry);

const struct cli_cmd_entry i2cs_functions[] =
{
	CLI_CMD_SIMPLE ( "on", i2cs_on,		"switch ON i2c slave"),
	CLI_CMD_SIMPLE ( "off", i2cs_off,	"switch OFF i2c slave"),
	CLI_CMD_SIMPLE ( "rdaddr", i2cs_readSlaveAddress,	"read i2c slave address"),
	CLI_CMD_WITH_ARG( "wraddr", i2cs_writeSlaveAddress,	1, 	"write i2c slave address"),
	CLI_CMD_TERMINATE()
};

static void i2cs_on (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t status = 0;
	status = hal_set_apb_i2c_slave_on_off(1);
	if( status == 1 )
	{
		dbg_str("<<PASSED>>\r\n");
	}
	else
	{
		dbg_str("<<FAILED>>\r\n");
	}

}

static void i2cs_off (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t status = 0;

	status = hal_set_apb_i2c_slave_on_off(0);
	if( status == 0 )
	{
		dbg_str("<<PASSED>>\r\n");
	}
	else
	{
		dbg_str("<<FAILED>>\r\n");
	}
}

static void i2cs_readSlaveAddress (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t address = 0;
	address = hal_get_apb_i2c_slave_address();

	dbg_str_hex8("Slave Address", address);
	dbg_str("<<PASSED>>\r\n");

}

static void i2cs_writeSlaveAddress (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t address = 0;
	uint8_t lReadAddress = 0;

	CLI_uint8_required( "i2c_addr", &address );

	hal_set_apb_i2c_slave_address(address);

	lReadAddress = hal_get_apb_i2c_slave_address();

	CLI_printf("Setting I2C Slave address to 0x%02x\n", address);
	if( lReadAddress == address )
	{
		dbg_str("<<PASSED>>\r\n");
	}
	else
	{
		dbg_str("<<PASSED>>\r\n");
	}
}

