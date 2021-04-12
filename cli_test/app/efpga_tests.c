/*
 * efpga_tests.c
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
#include "include/estruct.h"

static void tcdm_test(const struct cli_cmd_entry *pEntry);
// EFPGA menu
const struct cli_cmd_entry efpga_cli_tests[] =
{
  CLI_CMD_SIMPLE( "tcdm", tcdm_test, "Tcdm 0-4 read/write tests" ),
  CLI_CMD_TERMINATE()
};


static void tcdm_test(const struct cli_cmd_entry *pEntry)
{

    (void)pEntry;
    // Add functionality here
	uint32_t *scratch;
	char *message;
	uint32_t offset;
	apb_soc_ctrl_typedef *soc_ctrl;
	efpga_typedef *efpga;
	message  = pvPortMalloc(80);
	scratch = pvPortMalloc(256);
	efpga = (efpga_typedef*)0x1a300000;  // base address of efpga
	offset = (unsigned int)scratch & 0xFFFFF;
	soc_ctrl = (apb_soc_ctrl_typedef*)0x1a104000;
	soc_ctrl->rst_efpga = 0xf;
	soc_ctrl->ena_efpga = 0x7f;
	sprintf(message,"TCDM test - Scratch offset = %x\r\n", offset);
	dbg_str(message);
	{
		unsigned int i, j;
		i = efpga->test_read;
		sprintf(message,"eFPGA access test read = %x \r\n", i);
		dbg_str(message);
		soc_ctrl->control_in = 0x100000;
		efpga->tcdm0_ctl = 0x00000000 | offset;
		efpga->tcdm1_ctl = 0x00000000 | (offset+0x40);
		efpga->tcdm2_ctl = 0x00000000 | (offset+0x80);
		efpga->tcdm3_ctl = 0x00000000 | (offset+0xC0);
// Initialize eFPGA RAMs
		for (i = 0; i < 0x40; i = i + 1) {
			scratch[i] = 0;
			efpga->m0_oper0[i] = i;
			efpga->m0_oper1[i] = i+0x10;
			efpga->m1_oper0[i] = i+0x20;
			efpga->m1_oper1[i] = i+0x30;
		}
		soc_ctrl->control_in = 0x10000f;
		vTaskDelay(1);
		for (i = 0;i < 0x40;i = i+1) {
			efpga->m0_oper0[i] = 0;
			efpga->m0_oper1[i] = 0;
			efpga->m1_oper0[i] = 0;
			efpga->m1_oper1[i] = 0;
			j = scratch[i];
			scratch[i] = i;

			if (j != i) {
				sprintf(message,"scratch  = %x expected %x \r\n", j, i);
				dbg_str(message);
			}
		}
		soc_ctrl->control_in = 0x100000;
		efpga->tcdm0_ctl = 0x80000000 | offset;
		efpga->tcdm1_ctl = 0x80000000 | (offset+0x40);
		efpga->tcdm2_ctl = 0x80000000 | (offset+0x80);
		efpga->tcdm3_ctl = 0x80000000 | (offset+0xC0);
		soc_ctrl->control_in = 0x10000f;
		vTaskDelay(1);
		for (i = 0;i < 0x40;i = i+1) {
			if(i < 0x10)
			j = efpga->m0_oper0[i];
			else if (i < 0x20)
			j = efpga->m0_oper1[i-0x10];
			else if (i < 0x30)
			j = efpga->m1_oper0[i-0x20];
			else
			j = efpga->m1_oper1[i-0x30];
			if (j != i) {
				sprintf(message,"mX_operY  = %x expected %x \r\n", j, i);
				dbg_str(message);
			}
		}
	}
	vPortFree(scratch);
	vPortFree(message);
	}
