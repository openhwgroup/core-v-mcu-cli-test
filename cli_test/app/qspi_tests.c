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
static void flash_readid (const struct cli_cmd_entry *pEntry);
static uint8_t flash_sector_erase (const struct cli_cmd_entry *pEntry);
static uint8_t flash_subsector_erase (const struct cli_cmd_entry *pEntry);
static uint8_t flash_bulk_erase (const struct cli_cmd_entry *pEntry);
static void flash_read (const struct cli_cmd_entry *pEntry);
static void flash_write (const struct cli_cmd_entry *pEntry);

// EFPGA menu
const struct cli_cmd_entry qspi_cli_tests[] =
{
  CLI_CMD_SIMPLE( "read", qspi_read, "qspi read" ),
  CLI_CMD_WITH_ARG( "write", qspi_write, 0, "qspi write" ),
  CLI_CMD_WITH_ARG( "flashid", flash_readid, 0, "read flash id" ),
  CLI_CMD_WITH_ARG( "flash_read", flash_read, 0, "read spi flash address, num_bytes"),
  CLI_CMD_WITH_ARG( "flash_write", flash_write, 0, "write spi flash address, data"),
  CLI_CMD_WITH_ARG( "erase", flash_subsector_erase, 0, "Erase 4K subsector" ),
  CLI_CMD_WITH_ARG( "sector_erase", flash_sector_erase, 0, "Erase 64K sector" ),
  CLI_CMD_WITH_ARG( "bulk_erase", flash_bulk_erase, 0, "Erase All 32MB" ),
  CLI_CMD_TERMINATE()
};

static void flash_write (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		int errors = 0;
		int addr, i;
		uint16_t length;
		uint8_t l2addr[4];
		uint32_t wdata;
		CLI_uint32_required( "addr", &addr );
		CLI_uint32_required( "data", &wdata );
		l2addr[0] = (wdata >> 24) & 0xff;
		l2addr[1] = (wdata >> 16) & 0xff;
		l2addr[2] = (wdata >> 8) & 0xff;
		l2addr[3] = wdata  & 0xff;
		message  = pvPortMalloc(80);
		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		sprintf(message,"Qspi Flash write\n");
		dbg_str(message);
		udma_flash_write(0, 0, addr, l2addr, 4);
		udma_flash_read(0, 0, addr, l2addr, 4);
		sprintf(message,"Read data = %02x",l2addr[0]);
		dbg_str(message);
		for (i = 1; i < 4; i++) {
			sprintf(message," %02x",l2addr[i]);
			dbg_str(message);
		}
		dbg_str("\n");
		vPortFree(message);
}
static void flash_read(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		int errors = 0;
		uint32_t addr, i;
		uint16_t length;
		uint8_t *l2addr;
		CLI_uint32_required( "addr", &addr );
		CLI_uint16_required( "length", &length );
		message  = pvPortMalloc(80);
		l2addr = pvPortMalloc(length);
		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		sprintf(message,"Qspi Flash Read\n");
		dbg_str(message);
		udma_flash_read(0, 0, addr, l2addr, length);
		sprintf(message,"Read data = %02x",l2addr[0]);
		dbg_str(message);
		for (i = 1; i < length; i++) {
			sprintf(message," %02x",l2addr[i]);
			dbg_str(message);
		}
		dbg_str("\n");
		vPortFree(l2addr);
		vPortFree(message);
}

static uint8_t flash_bulk_erase (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		int errors = 0;
		int addr,i;
		uint8_t result;
		message  = pvPortMalloc(80);
		CLI_uint32_required( "addr", &addr );
		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		result = udma_flash_erase(0,0,addr,2);
		sprintf(message,"FLASH all erase = %s\n",
				result ? "Fail" : "Pass");
		dbg_str(message);
		vPortFree(message);
}
static uint8_t flash_sector_erase (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		int errors = 0;
		int addr,i;
		uint8_t result;
		message  = pvPortMalloc(80);
		CLI_uint32_required( "addr", &addr );
		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		result = udma_flash_erase(0,0,addr,1);
		sprintf(message,"FLASH sector 0x%x = %s\n", addr,
				result ? "Fail" : "Pass");
		dbg_str(message);
		vPortFree(message);
}
static uint8_t flash_subsector_erase (const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		int errors = 0;
		int addr,i;
		uint8_t result;
		message  = pvPortMalloc(80);
		CLI_uint32_required( "addr", &addr );
		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		result = udma_flash_erase(0,0,addr,0);
		sprintf(message,"FLASH subsector 0x%x = %s\n", addr,
				result ? "Fail" : "Pass");
		dbg_str(message);
		vPortFree(message);
}
static void flash_readid(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		int errors = 0;
		int i;
		union {
			uint32_t w;
			uint8_t b[4];
		} result ;
		message  = pvPortMalloc(80);
		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		result.w = udma_flash_readid(0,0);
		sprintf(message,"FLASH read ID results = %02x %02x %02x %02x\n",
				result.b[0],result.b[1],result.b[2],result.b[3]);
		dbg_str(message);
		vPortFree(message);
}


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
