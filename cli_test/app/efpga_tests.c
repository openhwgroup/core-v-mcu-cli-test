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
static void ram_32bit_16bit_8bit_test(const struct cli_cmd_entry *pEntry);
// EFPGA menu
const struct cli_cmd_entry efpga_cli_tests[] =
{
  CLI_CMD_SIMPLE( "tcdm", tcdm_test, "Tcdm 0-4 read/write tests" ),
  CLI_CMD_SIMPLE( "ram", ram_test, "32 bit ram tests" ),
  CLI_CMD_SIMPLE ( "dev", dev_test ,"dev_test"),
  CLI_CMD_SIMPLE( "rw", ram_32bit_16bit_8bit_test, "ram_rw_tests" ),
  CLI_CMD_TERMINATE()
};

typedef union {
	volatile unsigned char b[0x1000];
	volatile unsigned short hw[0x800];
	volatile unsigned int w[0x400];
} ram_word;

static unsigned int ram_rw_test(ram_word *ram_adr,volatile unsigned int *ram_ctl) {

	unsigned int i,err;
	char *message = pvPortMalloc(80);
	err = 0;
	unsigned char rdbuff_b[4] = { 0xef,0xbe,0xad,0xde };
	unsigned short int rdbuff_hw[2] = {0xbeef,0xdead};


	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 32bit and reading 8bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	*ram_ctl = 0x222;
	ram_adr->w[0] = 0xdeadbeef;
	sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
	dbg_str(message);
	for (i = 0; i < 4; i++) {
		sprintf(message,"ram_adr->b[%d]= 0x%0x\r\n",i, ram_adr->b[i]);
		dbg_str(message);
		if(ram_adr->b[i] != rdbuff_b[i]) err++;
	}
	memset(&ram_adr->w[0],0x0, 1);

	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 8bit and reading 32bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");

	*ram_ctl = 0x888;
	for (i = 0; i < 4; i++) {
		ram_adr->b[i] = rdbuff_b[i];
		sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
		dbg_str(message);
	}

	sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
	dbg_str(message);
	if(ram_adr->w[0] != 0xdeadbeef) err++;
	memset(&ram_adr->w[0],0x0, 1);

	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 32bit and reading 16bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");

	*ram_ctl = 0x111;
	ram_adr->w[0] = 0xdeadbeef;
	sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
	dbg_str(message);

	for (i = 0; i < 2; i++) {
		//rdbuff_hw[i] = efpga->m0_oper0.hw[i];
		sprintf(message,"ram_adr->hw[%d]= 0x%0x\r\n",i, ram_adr->hw[i]);
		dbg_str(message);
		if(ram_adr->hw[i] != rdbuff_hw[i]) err++;
	}
	memset(&ram_adr->w[0],0x0, 1);

	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 16bit and reading 32bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	*ram_ctl = 0x444;
	for (i = 0; i < 2; i++) {
		ram_adr->hw[i] = rdbuff_hw[i];
		sprintf(message,"ram_adr->hw[%d]= 0x%0x\r\n",i, ram_adr->hw[i]);
		dbg_str(message);
	}

	sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
	dbg_str(message);
	if(ram_adr->w[0] != 0xdeadbeef) err++;
	memset(&ram_adr->w[0],0x0, 1);

	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 8bit and reading 16bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	*ram_ctl = 0x999;
	for (i = 0; i < 4; i++) {
		ram_adr->b[i] = rdbuff_b[i];
		sprintf(message,"ram_adr->b[%d]= 0x%0x\r\n",i, ram_adr->b[i]);
		dbg_str(message);
	}
	for (i = 0; i < 2; i++) {
		//rdbuff_hw[i] = efpga->m0_oper0.hw[i];
		sprintf(message,"ram_adr->hw[%d]= 0x%0x\r\n",i, ram_adr->hw[i]);
		dbg_str(message);
		if(ram_adr->hw[i] != rdbuff_hw[i]) err++;
	}
	memset(&ram_adr->w[0],0x0, 1);

	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 16bit and reading 8bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	*ram_ctl = 0x666;
	for (i = 0; i < 2; i++) {
		ram_adr->hw[i] = rdbuff_hw[i];
		sprintf(message,"ram_adr->hw[%d]= 0x%0x\r\n",i, ram_adr->hw[i]);
		dbg_str(message);
	}

	for (i = 0; i < 4; i++) {
		sprintf(message,"ram_adr->b[%d]= 0x%0x\r\n",i, ram_adr->b[i]);
		dbg_str(message);
		if(ram_adr->b[i] != rdbuff_b[i]) err++;
	}
	memset(&ram_adr->w[0],0x0, 1);
	/*
			for (i = 0; i < 4; i++) {
				efpga->m0_oper0[i] = (rdbuff1 +i*4);
				sprintf(message,"m0_oper0[%d]= 0x%0x\r\n",i, efpga->m0_oper0[i]);
				dbg_str(message);
			}
	*/
	vPortFree(message);
	return err;

}

static void ram_32bit_16bit_8bit_test(const struct cli_cmd_entry *pEntry)
{
		(void)pEntry;
	    // Add functionality here
		char *message;
		apb_soc_ctrl_typedef *soc_ctrl;
		efpga_typedef *efpga;
		ram_word *ram_addr;
		unsigned int i;
		unsigned int test_no;
		volatile unsigned int *ram_ctrl;
		soc_ctrl = (apb_soc_ctrl_typedef*)0x1a104000;
		soc_ctrl->rst_efpga = 0xf;  //release efpga reset
		soc_ctrl->ena_efpga = 0x7f; // enable all interfaces
		message  = pvPortMalloc(80);
		efpga = (efpga_typedef*)0x1a300000;  // base address of efpga
		test_no = 1;
		do {
		switch(test_no) {
		case 1:
			ram_ctrl = (volatile unsigned int *)&efpga->m0_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m0_oper0);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) {
				dbg_str("m0_oper0_6_rw_test: **Failed**\n\r\r\r");
			} else {
				dbg_str("m0_oper0_6_rw_test: ##Passed##\n\r\r\r");
			}

			break;
		case 2:
			ram_ctrl = (volatile unsigned int *)&efpga->m0_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m0_oper1);
			ram_rw_test(ram_addr,ram_ctrl);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) {
				dbg_str("m0_oper1_6_rw_test: **Failed**\n\r\r\r");
			} else {
				dbg_str("m0_oper1_6_rw_test: ##Passed##\n\r\r\r");
			}

			break;
		case 3:
			ram_ctrl = (volatile unsigned int *)&efpga->m0_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m0_coef);
			ram_rw_test(ram_addr,ram_ctrl);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) {
				dbg_str("m0_coef_6_rw_tests: **Failed**\n\r\r\r");
			} else {
				dbg_str("m0_coef_6_rw_tests: ##Passed##\n\r\r\r");
			}
			break;
		case 4:
			ram_ctrl = (volatile unsigned int *)&efpga->m1_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m1_oper0);
			ram_rw_test(ram_addr,ram_ctrl);

			if( ram_rw_test(ram_addr,ram_ctrl) != 0) {
				dbg_str("m1_oper0_6_rw_tests: **Failed**\n\r\r\r");
			} else {
				dbg_str("m1_oper0_6_rw_tests: ##Passed##\n\r\r\r");
			}
			break;
		case 5:
			ram_ctrl = (volatile unsigned int *)&efpga->m1_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m1_oper1);
			ram_rw_test(ram_addr,ram_ctrl);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) {
				dbg_str("m1_oper1_6_rw_tests: **Failed**\n\r\r\r");
			} else {
				dbg_str("m1_oper1_6_rw_tests: ##Passed##\n\r\r\r");
			}
			break;
		case 6:
			ram_ctrl = (volatile unsigned int *)&efpga->m1_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m1_coef);
			ram_rw_test(ram_addr,ram_ctrl);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) {
				dbg_str("m1_coef_6_rw_tests: **Failed**\n\r\r\r");
			} else {
				dbg_str("m1_coef_6_rw_tests: ##Passed##\n\r\r\r");
			}
			break;
		default:
			break;
		}
		test_no ++;
	}while(test_no < 7);

	//dbg_str(message);
	vPortFree(message);
}

static void dev_test(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		uint32_t offset;
		apb_soc_ctrl_typedef *soc_ctrl;
		efpga_typedef *efpga;
		int errors = 0;
		unsigned int i, data1, data2;
		soc_ctrl = (apb_soc_ctrl_typedef*)0x1a104000;
		soc_ctrl->rst_efpga = 0xf;  //release efpga reset
		soc_ctrl->ena_efpga = 0x7f; // enable all interfaces
		message  = pvPortMalloc(80);
		efpga = (efpga_typedef*)0x1a300000;  // base address of efpga
		efpga->m0_ram_ctl = 0x0; //32b w, 8b r
		efpga->m0_m0_odata = 0x11111111;
		efpga->m0_oper0.w[0] = 0x87654321;
		efpga->m0_coef.w[0] = 5;
		efpga->m0_cdata = 0x87654321 ;
		data1 = efpga->m0_m0_odata;
		data2 = efpga->m0_cdata;

		efpga->m0_m0_ctl = 0x80000000;
		efpga->m0_m0_ctl = 0x0;
		sprintf(message,"data1 = 0x%0x\r\ndata2 = 0x%0x\r\n",data1, data2);
		dbg_str(message);
		sprintf(message,"epga->m0_m0_data_out = %08x\r\n",efpga->m0_m0_data_out);
		dbg_str(message);
		for (i = 0 ; i < 40; i++) {
			efpga->m0_m0_clken = 0xf;
			sprintf(message,"epga->m0_m0_data_out = %08d\r\n",efpga->m0_m0_data_out);
			dbg_str(message);
		}
		efpga->m0_m0_ctl = 0x80000000;
		efpga->m0_m0_ctl = 0x00003000;
		for (i = 0 ; i < 40; i++) {
			efpga->m0_m0_clken = 0xf;
			sprintf(message,"epga->m0_m0_data_out = %08x\r\n",efpga->m0_m0_data_out);
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
			efpga->m0_oper0.w[i] = 0;
			efpga->m0_oper1.w[i] = 0;
			efpga->m0_coef.w[i] = 0;
			efpga->m1_oper0.w[i] = 0;
			efpga->m1_oper1.w[i] = 0;
			efpga->m1_coef.w[i] = 0;
		}
		for (i = 512; i < 1024; i++) { // expect 0xffffffff in next 512 locations
			if (efpga->m0_oper0.w[i] != 0xffffffff) errors++;
			if (efpga->m0_oper1.w[i] != 0xffffffff) errors++;
			if (efpga->m0_coef.w[i] != 0xffffffff) errors++;
			if (efpga->m1_oper0.w[i] != 0xffffffff) errors++;
			if (efpga->m1_oper1.w[i] != 0xffffffff) errors++;
			if (efpga->m1_coef.w[i] != 0xffffffff) errors++;
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
			efpga->m0_oper0.w[i] = i;
		}
		for (i = 0; i < 512; i++) {
			if (efpga->m0_oper0.w[i+512] != ~i) {
				if (errors++ < 10) {
					sprintf(message,"m0_oper0[%d] = %x\r\n",
							i, efpga->m0_oper0.w[i]);
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
			efpga->m0_oper1.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m0_oper1.w[i+512] != ~i)
			if (errors++ < 10) {
				sprintf(message,"m0_oper1[%d] = %x\r\n",
						i, efpga->m0_oper0.w[i]);
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
			efpga->m0_coef.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m0_coef.w[i+512] != ~i) errors++;
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
			efpga->m1_oper0.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_oper0.w[i+512] != ~i) errors++;
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
			efpga->m1_oper1.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_oper1.w[i+512] != ~i) errors++;
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
			efpga->m1_coef.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_coef.w[i+512] != ~i) errors++;
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
			efpga->m0_oper0.w[i] = i;
			efpga->m0_oper1.w[i] = i+0x10;
			efpga->m1_oper0.w[i] = i+0x20;
			efpga->m1_oper1.w[i] = i+0x30;
		}
		soc_ctrl->control_in = 0x10000f;
		vTaskDelay(1);
		for (i = 0;i < 0x40;i = i+1) {
			efpga->m0_oper0.w[i] = 0;
			efpga->m0_oper1.w[i] = 0;
			efpga->m1_oper0.w[i] = 0;
			efpga->m1_oper1.w[i] = 0;
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
			j = efpga->m0_oper0.w[i];
			else if (i < 0x20)
			j = efpga->m0_oper1.w[i-0x10];
			else if (i < 0x30)
			j = efpga->m1_oper0.w[i-0x20];
			else
			j = efpga->m1_oper1.w[i-0x30];
			if (j != i) {
				errors++;
        sprintf(message,"mX_operY  = %x expected %x \r\n", j, i);
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
