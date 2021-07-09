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
static void program (const struct cli_cmd_entry *pEntry);

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
  CLI_CMD_SIMPLE( "program", program, "Program <filename>"),
  CLI_CMD_TERMINATE()
};

static void program (const struct cli_cmd_entry *pEntry) {
char*  pzArg = NULL;
	(void)pEntry;
	union {
		int d32;
		char d8[4];
	} sdata;
	char type = 0;
	int count = 0;
	int size = 0;
	int i, erase_addr;
	uint32_t fl_addr;
	// Add functionality here
	CLI_string_ptr_required("Loading file: ", &pzArg);
	CLI_uint32_required( "addr", &fl_addr );
		if (pzArg != NULL) {
			udma_uart_writeraw(1, 5, "Load ");
			udma_uart_writeraw(1, strlen(pzArg), pzArg);
			udma_uart_writeraw(1,2,"\r\n");
			type = 0;
			while (type != 'z') {
				type = uart_getchar(1);
				for (i = 0; i < 4; i++)
					sdata.d8[i]= uart_getchar(1);
				if (type == 'C') {
					udma_flash_write(0, 0, fl_addr, &sdata.d32, 4);
					count += 4;
					if ((count & 0x3ff) == 0)
						dbg_str(".");
					fl_addr += 4;
				}
				else if (type == 's') {
					dbg_str("Expecting ");
					dbg_hex32(sdata.d32);
					dbg_str(" bytes\r\n");
					erase_addr = fl_addr & ~0xfff;
					while (sdata.d32 > 0) {
						if(sdata.d32 & 0xfff)
							sdata.d32 -= sdata.d32 & 0xfff;
						else
							sdata.d32 -= 0x1000;
						dbg_str("Erasing 4k page at ");
						dbg_hex32(erase_addr);
						dbg_str("\r");
						udma_flash_erase(0, 0, erase_addr, 0); // 4k Sector erase
						erase_addr += 0x1000;
					}
					dbg_str("\r\n");
				}
				if (type != 'z')
					udma_uart_writeraw(1,1,"c");
			}
			dbg_str_hex32("\r\nReceived Bytes",count);
	}
}

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
				result ? "Pass" : "Fail");
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
				result ? "Pass" : "Fail");
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
				result ? "Pass" : "Fail");
		dbg_str(message);
		vPortFree(message);
}
static void flash_readid(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	// Add functionality here
	union {
		uint32_t w;
		uint8_t b[4];
	} result ;

	udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
	result.w = udma_flash_readid(0,0);
	CLI_printf("FLASH read ID results = 0x%08x %02x %02x %02x %02x\n",
			result.w, result.b[0],result.b[1],result.b[2],result.b[3]);
	if( result.w == 0x1019ba20 )
		CLI_printf("<<PASSED>>\n");
	else
		CLI_printf("<<FAILED>>\n");
}


static void qspi_read(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here

		uint8_t read_data[8] = {0xff,0xfe,0xfc,0xf8,0xf0,0xe0,0xc0,0x80};
		uint8_t lRead_data[8] = {0};

		udma_qspim_control((uint8_t) 0, (udma_qspim_control_type_t) kQSPImReset , (void*) 0);
		udma_qspim_read(0, 0, 4, lRead_data);
		CLI_printf("[0x%02x]/[0x%02x]/[0x%02x]/[0x%02x]\n",lRead_data[0], lRead_data[1], lRead_data[2], lRead_data[3]);

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
