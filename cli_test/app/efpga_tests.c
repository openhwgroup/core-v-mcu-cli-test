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
static void ram_test(const struct cli_cmd_entry *pEntry);
static void dev_test(const struct cli_cmd_entry *pEntry);
// EFPGA menu
const struct cli_cmd_entry efpga_cli_tests[] =
{
  CLI_CMD_SIMPLE( "tcdm", tcdm_test, "Tcdm 0-4 read/write tests" ),
  CLI_CMD_SIMPLE( "ram", ram_test, "32 bit ram tests" ),
  CLI_CMD_SIMPLE ( "dev", dev_test ,""),
  CLI_CMD_TERMINATE()
};
static void dev_test(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		uint32_t offset;
		apb_soc_ctrl_typedef *soc_ctrl;
		efpga_typedef *efpga;
		int errors = 0;
		int i;
		soc_ctrl = (apb_soc_ctrl_typedef*)0x1a104000;
		soc_ctrl->rst_efpga = 0xf;  //release efpga reset
		soc_ctrl->ena_efpga = 0x7f; // enable all interfaces
		message  = pvPortMalloc(80);
		efpga = (efpga_typedef*)0x1a300000;  // base address of efpga
		efpga->m0_ram_ctl = 0x2; //32b w, 8b r
		efpga->m0_oper0.l[0] = 0x55aa6699;
		sprintf(message,"m0_oper.l[0] = %08x\r\n",efpga->m0_oper0.l[0]);
		dbg_str(message);
		for (i = 0 ; i < 4; i++) {
			sprintf(message,"m0_oper[%d] = %x\r\n",i, efpga->m0_oper0.b[i]);
			dbg_str(message);
		}
}

static void ram_test(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		uint32_t offset;
		apb_soc_ctrl_typedef *soc_ctrl;
		efpga_typedef *efpga;
		int errors = 0;
		int i;
		soc_ctrl = (apb_soc_ctrl_typedef*)0x1a104000;
		soc_ctrl->rst_efpga = 0xf;  //release efpga reset
		soc_ctrl->ena_efpga = 0x7f; // enable all interfaces
		message  = pvPortMalloc(80);
		efpga = (efpga_typedef*)0x1a300000;  // base address of efpga
		// Init all rams to 0
		sprintf(message,"Initializing 6 RAMs");
		dbg_str(message);
		for (i = 0; i < 512; i++) {
			efpga->m0_oper0.l[i] = 0;
			efpga->m0_oper1.l[i] = 0;
			efpga->m0_coef.l[i] = 0;
			efpga->m1_oper0.l[i] = 0;
			efpga->m1_oper1.l[i] = 0;
			efpga->m1_coef.l[i] = 0;
		}
		for (i = 512; i < 1024; i++) { // expect 0xffffffff in next 512 locations
			if (efpga->m0_oper0.l[i] != 0xffffffff) errors++;
			if (efpga->m0_oper1.l[i] != 0xffffffff) errors++;
			if (efpga->m0_coef.l[i] != 0xffffffff) errors++;
			if (efpga->m1_oper0.l[i] != 0xffffffff) errors++;
			if (efpga->m1_oper1.l[i] != 0xffffffff) errors++;
			if (efpga->m1_coef.l[i] != 0xffffffff) errors++;
		}
		if (errors == 0)
			sprintf(message," PASSED!\r\n");
		else
			sprintf(message," *** %d Test Failures\r\n",errors);
		errors = 0;
		dbg_str(message);
		sprintf(message,"Testing m0_oper0");
		dbg_str(message);
		for (i = 0; i < 512; i++) {
			efpga->m0_oper0.l[i] = i;
		}
		for (i = 0; i < 512; i++) {
			if (efpga->m0_oper0.l[i+512] != ~i) {
				if (errors++ < 10) {
					sprintf(message,"m0_oper0[%d] = %x\r\n",
							i, efpga->m0_oper0.l[i]);
					dbg_str(message);
				}
			}
		}
		if (errors == 0)
			sprintf(message," PASSED!\r\n");
		else
			sprintf(message," *** %d Test Failures\r\n",errors);
		dbg_str(message);
		errors = 0;
		sprintf(message,"Testing m0_oper1");
		dbg_str(message);
		for (i = 0 ; i < 512; i++) {
			efpga->m0_oper1.l[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m0_oper1.l[i+512] != ~i)
			if (errors++ < 10) {
				sprintf(message,"m0_oper1[%d] = %x\r\n",
						i, efpga->m0_oper0.l[i]);
				dbg_str(message);
			}
		}
		if (errors == 0)
			sprintf(message," PASSED!\r\n");
		else
			sprintf(message," *** %d Test Failures\r\n",errors);
		dbg_str(message);
		errors = 0;
		sprintf(message,"Testing m0_coef");
		dbg_str(message);
		for (i = 0 ; i < 512; i++) {
			efpga->m0_coef.l[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m0_coef.l[i+512] != ~i) errors++;
		}
		if (errors == 0)
			sprintf(message," PASSED!\r\n");
		else
			sprintf(message," *** %d Test Failures\r\n",errors);
		dbg_str(message);
		errors = 0;
		sprintf(message,"Testing m1_oper0");
		dbg_str(message);
		for (i = 0 ; i < 512; i++) {
			efpga->m1_oper0.l[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_oper0.l[i+512] != ~i) errors++;
		}
		if (errors == 0)
			sprintf(message," PASSED!\r\n");
		else
			sprintf(message," *** %d Test Failures\r\n",errors);
		dbg_str(message);
		errors = 0;
		sprintf(message,"Testing m1_oper1");
		dbg_str(message);
		for (i = 0 ; i < 512; i++) {
			efpga->m1_oper1.l[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_oper1.l[i+512] != ~i) errors++;
		}
		if (errors == 0)
			sprintf(message," PASSED!\r\n");
		else
			sprintf(message," *** %d Test Failures\r\n",errors);
		dbg_str(message);
		errors = 0;
		sprintf(message,"Testing m1_coef");
		dbg_str(message);
		for (i = 0 ; i < 512; i++) {
			efpga->m1_coef.l[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_coef.l[i+512] != ~i) errors++;
		}
		if (errors == 0)
			sprintf(message," PASSED!\r\n");
		else
			sprintf(message," *** %d Test Failures\r\n",errors);
		errors = 0;
		dbg_str(message);
		vPortFree(message);
}

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
	efpga->m0_ram_ctl = 0;
	efpga->m1_ram_ctl = 0;
	sprintf(message,"TCDM test - Scratch offset = %x\r\n", offset);
	dbg_str(message);
	{
		unsigned int i, j;
		int errors = 0;
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
			efpga->m0_oper0.l[i] = i;
			efpga->m0_oper1.l[i] = i+0x10;
			efpga->m1_oper0.l[i] = i+0x20;
			efpga->m1_oper1.l[i] = i+0x30;
		}
		soc_ctrl->control_in = 0x10000f;
		vTaskDelay(1);
		for (i = 0;i < 0x40;i = i+1) {
			efpga->m0_oper0.l[i] = 0;
			efpga->m0_oper1.l[i] = 0;
			efpga->m1_oper0.l[i] = 0;
			efpga->m1_oper1.l[i] = 0;
			j = scratch[i];
			scratch[i] = i;

			if (j != i) {
				errors++;
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
			j = efpga->m0_oper0.l[i];
			else if (i < 0x20)
			j = efpga->m0_oper1.l[i-0x10];
			else if (i < 0x30)
			j = efpga->m1_oper0.l[i-0x20];
			else
			j = efpga->m1_oper1.l[i-0x30];
			if (j != i) {
				errors++;
				sprintf(message,"mX_operY[%d]  = %x expected %x \r\n",i, j, i);
				dbg_str(message);
			}
		}
		if (errors == 0)
			sprintf(message,"PASSED!\r\n");
		else
			sprintf(message,"*** %d Test Failures\r\n",errors);
		dbg_str(message);
	}
	vPortFree(scratch);
	vPortFree(message);
	}
