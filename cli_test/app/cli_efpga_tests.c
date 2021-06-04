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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include "libs/cli/include/cli.h"
#include "libs/utils/include/dbg_uart.h"
#include "include/efpga_tests.h"
#include "FreeRTOS.h"
#include "task.h"

static void tcdm_test(const struct cli_cmd_entry *pEntry);
static void ram_test(const struct cli_cmd_entry *pEntry);
static void m_mltiply_test(const struct cli_cmd_entry *pEntry);
static void ram_32bit_16bit_8bit_test(const struct cli_cmd_entry *pEntry);
static void tcdm_task_start(const struct cli_cmd_entry *pEntry);
static void tcdm_task_stop(const struct cli_cmd_entry *pEntry);
static void efpga_autotest(const struct cli_cmd_entry *pEntry);

// EFPGA menu
const struct cli_cmd_entry efpga_cli_tests[] =
{
  CLI_CMD_SIMPLE( "tcdm", tcdm_test, "Tcdm0-4 r/w tests" ),
  CLI_CMD_SIMPLE( "tcdm_st", tcdm_task_start, "Tcdm start task" ),
  CLI_CMD_SIMPLE( "tcdm_sp", tcdm_task_stop, "Tcdm delete task" ),
  CLI_CMD_SIMPLE( "ram", ram_test, "32 bit ram tests" ),
  CLI_CMD_SIMPLE ( "mlt", m_mltiply_test ,"mltiply_test"),
  CLI_CMD_SIMPLE( "rw", ram_32bit_16bit_8bit_test, "ram_rw_tests" ),
  CLI_CMD_SIMPLE( "auto", efpga_autotest, "autotest" ),

  CLI_CMD_TERMINATE()
};

typedef union {
	volatile unsigned char b[0x1000];
	volatile unsigned short hw[0x800];
	volatile unsigned int w[0x400];
} ram_word;

typedef struct {
	volatile unsigned int *m_ctl;
	volatile unsigned int *m_clken;
	volatile unsigned int *m_odata;
	volatile unsigned int *m_cdata;
	volatile unsigned int *m_data_out;
}mlti_ctl;

xTaskHandle xHandleTcmdTest = NULL;

static unsigned int ram_rw_test(ram_word *ram_adr,volatile unsigned int *ram_ctl) {

	unsigned int i,err;
	char *message = pvPortMalloc(80);
	err = 0;
	unsigned char rdbuff_b[4] = { 0xef,0xbe,0xad,0xde };
	unsigned short int rdbuff_hw[2] = {0xbeef,0xdead};

#if EFPGA_DEBUG
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 32bit and reading 8bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif
	*ram_ctl = 0x222;
	ram_adr->w[0] = 0xdeadbeef;

#if EFPGA_DEBUG
	sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
	dbg_str(message);
#endif

	for (i = 0; i < 4; i++) {
#if EFPGA_DEBUG
		sprintf(message,"ram_adr->b[%d]= 0x%0x\r\n",i, ram_adr->b[i]);
		dbg_str(message);
#endif
		if(ram_adr->b[i] != rdbuff_b[i]) err++;
	}
	memset(&ram_adr->w[0],0x0, 1);

#if EFPGA_DEBUG
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 8bit and reading 32bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif

	*ram_ctl = 0x888;
	for (i = 0; i < 4; i++) {
		ram_adr->b[i] = rdbuff_b[i];
#if EFPGA_DEBUG
		sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
		dbg_str(message);
#endif
	}
#if EFPGA_DEBUG
	sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
	dbg_str(message);
#endif
	if(ram_adr->w[0] != 0xdeadbeef) err++;
	memset(&ram_adr->w[0],0x0, 1);

#if EFPGA_DEBUG
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 32bit and reading 16bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif

	*ram_ctl = 0x111;
	ram_adr->w[0] = 0xdeadbeef;
#if EFPGA_DEBUG
	sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
	dbg_str(message);
#endif
	for (i = 0; i < 2; i++) {
		//rdbuff_hw[i] = efpga->m0_oper0.hw[i];
#if EFPGA_DEBUG
		sprintf(message,"ram_adr->hw[%d]= 0x%0x\r\n",i, ram_adr->hw[i]);
		dbg_str(message);
#endif
		if(ram_adr->hw[i] != rdbuff_hw[i]) err++;
	}
	memset(&ram_adr->w[0],0x0, 1);

#if EFPGA_DEBUG
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 16bit and reading 32bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif
	*ram_ctl = 0x444;
	for (i = 0; i < 2; i++) {
		ram_adr->hw[i] = rdbuff_hw[i];

#if EFPGA_DEBUG
		sprintf(message,"ram_adr->hw[%d]= 0x%0x\r\n",i, ram_adr->hw[i]);
		dbg_str(message);
#endif
	}
#if EFPGA_DEBUG
	sprintf(message,"ram_adr->w[0]= 0x%0x\r\n",ram_adr->w[0]);
	dbg_str(message);
#endif
	if(ram_adr->w[0] != 0xdeadbeef) err++;
	memset(&ram_adr->w[0],0x0, 1);
#if EFPGA_DEBUG
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 8bit and reading 16bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif
	*ram_ctl = 0x999;
	for (i = 0; i < 4; i++) {
		ram_adr->b[i] = rdbuff_b[i];
#if EFPGA_DEBUG
		sprintf(message,"ram_adr->b[%d]= 0x%0x\r\n",i, ram_adr->b[i]);
		dbg_str(message);
#endif
	}
	for (i = 0; i < 2; i++) {
		//rdbuff_hw[i] = efpga->m0_oper0.hw[i];
#if EFPGA_DEBUG
		sprintf(message,"ram_adr->hw[%d]= 0x%0x\r\n",i, ram_adr->hw[i]);
		dbg_str(message);
#endif
		if(ram_adr->hw[i] != rdbuff_hw[i]) err++;
	}
	memset(&ram_adr->w[0],0x0, 1);
#if EFPGA_DEBUG
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
	dbg_str("writing 16bit and reading 8bit Test\n\r\r\r");
	dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif
	*ram_ctl = 0x666;
	for (i = 0; i < 2; i++) {
		ram_adr->hw[i] = rdbuff_hw[i];
#if EFPGA_DEBUG
		sprintf(message,"ram_adr->hw[%d]= 0x%0x\r\n",i, ram_adr->hw[i]);
		dbg_str(message);
#endif
	}

	for (i = 0; i < 4; i++) {
#if EFPGA_DEBUG
		sprintf(message,"ram_adr->b[%d]= 0x%0x\r\n",i, ram_adr->b[i]);
		dbg_str(message);
#endif
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
		unsigned int errors = 0;
		volatile unsigned int *ram_ctrl;
		soc_ctrl = (apb_soc_ctrl_typedef*)APB_SOC_CTRL_BASE_ADDR;
		soc_ctrl->rst_efpga = 0xf;  //release efpga reset
		soc_ctrl->ena_efpga = 0x7f; // enable all interfaces
		message  = pvPortMalloc(80);
		efpga = (efpga_typedef*)EFPGA_BASE_ADDR;  // base address of efpga
		test_no = 1;
		do {
		switch(test_no) {
		case 1:
			ram_ctrl = (volatile unsigned int *)&efpga->m0_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m0_oper0);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0){
				dbg_str("m0_oper0_6_rw_test: <<FAILED>>\r\n");
			} else {
				dbg_str("m0_oper0_6_rw_test: <<PASSED>>\r\n");
			}
#endif
			break;
		case 2:
			ram_ctrl = (volatile unsigned int *)&efpga->m0_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m0_oper1);
			ram_rw_test(ram_addr,ram_ctrl);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0) {
				dbg_str("m0_oper1_6_rw_test: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m0_oper1_6_rw_test: <<PASSED>>\n\r\r\r");
			}
#endif
			break;
		case 3:
			ram_ctrl = (volatile unsigned int *)&efpga->m0_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m0_coef);
			ram_rw_test(ram_addr,ram_ctrl);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0) {
				dbg_str("m0_coef_6_rw_tests: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m0_coef_6_rw_tests: <<PASSED>>\n\r\r\r");
			}
#endif
			break;
		case 4:
			ram_ctrl = (volatile unsigned int *)&efpga->m1_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m1_oper0);
			ram_rw_test(ram_addr,ram_ctrl);

			if( ram_rw_test(ram_addr,ram_ctrl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0) {
				dbg_str("m1_oper0_6_rw_tests: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m1_oper0_6_rw_tests: <<PASSED>>\n\r\r\r");
			}
#endif
			break;
		case 5:
			ram_ctrl = (volatile unsigned int *)&efpga->m1_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m1_oper1);
			ram_rw_test(ram_addr,ram_ctrl);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0)errors++;
#if EFPGA_ERROR
			if(errors != 0) {
				dbg_str("m1_oper1_6_rw_tests: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m1_oper1_6_rw_tests: <<PASSED>>\n\r\r\r");
			}
#endif
			break;
		case 6:
			ram_ctrl = (volatile unsigned int *)&efpga->m1_ram_ctl;
			ram_addr = (ram_word *)&(efpga->m1_coef);
			ram_rw_test(ram_addr,ram_ctrl);
			if( ram_rw_test(ram_addr,ram_ctrl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0) {
				dbg_str("m1_coef_6_rw_tests: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m1_coef_6_rw_tests: <<PASSED>>\n\r\r\r");
			}
#endif
			break;
		default:
			break;
		}
		test_no ++;
	}while(test_no < 7);

	(errors == 0)?(dbg_str("RAMs RW TEST:<<PASSED>>\r\n")):(dbg_str("RAMs RW TEST:<<FAILED>>\r\n"));
	vPortFree(message);
}




static unsigned int mltiply_test(ram_word *ram_adr1, ram_word *ram_adr2, mlti_ctl *mctl)
{

		char *message;
		unsigned int errors = 0;
		unsigned int i, test_type, exp_data_out, data_out;
		unsigned int mlt_type;
		message  = pvPortMalloc(80);
		test_type = 1;
	do{
		switch(test_type) {
		case 1:
			mlt_type = 0;
			do {
				*mctl->m_ctl = 0x80000000;
				*mctl->m_ctl = 0x0;
				errors = 0;

				*mctl->m_odata = 0x2;
				*mctl->m_cdata = 0x3;
				*mctl->m_ctl = (0x40000 | ((mlt_type & 0x3) << 12));
				exp_data_out = (*mctl->m_odata) * (*mctl->m_cdata);
				if ((*mctl->m_data_out) != 0x0) errors ++;
				*mctl->m_clken = 0xf;
				data_out = (*mctl->m_data_out);
				if (mlt_type != 3) {
					if(data_out != exp_data_out) errors ++;
				}
#if EFPGA_DEBUG
				sprintf(message,"mctl->m_data_out = %08x\r\n",data_out);
				dbg_str(message);
#endif
				for (i = 0 ; i < 3; i++) {
					*mctl->m_clken = 0xf;
					data_out = data_out + exp_data_out;
					if (mlt_type != 3) {
						if((*mctl->m_data_out) !=  data_out) errors ++;
					}
#if EFPGA_DEBUG
					sprintf(message,"mctl->m_data_out = %08x\r\n",*mctl->m_data_out);
					dbg_str(message);
#endif
				}
#if EFPGA_DEBUG
				(errors)? dbg_str("*** Test: failed***\n"): dbg_str("###Test: passed###\n");
#endif
				mlt_type ++;
			}while(mlt_type < 4);
			break;
		case 2:
			mlt_type = 0;
			do {
				*mctl->m_ctl = 0x80000000;
				*mctl->m_ctl = 0x0;
				errors = 0;

				ram_adr1->w[0] = 0x4;
				ram_adr2->w[0] = 0x5;
				*mctl->m_ctl = (0x4c000 | ((mlt_type & 0x3) << 12));
				exp_data_out = (ram_adr1->w[0]) * (ram_adr2->w[0]);
				if ((*mctl->m_data_out) != 0x0) errors ++;
				*mctl->m_clken = 0xf;
				data_out = *mctl->m_data_out;
				if (mlt_type != 3) {
					if(data_out != exp_data_out) errors ++;
				}
#if EFPGA_DEBUG
				sprintf(message,"mctl->m_data_out = %08x\r\n",data_out);
				dbg_str(message);
#endif

				for (i = 0 ; i < 3; i++) {
					*mctl->m_clken = 0xf;
					data_out = data_out + exp_data_out;
					if (mlt_type != 3) {
						if((*mctl->m_data_out) !=  data_out) errors ++;

					}
#if EFPGA_DEBUG
					sprintf(message,"mctl->m_data_out = %08x\r\n",*mctl->m_data_out);
					dbg_str(message);
#endif
				}
#if EFPGA_DEBUG
			(errors)? dbg_str("*** Test: failed***\n"): dbg_str("###Test: passed###\n");
#endif
			mlt_type ++;
			}while(mlt_type < 4);
			break;
		default:
			break;
		}
		test_type ++;
	}while(test_type <= 2 );

	vPortFree(message);
	return errors;
}
static void m_mltiply_test(const struct cli_cmd_entry *pEntry)
{

	    (void)pEntry;
	    // Add functionality here
		char *message;
		apb_soc_ctrl_typedef *soc_ctrl;
		efpga_typedef *efpga;
		ram_word *ram_addr1, *ram_addr2;
		mlti_ctl *mt_ctl;
		unsigned int test_no;
		unsigned int errors = 0;
		soc_ctrl = (apb_soc_ctrl_typedef*)APB_SOC_CTRL_BASE_ADDR;
		soc_ctrl->rst_efpga = 0xf;  //release efpga reset
		soc_ctrl->ena_efpga = 0x7f; // enable all interfaces
		message  = pvPortMalloc(80);
		efpga = (efpga_typedef*)EFPGA_BASE_ADDR;  // base address of efpga
		efpga->m0_ram_ctl = 0x0; //32b w, 8b r
		test_no = 1;
		do {
		switch(test_no) {
		case 1:
#if EFPGA_DEBUG
			dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
			dbg_str("M0_M0_Multiplier Test\n\r\r\r");
			dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif
			mt_ctl->m_ctl = (volatile unsigned int *)&efpga->m0_m0_ctl;
			mt_ctl->m_clken = (volatile unsigned int *)&efpga->m0_m0_clken;
			mt_ctl->m_odata = (volatile unsigned int *)&efpga->m0_m0_odata;
			mt_ctl->m_cdata = (volatile unsigned int *)&efpga->m0_m0_cdata;
			mt_ctl->m_data_out = (volatile unsigned int *)&efpga->m0_m0_data_out;
			ram_addr1 = (ram_word *)&(efpga->m0_oper0);
			ram_addr2 = (ram_word *)&(efpga->m0_coef);
			if( mltiply_test(ram_addr1, ram_addr2, mt_ctl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0) {
				dbg_str("m0_m0_ctl_operation: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m0_m0_ctl_operation: <<PASSED>>\n\r\r\r");
			}
#endif
			break;
		case 2:
#if EFPGA_DEBUG
			dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
			dbg_str("M0_M1_Multiplier Test\n\r\r\r");
			dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif
			mt_ctl->m_ctl = (volatile unsigned int *)&efpga->m0_m1_ctl;
			mt_ctl->m_clken = (volatile unsigned int *)&efpga->m0_m1_clken;
			mt_ctl->m_odata = (volatile unsigned int *)&efpga->m0_m1_odata;
			mt_ctl->m_cdata = (volatile unsigned int *)&efpga->m0_m1_cdata;
			mt_ctl->m_data_out = (volatile unsigned int *)&efpga->m0_m1_data_out;
			ram_addr1 = (ram_word *)&(efpga->m0_oper1);
			ram_addr2 = (ram_word *)&(efpga->m0_coef);
			if( mltiply_test(ram_addr1, ram_addr2, mt_ctl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0){
				dbg_str("m0_m1_ctl_operation: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m0_m1_ctl_operation: <<PASSED>>\n\r\r\r");
			}
#endif
			break;

		case 3:
#if EFPGA_DEBUG
			dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
			dbg_str("M1_M0_Multiplier Test\n\r\r\r");
			dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif
			mt_ctl->m_ctl = (volatile unsigned int *)&efpga->m1_m0_ctl;
			mt_ctl->m_clken = (volatile unsigned int *)&efpga->m1_m0_clken;
			mt_ctl->m_odata = (volatile unsigned int *)&efpga->m1_m0_odata;
			mt_ctl->m_cdata = (volatile unsigned int *)&efpga->m1_m0_cdata;
			mt_ctl->m_data_out = (volatile unsigned int *)&efpga->m1_m0_data_out;
			ram_addr1 = (ram_word *)&(efpga->m1_oper0);
			ram_addr2 = (ram_word *)&(efpga->m1_coef);
			if( mltiply_test(ram_addr1, ram_addr2, mt_ctl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0) {
				dbg_str("m1_m0_ctl_operation: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m1_m0_ctl_operation: <<PASSED>>\n\r\r\r");
			}
#endif
			break;

		case 4:
#if EFPGA_DEBUG
			dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
			dbg_str("M1_M1_Multiplier Test\n\r\r\r");
			dbg_str("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\r\r\r");
#endif
			mt_ctl->m_ctl = (volatile unsigned int *)&efpga->m1_m1_ctl;
			mt_ctl->m_clken = (volatile unsigned int *)&efpga->m1_m1_clken;
			mt_ctl->m_odata = (volatile unsigned int *)&efpga->m1_m1_odata;
			mt_ctl->m_cdata = (volatile unsigned int *)&efpga->m1_m1_cdata;
			mt_ctl->m_data_out = (volatile unsigned int *)&efpga->m1_m1_data_out;
			ram_addr1 = (ram_word *)&(efpga->m1_oper1);
			ram_addr2 = (ram_word *)&(efpga->m1_coef);
			if( mltiply_test(ram_addr1, ram_addr2, mt_ctl) != 0) errors++;
#if EFPGA_ERROR
			if(errors != 0)  {
				dbg_str("m1_m1_ctl_operation: <<FAILED>>\n\r\r\r");
			} else {
				dbg_str("m1_m1_ctl_operation: <<PASSED>>\n\r\r\r");
			}
#endif
						break;

			break;

		default:
			break;
		}
		test_no ++;
	}while(test_no < 5);

	(errors == 0)?(dbg_str("MULTIPLIER TEST:<<PASSED>>\r\n")):(dbg_str("MULTIPLIER TEST:<<FAILED>>\r\n"));
	vPortFree(message);
}


static void ram_test(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	    // Add functionality here
		char *message;
		uint32_t offset;
		apb_soc_ctrl_typedef *soc_ctrl;
		efpga_typedef *efpga;
		unsigned int errors = 0;
		unsigned int global_err = 0;
		int i;
		soc_ctrl = (apb_soc_ctrl_typedef*)APB_SOC_CTRL_BASE_ADDR;
		soc_ctrl->rst_efpga = 0xf;  //release efpga reset
		soc_ctrl->ena_efpga = 0x7f; // enable all interfaces
		message  = pvPortMalloc(80);
		efpga = (efpga_typedef*)EFPGA_BASE_ADDR;  // base address of efpga
		// Init all rams to 0
#if EFPGA_ERROR
		sprintf(message,"Testing 6RAMs :");
		dbg_str(message);
#endif
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
		global_err += errors;
#if EFPGA_ERROR
		if (errors == 0)
			sprintf(message," <<PASSED>>\r\n");
		else
			sprintf(message," <<FAILED>>\r\n");
		dbg_str(message);
#endif
		errors = 0;
#if EFPGA_ERROR
		sprintf(message,"Testing m0_oper0");
		dbg_str(message);
#endif
		for (i = 0; i < 512; i++) {
			efpga->m0_oper0.w[i] = i;
		}
		for (i = 0; i < 512; i++) {
			if (efpga->m0_oper0.w[i+512] != ~i) {
				if (errors++ < 10) {
#if EFPGA_DEBUG
					sprintf(message,"m0_oper0[%d] = %x\r\n",i,efpga->m0_oper0.w[i]);
					dbg_str(message);
#endif
				}
			}
		}
		global_err += errors;

#if EFPGA_ERROR
		if (errors == 0)
			sprintf(message," <<PASSED>>\r\n");
		else
			sprintf(message," <<FAILED>>\r\n");
		dbg_str(message);
#endif
		errors = 0;
#if EFPGA_ERROR
		sprintf(message,"Testing m0_oper1");
		dbg_str(message);
#endif
		for (i = 0 ; i < 512; i++) {
			efpga->m0_oper1.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m0_oper1.w[i+512] != ~i)
			if (errors++ < 10) {
#if EFPGA_DEBUG
				sprintf(message,"m0_oper1[%d] = %x\r\n",i,efpga->m0_oper0.w[i]);
				dbg_str(message);
#endif
			}
		}
		global_err += errors;
#if EFPGA_ERROR
		if (errors == 0)
			sprintf(message," <<PASSED>>\r\n");
		else
			sprintf(message," <<FAILED>>\r\n");
		dbg_str(message);
#endif
		errors = 0;

#if EFPGA_ERROR
		sprintf(message,"Testing m0_coef");
		dbg_str(message);
#endif
		for (i = 0 ; i < 512; i++) {
			efpga->m0_coef.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m0_coef.w[i+512] != ~i) errors++;
		}
		global_err += errors;
#if EFPGA_ERROR
		if (errors == 0)
			sprintf(message," <<PASSED>>\r\n");
		else
			sprintf(message," <<FAILED>>\r\n");
		dbg_str(message);
#endif
		errors = 0;
#if EFPGA_ERROR
		sprintf(message,"Testing m1_oper0");
		dbg_str(message);
#endif
		for (i = 0 ; i < 512; i++) {
			efpga->m1_oper0.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_oper0.w[i+512] != ~i) errors++;
		}
		global_err += errors;
#if EFPGA_ERROR
		if (errors == 0)
			sprintf(message," <<PASSED>>\r\n");
		else
			sprintf(message," <<FAILED>>\r\n");
		dbg_str(message);
#endif
		errors = 0;
#if EFPGA_ERROR
		sprintf(message,"Testing m1_oper1");
		dbg_str(message);
#endif
		for (i = 0 ; i < 512; i++) {
			efpga->m1_oper1.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_oper1.w[i+512] != ~i) errors++;
		}
		global_err += errors;
#if EFPGA_ERROR
		if (errors == 0)
			sprintf(message," <<PASSED>>\r\n");
		else
			sprintf(message," <<FAILED>>\r\n");
		dbg_str(message);
#endif
		errors = 0;
#if EFPGA_ERROR
		sprintf(message,"Testing m1_coef");
		dbg_str(message);
#endif
		for (i = 0 ; i < 512; i++) {
			efpga->m1_coef.w[i] = i;
		}
		for (i = 0 ; i < 512; i++) {
			if (efpga->m1_coef.w[i+512] != ~i) errors++;
		}
		global_err += errors;
#if EFPGA_ERROR
		if (errors == 0)
			sprintf(message," <<PASSED>>\r\n");
		else
			sprintf(message," <<FAILED>>\r\n");
		dbg_str(message);
#endif
		errors = 0;
		(global_err == 0)?(dbg_str("RAM TEST: <<PASSED>>\r\n")):(dbg_str(" RAM TEST: <<FAILED>>\r\n"));
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
	efpga = (efpga_typedef*)EFPGA_BASE_ADDR;  // base address of efpga
	offset = (unsigned int)scratch & 0xFFFFF;
	soc_ctrl = (apb_soc_ctrl_typedef*)APB_SOC_CTRL_BASE_ADDR;
	soc_ctrl->control_in = 0;
	soc_ctrl->rst_efpga = 0xf;
	soc_ctrl->ena_efpga = 0x7f;

	efpga->m0_ram_ctl = 0;
	efpga->m1_ram_ctl = 0;
#if EFPGA_DEBUG
	sprintf(message,"TCDM test - Scratch offset = %x\r\n", offset);
	dbg_str(message);
#endif
	{
		unsigned int i, j;
		unsigned int errors = 0;
		unsigned int global_err = 0;
		i = efpga->test_read;
#if EFPGA_DEBUG
		sprintf(message,"eFPGA access test read = %x \r\n", i);
		dbg_str(message);
#endif
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
		vTaskDelay(10);
		for (i = 0;i < 0x40;i = i+1) {
			efpga->m0_oper0.w[i] = 0;
			efpga->m0_oper1.w[i] = 0;
			efpga->m1_oper0.w[i] = 0;
			efpga->m1_oper1.w[i] = 0;
			j = (volatile)scratch[i];
			scratch[i] = i;

			if (j != i) {
				errors++;

//#if EFPGA_DEBUG
				sprintf(message,"scratch  = %x expected %x \r\n", j, i);
				dbg_str(message);

//efpga tcdm				#endif
			}
		}
		global_err += errors;
#if EFPGA_ERROR
		if (errors == 0)
			dbg_str("eFPGA RAM Write Test: <<PASSED>>\r\n");
		else {
			dbg_str("eFPGA RAM Write Test: <<FAILED>>\r\n");
		}
#endif
		errors = 0;
		soc_ctrl->control_in = 0x100000;
		efpga->tcdm0_ctl = 0x80000000 | offset;
		efpga->tcdm1_ctl = 0x80000000 | (offset+0x40);
		efpga->tcdm2_ctl = 0x80000000 | (offset+0x80);
		efpga->tcdm3_ctl = 0x80000000 | (offset+0xC0);
		soc_ctrl->control_in = 0x10000f;
		vTaskDelay(10);
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
#if EFPGA_DEBUG
				sprintf(message,"mX_operY  = %x expected %x \r\n", j, i);
				dbg_str(message);
#endif
			}
		}
		global_err += errors;
#if EFPGA_ERROR
		if (errors == 0)
			dbg_str("eFPGA RAM Read Test: <<PASSED>>\r\n");
		else {
#if EFPGA_DEBUG
			sprintf(message,"*** %d Test Failures\r\n",errors);
			dbg_str(message);
#endif
			dbg_str("eFPGA RAM Read Test: FAILED\r\n");
		}
#endif
		(global_err == 0)?(dbg_str("TCDM TEST: <<PASSED>>\r\n")):(dbg_str("TCDM TEST: <<FAILED>>\r\n"));
	}
	vPortFree(scratch);
	vPortFree(message);
}

void tcdm_task( void *pParameter )
{

    (void)pParameter;
    // Add functionality here
	uint32_t *scratch;
	char *message;
	uint32_t offset;
	apb_soc_ctrl_typedef *soc_ctrl;
	efpga_typedef *efpga;
	message  = pvPortMalloc(80);
	scratch = pvPortMalloc(256);
	efpga = (efpga_typedef*)EFPGA_BASE_ADDR;  // base address of efpga
	offset = (unsigned int)scratch & 0xFFFFF;
	soc_ctrl = (apb_soc_ctrl_typedef*)APB_SOC_CTRL_BASE_ADDR;
	soc_ctrl->rst_efpga = 0xf;
	soc_ctrl->ena_efpga = 0x7f;

#if EFPGA_DEBUG
	sprintf(message,"TCDM test - Scratch offset = %x\r\n", offset);
	dbg_str(message);
#endif
	for(;;){
		efpga->m0_ram_ctl = 0;
		efpga->m1_ram_ctl = 0;
		unsigned int i, j;
		int errors = 0;
		i = efpga->test_read;
#if EFPGA_DEBUG
		sprintf(message,"eFPGA access test read = %x \r\n", i);
		dbg_str(message);
#endif
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
#if EFPGA_DEBUG
				sprintf(message,"scratch  = %x expected %x \r\n", j, i);
				dbg_str(message);
#endif
			}
		}
		if (errors == 0)
			dbg_str("eFPGA RAM Write Test: <<PASSED>>\r\n");
		else {
			dbg_str("eFPGA RAM Write Test: <<FAILED>>\r\n");
		}
		errors = 0;
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
#if EFPGA_DEBUG
				sprintf(message,"mX_operY  = %x expected %x \r\n", j, i);
				dbg_str(message);
#endif
			}
		}
		if (errors == 0)
			dbg_str("eFPGA RAM Read Test: <<PASSED>>\r\n");
		else {
#if EFPGA_DEBUG
			sprintf(message,"*** %d Test Failures\r\n",errors);
			dbg_str(message);
#endif
			dbg_str("eFPGA RAM Read Test: <<FAILED>>\r\n");
		}
		vTaskDelay(1000);
	}
	vPortFree(scratch);
	vPortFree(message);
	}


static void tcdm_task_start(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	xTaskCreate ( tcdm_task, "tcdm_task", 1000, NULL, (UBaseType_t)(tskIDLE_PRIORITY+1), &xHandleTcmdTest);
	configASSERT( xHandleTcmdTest );

}


static void tcdm_task_stop(const struct cli_cmd_entry *pEntry)
{
	(void)pEntry;
	if(xHandleTcmdTest != NULL) {
		vTaskDelete(xHandleTcmdTest);
		dbg_str("<<TCDM TASK DELETED>>\r\n");
	}
	else {
		dbg_str("<<NO TCDM TASK STARTED>>\r\n");
		xHandleTcmdTest = NULL;

	}

}

static void efpga_autotest(const struct cli_cmd_entry *pEntry) {
	ram_test(NULL);
	m_mltiply_test(NULL);
	ram_32bit_16bit_8bit_test(NULL);
	tcdm_test(NULL);
}

