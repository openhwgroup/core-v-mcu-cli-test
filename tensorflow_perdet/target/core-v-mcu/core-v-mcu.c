/*
 * Copyright 2020 ETH Zurich
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * SPDX-License-Identifier: Apache-2.0
 * Author: Robert Balas (balasr@iis.ee.ethz.ch)
 */

#include <stdint.h>
#include <assert.h>


#include <target/core-v-mcu/include/core_pulp_cluster.h>
#include <target/core-v-mcu/include/core-v-mcu-config.h>

//#include "pmsis/implem/drivers/fc_event/fc_event.h"
#include "hal/include/hal_fc_event.h"
/* TODO: weird include */
#include "target/core-v-mcu/include/core-v-mcu-properties.h"
#include "target/core-v-mcu/include/core-v-mcu-system.h"
#include "hal/include/hal_irq.h"
#include "hal/include/hal_soc_eu.h"
#include "hal/include/hal_apb_soc_ctrl_reg_defs.h"

#include "drivers/include/udma_uart_driver.h"
#include "drivers/include/udma_i2cm_driver.h"

#include "hal/include/hal_udma_cam_reg_defs.h"
#include "drivers/include/camera.h"




/* test some assumptions we make about compiler settings */
static_assert(sizeof(uintptr_t) == 4,
	      "uintptr_t is not 4 bytes. Make sure you are using -mabi=ilp32*");

/* Allocate heap to special section. Note that we have no references in the
 * whole program to this variable (since its just here to allocate space in the
 * section for our heap), so when using LTO it will be removed. We force it to
 * stay with the "used" attribute
 */
__attribute__((section(".heap"), used)) uint8_t ucHeap[HEAP_SIZE];

/* Inform linker script about .heap section size. Note: GNU ld seems to
 * internally represent integers with the bfd_vma type, that is a type that can
 * contain memory addresses (typdefd to some int type depending on the
 * architecture). uint32_t seems to me the most fitting candidate for rv32.
 */
uint32_t __heap_size = HEAP_SIZE;

volatile uint32_t system_core_clock = DEFAULT_SYSTEM_CLOCK;

/**
 * Board init code. Always call this before anything else.
 */

uint8_t setFLLFrequencyInIntegerMode(uint8_t aFLLNum, uint8_t aRefFreqInMHz, uint16_t aMultiplier, uint8_t aDivideRatio_R_Prescale, uint8_t aPS0_L1, uint8_t aPS0_L2  )
{
    uint8_t lSts = 0;
	volatile uint32_t *lPLLStartAddress = (uint32_t *)NULL;
    uint32_t lCounter = 0;
    uint32_t lCfgVal = 0;

    uint8_t lPS0_L1 = aPS0_L1 & 0x03;
    uint8_t lPS0_L2 = aPS0_L2 & 0xFF;

    if( aFLLNum == 0 )
        lPLLStartAddress = (uint32_t *)FLL1_START_ADDR;
    else if( aFLLNum == 1 )
        lPLLStartAddress = (uint32_t *)FLL2_START_ADDR;
    else if( aFLLNum == 2 )
        lPLLStartAddress = (uint32_t *)FLL3_START_ADDR;
    else
        lPLLStartAddress = (uint32_t *)NULL;

    if( lPLLStartAddress != NULL )
    {
	    if( ( aRefFreqInMHz >= 5 ) && ( aRefFreqInMHz <= 500 ) )
	    {
	        if( ( aMultiplier > 0 ) && ( aMultiplier < 2048 ) )
	        {
	            if( aDivideRatio_R_Prescale < 16 )
	            {
                    *lPLLStartAddress |= (1 << 19);//Bypass on;
                    *lPLLStartAddress |= (1 << 2);   //Reset high
                    *lPLLStartAddress &= ~(1 << 2) ;//Reset low;
                    *lPLLStartAddress &= ~(1 << 18); //PS0_EN is set to low
                    *lPLLStartAddress |= (lPS0_L1 << 0);   //PS0_L1 0 which gives L01 = 1
                    *lPLLStartAddress |= (lPS0_L2 << 4);   //PS0_L2_INT 0 and PS0_L2_FRAC 0 which gives L02 = 1
                    *lPLLStartAddress |= (0 << 12);   //PS0_L2_INT 0 and PS0_L2_FRAC 0 which gives L02 = 1


                    //FLL1 Config 1 register not configuring PS1
                    *(lPLLStartAddress + 1) = 0;

                    //FLL1 Config 2 register
                    lCfgVal = 0;
                    lCfgVal |= (aMultiplier << 4 ); //MULT_INT	0x28 = 40 (40*10 = 400MHz) Multiplier cannot hold 0
                    lCfgVal |= (1 << 27 ); //INTEGER_MODE is enabled
                    lCfgVal |= (aDivideRatio_R_Prescale << 28 ); //PRESCALE value (Divide Ratio R = 1)

                    *(lPLLStartAddress + 2) = lCfgVal;

                    //FLL1 Config 3 register not configuring SSC
                    *(lPLLStartAddress + 3) = 0;

                    //FLL1 Config 4 register
                    *(lPLLStartAddress + 4) = 0x64;

                    //FLL1 Config 5 register
                    *(lPLLStartAddress + 5) = 0x269;

                    *lPLLStartAddress |= (1<<2);   //Reset high
                    *lPLLStartAddress |= (1<<18); //PS0_EN;
                    //lCounter = 0;
                    while ( (*(lPLLStartAddress+4) & 0x80000000) == 0 )  //Wait for lock detect to go high
                    {
                        lCounter++;
                        if( lCounter >= 0x00010000)
                        {
                            lSts = 5;     //Unable to achieve lock
                            lCounter = 0;
                            break;
                        }
                    }
                    if( lSts == 0 )
                        *(lPLLStartAddress) &= ~(1<<19) ;//Bypass off;
                }
                else
                {
                    lSts = 1;   //aDivideRatio_R_Prescale
                }
            }
            else
            {
                lSts = 2;   //Invalid aMultiplier
            }
        }
        else
        {
            lSts = 3;   //Invalid reference freq
        }
    }
    else
    {
        lSts = 4;   //Invalid PLL number
    }
    return lSts;
}

void udma_cam_open (uint8_t cam_id)
{
	int i = 0;
	volatile UdmaCtrl_t*pudma_ctrl = (UdmaCtrl_t*)UDMA_CH_ADDR_CTRL;

	/* Enable reset and enable uart clock */
	pudma_ctrl->reg_rst |= (UDMA_CTRL_CAM0_CLKEN << cam_id);
	pudma_ctrl->reg_rst &= ~(UDMA_CTRL_CAM0_CLKEN << cam_id);
	pudma_ctrl->reg_cg |= (UDMA_CTRL_CAM0_CLKEN << cam_id);

	//psdio_regs->clk_div_b.clk_div = 5;
	//psdio_regs->clk_div_b.valid = 1;
	hal_setpinmux(21, 0); 	//cam0_vsync
	hal_setpinmux(22, 0);	//cam0_hsync
	hal_setpinmux(25, 0);	//cam0_clk
	for(i=0; i<8; i++ )
	{
		//set pin muxes to cam functionality
		 hal_setpinmux(29+i, 0);
	}
	return;
}

void cam_interface_init (uint16_t x, uint16_t y, uint8_t aBitMode)
{
	camera_struct_t *camera;

	uint16_t lXCoordOfUpperRightCorner = 0; // X coordinate of upper right corner of slice
	uint16_t lYCoordOfUpperRightCorner = 0; // Y coordinate of upper right corner of slice

	lXCoordOfUpperRightCorner = x + 3;
	lYCoordOfUpperRightCorner = y + 3;
	//camera = (camera_struct_t *)0x1A102300;  // Peripheral 5?
	camera = (camera_struct_t *)(UDMA_CH_ADDR_CAM + 0 * UDMA_CH_SIZE);
	camera->cfg_ll = 0<<16 | 0;
	camera->cfg_ur = ( ( lYCoordOfUpperRightCorner << 16 ) | lXCoordOfUpperRightCorner );
	//camera->cfg_ur = 323<<16 | 243; // 320 x 240 ?
	camera->cfg_filter = (1 << 16) | (1 << 8) | 1;
	//camera->cfg_size = 324;
	camera->cfg_size = y + 4;
	camera->vsync_pol = 1;
	if( aBitMode == 1 )
	{
		camera->cfg_glob |= (1 << 17);
	}
	else if( aBitMode == 4 )
	{
		camera->cfg_glob |= (1 << 16);
	}
	else if( aBitMode == 8 )
	{
		camera->cfg_glob |= (0 << 0);
	}
	camera->cfg_glob |= (0 << 0) | //  framedrop disabled
			(000000 << 1) | // number of frames to drop
			(0 << 7) | // Frame slice disabled
			(004 << 8) | // Format binary 100 = ByPass little endian
			(0000 << 11);  // Shift value ignored in bypass
}

uint8_t cam_grab_frame (int x, int y, uint8_t* pparam)
{
	uint32_t lCounter = 0;
	uint8_t lStatus = 0;
	uint16_t lX = x+4;
	uint16_t lY = y+4;

	camera_struct_t *camera;
	//camera = (camera_struct_t *)0x1A102300;  // Peripheral 5?
	camera = (camera_struct_t *)(UDMA_CH_ADDR_CAM + 0 * UDMA_CH_SIZE);

	camera->rx_saddr = pparam;
	camera->rx_size = (lX * lY);
	camera->rx_cfg = 0x12;  // start 16-bit transfers
	camera->cfg_glob |= camera->cfg_glob | (1 << 31) ; // enable 1 == go

	lCounter = 0;
	while (camera->rx_size != 0) {
		lCounter++;
		if( lCounter >= 0x00100000 )
		{
			lStatus = 3;	//Time out
			break;
		}
	}
	//configASSERT( xSemaphoreTake( shSemaphoreHandle, 1000000 ) == pdTRUE );
	camera->cfg_glob = camera->cfg_glob & (0x7fffffff) ; // enable 1 == go
	//configASSERT( xSemaphoreGive( shSemaphoreHandle ) == pdTRUE );
	return lStatus;

}

void initPictureBuf(int x, int y, uint8_t* pictureBuf)
{
	volatile int i = 0, j = 0, k = 0;

	for (i = 0; i < (x+4); i++)
	{
		for (j = 0; j< (y+4); j++)
		{
			pictureBuf[i*324+j] = 0xAA;
		}
	}
}

void displayFrame(int x, int y, uint8_t* pictureBuf)
{
	volatile int i = 0, j = 0, k = 0;
	uint8_t lChar = 0;

	for (i = 0; i<x; i++)
	{
		for (j = 0; j < y; j += 16)
		{
			CLI_printf(1,"ImAgE %d %d",i,j);
			k = 0;
			while( k <16 ) {
				lChar = ( (pictureBuf[i*324+j+(k)] & 0x0f) << 4 );
				lChar |= ( (pictureBuf[i*324+j+(k)] & 0xf0) >> 4 );
				//CLI_printf(1," %02x",pictureBuf[i*324+j+(k++)] & 0xff);
				CLI_printf(1," %02x",lChar);
				k++;
			}
			CLI_printf(1,"\n");
		}
	}
}

void system_init(void)
{
	SocCtrl_t *soc=APB_SOC_CTRL_ADDR;
	soc->soft_reset = 1;

    setFLLFrequencyInIntegerMode(0, 10, 40, 1, 0, 0);   // 400

    setFLLFrequencyInIntegerMode(1, 10, 20, 1, 0, 0);   // 200

    setFLLFrequencyInIntegerMode(2, 10, 10, 1, 0, 0);   // 100

	/* TODO: enable uart */
	for (uint8_t id = 0; id != N_UART; id++) {
		udma_uart_open(id, 115200);
	}
	for (uint8_t id = 0; id != N_I2CM; id++) {
		udma_i2cm_open(id, 400000);  //200000
	}
	udma_cam_open(0);
}


