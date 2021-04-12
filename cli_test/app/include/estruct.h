/*
 * estruct.c
 *
 *  Created on: Jan 19, 2021
 *      Author: gregmartin
 */

typedef struct {
	volatile unsigned int CFG_REG_LO; //         6'h0
	volatile unsigned int CFG_REG_HI; //         6'h4
	volatile unsigned int TIMER_VAL_LO; //       6'h8
	volatile unsigned int TIMER_VAL_HI; //       6'hC
	volatile unsigned int TIMER_CMP_LO; //       6'h10
	volatile unsigned int TIMER_CMP_HI; //       6'h14
	volatile unsigned int TIMER_START_LO; //     6'h18
	volatile unsigned int TIMER_START_HI; //     6'h1C
	volatile unsigned int TIMER_RESET_LO; //     6'h20
	volatile unsigned int TIMER_RESET_HI; //     6'h24
} apb_timer_typedef;

typedef struct {
	volatile unsigned int MASK;//		0x0
	volatile unsigned int MASK_SET;//			0x4
	volatile unsigned int MASK_CLEAR;//			0x8
	volatile unsigned int INT;//			0xC
	volatile unsigned int INT_SET;//			0x10
	volatile unsigned int INT_CLEAR;//			0x14
	volatile unsigned int ACK;//			0x18
	volatile unsigned int ACK_SET;//			0x1C
	volatile unsigned int ACK_CLEAR;//			0x20
	volatile unsigned int FIFO;//			0x24
} apb_interrupt_ctl_typedef;
typedef struct {
	volatile unsigned int tcdm0_ctl;
	volatile unsigned int tcdm1_ctl;
	volatile unsigned int tcdm2_ctl;
	volatile unsigned int tcdm3_ctl;
	volatile unsigned int m0_m0_ctl;
	volatile unsigned int m0_m1_ctl;
	volatile unsigned int m1_m0_ctl;
	volatile unsigned int m1_m1_ctl;
	volatile unsigned int m0_ram_ctl;
	volatile unsigned int m1_ram_ctl;
	volatile unsigned int reserved28;
	volatile unsigned int reserved2c;
	volatile unsigned int m0_m0_clken;
	volatile unsigned int m0_m1_clken;
	volatile unsigned int m1_m0_clken;
	volatile unsigned int m1_m1_clken;
	volatile unsigned int efpga_out0;
	volatile unsigned int efpga_out32;
	volatile unsigned int efpga_out64;
	volatile unsigned int reserved4c;
	volatile unsigned int efpga_oe0;
	volatile unsigned int efpga_oe32;
	volatile unsigned int efpga_oe64;
	volatile unsigned int reserved5c;
	volatile unsigned int efpga_in0;
	volatile unsigned int efpga_in32;
	volatile unsigned int efpga_in64;
	volatile unsigned int events;
	volatile unsigned int reserved70[0x1e4]; // 0x70 - 0x7FF
	volatile unsigned int test_read;
	volatile unsigned int reserved804[0x1ff];
	volatile unsigned int m0_oper0[0x400];
	volatile unsigned int m0_oper1[0x400];
	volatile unsigned int m0_coef[0x400];
	volatile unsigned int m1_oper0[0x400];
	volatile unsigned int m1_oper1[0x400];
	volatile unsigned int m1_coef[0x400];
} efpga_typedef;

typedef struct {
	volatile unsigned int reserved0[0x18]; //
	volatile unsigned int WCFGFUN;
	volatile unsigned int RCFGFUN;
	volatile unsigned int reserved68[0x20]; // 0x68-0xE7
	volatile unsigned int rst_efpga;
	volatile unsigned int ena_efpga;
	volatile unsigned int control_in;
	volatile unsigned int status_out;
	volatile unsigned int version;
	volatile unsigned int reservedf0[0xb8];
	volatile unsigned int padmux[64];
} apb_soc_ctrl_typedef;
