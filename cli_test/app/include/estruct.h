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

