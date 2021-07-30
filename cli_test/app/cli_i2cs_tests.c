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
#include "libs/utils/include/dbg_uart.h"
#include "hal/include/hal_apb_i2cs_reg_defs.h"
#include "hal/include/hal_apb_i2cs.h"


static void i2cs_readSlaveAddress(const struct cli_cmd_entry *pEntry);
static void i2cs_writeSlaveAddress(const struct cli_cmd_entry *pEntry);
static void i2cs_on(const struct cli_cmd_entry *pEntry);
static void i2cs_off(const struct cli_cmd_entry *pEntry);
static void i2cs_runI2cToApbFIFOTests(const struct cli_cmd_entry *pEntry);
static void i2cs_runApbToI2cFIFOTests(const struct cli_cmd_entry *pEntry);

static void i2cs_runI2cToApbMsgTests(const struct cli_cmd_entry *pEntry);
static void i2cs_runApbToI2cMsgTests(const struct cli_cmd_entry *pEntry);
static void i2cs_runI2cToApbFIFOTests(const struct cli_cmd_entry *pEntry);
static void i2cs_runApbToI2cFIFOTests(const struct cli_cmd_entry *pEntry);
static void i2cs_runI2cToApbFIFOWatermarkLevelTests(const struct cli_cmd_entry *pEntry);
static void i2cs_runApbToI2cFIFOWatermarkLevelTests(const struct cli_cmd_entry *pEntry);
const struct cli_cmd_entry i2cs_functions[] =
{
	CLI_CMD_SIMPLE ( "on", i2cs_on,		"switch ON i2c slave"),
	CLI_CMD_SIMPLE ( "off", i2cs_off,	"switch OFF i2c slave"),
	CLI_CMD_SIMPLE ( "rdaddr", i2cs_readSlaveAddress,	"read i2c slave address"),
	CLI_CMD_WITH_ARG( "wraddr", i2cs_writeSlaveAddress,	1, 	"write i2c slave address"),
	CLI_CMD_WITH_ARG( "i2c2apbmsg", i2cs_runI2cToApbMsgTests,	1, 	"Run I2C slave single msg tests"),
	CLI_CMD_WITH_ARG( "apb2i2cmsg", i2cs_runApbToI2cMsgTests,	1, 	"Run I2C slave single msg tests"),
	CLI_CMD_WITH_ARG( "i2c2apbfifo", i2cs_runI2cToApbFIFOTests,	1, 	"Run I2C slave FIFO tests"),
	CLI_CMD_WITH_ARG( "apb2i2cfifo", i2cs_runApbToI2cFIFOTests,	1, 	"Run I2C slave FIFO tests"),
	CLI_CMD_WITH_ARG( "i2c2apbfifowm", i2cs_runI2cToApbFIFOWatermarkLevelTests,	1, 	"Run I2C slave FIFO water mark level tests"),
	CLI_CMD_WITH_ARG( "apb2i2cfifowm", i2cs_runApbToI2cFIFOWatermarkLevelTests,	1, 	"Run I2C slave FIFO water mark level tests"),
	CLI_CMD_TERMINATE()
};

static void i2cs_on (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t status = 0;
	status = hal_set_apb_i2cs_slave_on_off(1);
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

	status = hal_set_apb_i2cs_slave_on_off(0);
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
	address = hal_get_apb_i2cs_slave_address();

	dbg_str_hex8("Slave Address", address);
	dbg_str("<<PASSED>>\r\n");

}

static void i2cs_writeSlaveAddress (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t address = 0;
	uint8_t lReadAddress = 0;

	CLI_uint8_required( "i2c_addr", &address );

	hal_set_apb_i2cs_slave_address(address);

	lReadAddress = hal_get_apb_i2cs_slave_address();

	CLI_printf("Setting I2C Slave address to 0x%02x\n", address);
	if( lReadAddress == address )
	{
		dbg_str("<<PASSED>>\r\n");
	}
	else
	{
		dbg_str("<<FAILED>>\r\n");
	}
}
static uint8_t gsI2CTxBuf[32] = {0};
static uint8_t gsI2CRxBuf[32] = {0};
#define MY_I2C_SLAVE_ADDRESS 		0x62
#define MY_I2C_SLAVE_ADDRESS_7BIT 	(0x62 << 1)

#define I2C_MASTER_REG_DEV_ADDRESS								0x00
#define I2C_MASTER_REG_ENABLE									0x01
#define I2C_MASTER_REG_DEBOUNCE_LENGTH							0x02
#define I2C_MASTER_REG_SCL_DELAY_LENGTH							0x03
#define I2C_MASTER_REG_SDA_DELAY_LENGTH							0x04
#define I2C_MASTER_REG_MSG_I2C_APB								0x10
#define I2C_MASTER_REG_MSG_I2C_APB_STATUS						0x11
#define I2C_MASTER_REG_MSG_APB_I2C								0x12
#define I2C_MASTER_REG_MSG_APB_I2C_STATUS						0x13
#define I2C_MASTER_REG_FIFO_I2C_APB_WRITE_DATA_PORT				0x20
#define I2C_MASTER_REG_FIFO_I2C_APB_READ_DATA_PORT				0x21
#define I2C_MASTER_REG_FIFO_I2C_APB_FLUSH						0x22
#define I2C_MASTER_REG_FIFO_I2C_APB_WRITE_FLAGS					0x23
#define I2C_MASTER_REG_FIFO_I2C_APB_READ_FLAGS					0x24
#define I2C_MASTER_REG_FIFO_APB_I2C_WRITE_DATA_PORT				0x30
#define I2C_MASTER_REG_FIFO_APB_I2C_READ_DATA_PORT				0x31
#define I2C_MASTER_REG_FIFO_APB_I2C_FLUSH						0x32
#define I2C_MASTER_REG_FIFO_APB_I2C_WRITE_FLAGS					0x33
#define I2C_MASTER_REG_FIFO_APB_I2C_READ_FLAGS					0x34
#define I2C_MASTER_REG_INTERRUPT_STATUS							0x40
#define I2C_MASTER_REG_INTERRUPT_ENABLE							0x41
#define I2C_MASTER_REG_INTERRUPT_I2C_APB_WRITE_FLAGS_SELECT		0x42
#define I2C_MASTER_REG_INTERRUPT_APB_I2C_READ_FLAGS_SELECT		0x43
#define I2C_MASTER_REG_INTERRUPT_TO_APB_STATUS					0x50
#define I2C_MASTER_REG_INTERRUPT_TO_APB_ENABLE					0x51
#define I2C_MASTER_REG_INTERRUPT_APB_I2C_WRITE_FLAGS_SELECT 	0x52
#define I2C_MASTER_REG_INTERRUPT_I2C_APB_READ_FLAGS_SELECT		0x53


static void i2cs_runI2cToApbFIFOTests (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t lTestByte = 0xCD;

	hal_set_apb_i2cs_slave_on_off(1);
	if( hal_get_apb_i2cs_slave_address() !=  MY_I2C_SLAVE_ADDRESS )
		hal_set_apb_i2cs_slave_address(MY_I2C_SLAVE_ADDRESS);


	//I2C master flushes the I2C2APB FIFO first.
	//gsI2CTxBuf[0] = 1;
	//udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_FLUSH, 1, gsI2CTxBuf,  false);
	//vTaskDelay(10);
	//gsI2CTxBuf[0] = 0;
	//udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_FLUSH, 1, gsI2CTxBuf,  false);
	//vTaskDelay(10);

	//hal_i2cs_fifo_i2c_apb_flush();
	//I2C master checks if space is available to write into the FIFO
	gsI2CRxBuf[0] = 0;
	if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_FLAGS, 1, gsI2CRxBuf, false) == pdTRUE )
	{
		if( gsI2CRxBuf[0] != 0x07 )	//FIFO is not full, at least one byte space is available to write into the FIFO,
		{
			gsI2CTxBuf[0] = lTestByte;
			if( udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_DATA_PORT, 1, gsI2CTxBuf,  false) == pdTRUE )
			{

			}
			else
			{

			}

			//Ensure that the FIFO write happened by checking if the write count increased
			if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_READ_FLAGS, 1, gsI2CRxBuf, false) == pdTRUE )
			{
				if( gsI2CRxBuf[0] == 0x01 )
				{
					dbg_str("0. <<PASSED>>\r\n");
					//APB reads out the written data.
					//If the read FIFO is not empty
					if( hal_get_i2cs_fifo_i2c_apb_read_flags() != 0 )
					{
						if( hal_get_i2cs_fifo_i2c_apb_read_data_port() == lTestByte )
						{
							dbg_str("1. <<PASSED>>\r\n");
						}
						else
						{
							dbg_str("<<FAILED>>\r\n");
						}
					}
				}
				else
				{
					dbg_str("<<FAILED>>\r\n");
				}
			}
		}
		else
		{

		}
	}
}

static void i2cs_runApbToI2cFIFOTests (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t lTestByte = 0x12;

	hal_set_apb_i2cs_slave_on_off(1);
	if( hal_get_apb_i2cs_slave_address() !=  MY_I2C_SLAVE_ADDRESS )
		hal_set_apb_i2cs_slave_address(MY_I2C_SLAVE_ADDRESS);


	//I2C master flushes the I2C2APB FIFO first.
	//gsI2CTxBuf[0] = 1;
	//udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_FLUSH, 1, gsI2CTxBuf,  false);
	//vTaskDelay(10);
	//gsI2CTxBuf[0] = 0;
	//udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_FLUSH, 1, gsI2CTxBuf,  false);
	//vTaskDelay(10);

	//hal_i2cs_fifo_i2c_apb_flush();
	//I2C master checks if space is available to write into the FIFO



	if( hal_get_i2cs_fifo_apb_i2c_write_flags() != 0x07 )	//FIFO is not full, at least one byte space is available to write into the FIFO,
	{
		hal_set_i2cs_fifo_apb_i2c_write_data_port(lTestByte);

		//Ensure that the FIFO write happened by checking if the write count increased
		if(hal_get_i2cs_fifo_apb_i2c_read_flags() == 0x01 )
		{
			dbg_str("0. <<PASSED>>\r\n");
			//I2C master reads out the written data.
			//If the APB2I2C read FIFO is not empty
			gsI2CRxBuf[0] = 0;
			if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_APB_I2C_READ_FLAGS, 1, gsI2CRxBuf, false) == pdTRUE )
			{
				if( gsI2CRxBuf[0] != 0 ) //APB2I2C read FIFO is not empty
				{
					gsI2CRxBuf[0] = 0;	//APB2I2C read out FIFO data
					if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_APB_I2C_READ_DATA_PORT, 1, gsI2CRxBuf, false) == pdTRUE )
					{
						if( gsI2CRxBuf[0] == lTestByte )
						{
							dbg_str("1. <<PASSED>>\r\n");
						}
						else
						{
							dbg_str("<<FAILED>>\r\n");
						}
					}
				}
			}
		}
		else
		{
			dbg_str("<<FAILED>>\r\n");
		}
	}
	else
	{

	}
}

static void i2cs_runI2cToApbMsgTests (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t lTestByte = 0x34;

	hal_set_apb_i2cs_slave_on_off(1);
	if( hal_get_apb_i2cs_slave_address() !=  MY_I2C_SLAVE_ADDRESS )
		hal_set_apb_i2cs_slave_address(MY_I2C_SLAVE_ADDRESS);

	gsI2CTxBuf[0] = lTestByte;

	if( udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_I2C_APB, 1, gsI2CTxBuf,  false) == pdTRUE )
	{
		gsI2CRxBuf[0] = 0;
		//Ensuring from the I2C side that the APB side gets the correct status
		if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_I2C_APB_STATUS, 1, gsI2CRxBuf, false) == pdTRUE )
		{
			if( gsI2CRxBuf[0] == 1 )
			{
				//now check from APB side that a byte msg is received.
				if( hal_get_i2cs_msg_i2c_apb_status() == 1 )
				{
					if( hal_get_i2cs_msg_i2c_apb() == lTestByte )
					{
						dbg_str("0. <<PASSED>>\r\n");
						//When the Message register (offset 0x10) has been read by the APB interface, this bit will be automatically cleared.
						//Checking this condition
						if( hal_get_i2cs_msg_i2c_apb_status() == 0 )
						{
							dbg_str("1. <<PASSED>>\r\n");
						}
						else
						{
							dbg_str("0. <<FAILED>>\r\n");
						}
					}
					else
					{
						dbg_str("1. <<FAILED>>\r\n");
					}
				}
				else
				{
					dbg_str("2. <<FAILED>>\r\n");
				}
			}
			else
			{
				dbg_str("3. <<FAILED>>\r\n");
			}
		}
		else
		{
			dbg_str("4. <<FAILED>>\r\n");
		}
	}
	else
	{
		dbg_str("5. <<FAILED>>\r\n");
	}
}

static void i2cs_runApbToI2cMsgTests (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t lTestByte = 0x56;

	hal_set_apb_i2cs_slave_on_off(1);
	if( hal_get_apb_i2cs_slave_address() !=  MY_I2C_SLAVE_ADDRESS )
		hal_set_apb_i2cs_slave_address(MY_I2C_SLAVE_ADDRESS);


	hal_set_i2cs_msg_apb_i2c(lTestByte);
	//Ensuring from the APB side that the I2C side gets the correct status

	if( hal_get_i2cs_msg_apb_i2c_status() == 1 )
	{
		//now check from I2C side that a byte msg is received.
		gsI2CRxBuf[0] = 0;
		if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C_STATUS, 1, gsI2CRxBuf, false) == pdTRUE )
		{
			if( gsI2CRxBuf[0] == 1 )
			{
				gsI2CRxBuf[0] = 0;
				if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C, 1, gsI2CRxBuf, false) == pdTRUE )
				{
					if( gsI2CRxBuf[0] == lTestByte )
					{
						dbg_str("0. <<PASSED>>\r\n");
						if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C_STATUS, 1, gsI2CRxBuf, false) == pdTRUE )
						{
							if( gsI2CRxBuf[0] == 0 )
							{
								dbg_str("1. <<PASSED>>\r\n");
							}
							else
							{
								dbg_str("0. <<FAILED>>\r\n");
							}
						}
						else
						{
							dbg_str("1. <<FAILED>>\r\n");
						}
					}
					else
					{
						dbg_str("2. <<FAILED>>\r\n");
					}
				}
				else
				{
					dbg_str("3. <<FAILED>>\r\n");
				}
			}
			else
			{
				dbg_str("4. <<FAILED>>\r\n");
			}
		}
		else
		{
			dbg_str("5. <<FAILED>>\r\n");
		}
	}
	else
	{
		dbg_str("6. <<FAILED>>\r\n");
	}
}


static void i2cs_runI2cToApbFIFOWatermarkLevelTests (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint16_t i = 0;

	hal_set_apb_i2cs_slave_on_off(1);
	if( hal_get_apb_i2cs_slave_address() !=  MY_I2C_SLAVE_ADDRESS )
		hal_set_apb_i2cs_slave_address(MY_I2C_SLAVE_ADDRESS);
#if 0
	//Temporarily using this to flush all the contents of the I2C2APB FIFO
	while( hal_get_i2cs_fifo_i2c_apb_read_flags() != 0 )
		hal_get_i2cs_fifo_i2c_apb_read_data_port();

	gsI2CRxBuf[0] = 0;
	if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_FLAGS, 1, gsI2CRxBuf, false) == pdTRUE )
	{
		if( gsI2CRxBuf[0] == 0x0 )	//128+ bytes left in FIFO
		{
			//Write 128 bytes into I2C2APB FIFO
			for( i=0; i<128; i++ )
			{
				gsI2CTxBuf[0] = i;
				if( udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_DATA_PORT, 1, gsI2CTxBuf,  false) == pdTRUE )
				{

				}
			}
			gsI2CRxBuf[0] = 0;
			if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_FLAGS, 1, gsI2CRxBuf, false) == pdTRUE )
			{
				if( gsI2CRxBuf[0] == 0x1 )	//64 - 127 bytes left in FIFO
				{
					for( i=0; i<64; i++ )
					{
						gsI2CTxBuf[0] = i;
						if( udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_DATA_PORT, 1, gsI2CTxBuf,  false) == pdTRUE )
						{
						}
					}
				}
				gsI2CRxBuf[0] = 0;
				if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_FLAGS, 1, gsI2CRxBuf, false) == pdTRUE )
				{
					if( gsI2CRxBuf[0] == 0x2 )	//32-63 spaces available
					{
						for( i=0; i<32; i++ )
						{
							gsI2CTxBuf[0] = i;
							if( udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_DATA_PORT, 1, gsI2CTxBuf,  false) == pdTRUE )
							{

							}
						}
					}
					gsI2CRxBuf[0] = 0;
					if( udma_i2cm_read(0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_FLAGS, 1, gsI2CRxBuf, false) == pdTRUE )
					{
						if( gsI2CRxBuf[0] == 0x3 )	//32-63 spaces available
						{
							for( i=0; i<32; i++ )
							{
								gsI2CTxBuf[0] = i;
								if( udma_i2cm_write (0, MY_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_DATA_PORT, 1, gsI2CTxBuf,  false) == pdTRUE )
								{

								}
							}
						}
					}
				}

#endif
}

static void i2cs_runApbToI2cFIFOWatermarkLevelTests (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	uint8_t lTestByte = 0x56;

	hal_set_apb_i2cs_slave_on_off(1);
	if( hal_get_apb_i2cs_slave_address() !=  MY_I2C_SLAVE_ADDRESS )
		hal_set_apb_i2cs_slave_address(MY_I2C_SLAVE_ADDRESS);

}

