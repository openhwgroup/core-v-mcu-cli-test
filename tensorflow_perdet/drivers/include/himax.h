/*
 * Copyright (C) 2018 ETH Zurich and University of Bologna
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
 */

/*
 * Copyright (C) 2018 GreenWaves Technologies
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
 */

#ifndef __HIMAX_H__
#define __HIMAX_H__

/*
 *  HIMAX camera macros
 */
// Register address
// Read only registers
#define         MODEL_ID_H          0x0000
#define         MODEL_ID_L          0x0001
#define         FRAME_COUNT         0x0005
#define         PIXEL_ORDER         0x0006
// R&W registers
// Sensor mode control
#define         MODE_SELECT         0x0100
#define         IMG_ORIENTATION     0x0101
#define         SW_RESET            0x0103
#define         GRP_PARAM_HOLD      0x0104
// Sensor exposure gain control
#define         INTEGRATION_H       0x0202
#define         INTEGRATION_L       0x0203
#define         ANALOG_GAIN         0x0205
#define         DIGITAL_GAIN_H      0x020E
#define         DIGITAL_GAIN_L      0x020F
// Frame timing control
#define         FRAME_LEN_LINES_H   0x0340
#define         FRAME_LEN_LINES_L   0x0341
#define         LINE_LEN_PCK_H      0x0342
#define         LINE_LEN_PCK_L      0x0343
// Binning mode control
#define         READOUT_X           0x0383
#define         READOUT_Y           0x0387
#define         BINNING_MODE        0x0390
// Test pattern control
#define         TEST_PATTERN_MODE   0x0601
// Black level control
#define         BLC_CFG             0x1000
#define         BLC_TGT             0x1003
#define         BLI_EN              0x1006
#define         BLC2_TGT            0x1007
//  Sensor reserved
#define         DPC_CTRL            0x1008
#define         SINGLE_THR_HOT      0x100B
#define         SINGLE_THR_COLD     0x100C
// VSYNC,HSYNC and pixel shift register
#define         VSYNC_HSYNC_PIXEL_SHIFT_EN  0x1012
// Automatic exposure gain control
#define         AE_CTRL             0x2100
#define         AE_TARGET_MEAN      0x2101
#define         AE_MIN_MEAN         0x2102
#define         CONVERGE_IN_TH      0x2103
#define         CONVERGE_OUT_TH     0x2104
#define         MAX_INTG_H          0x2105
#define         MAX_INTG_L          0x2106
#define         MIN_INTG            0x2107
#define         MAX_AGAIN_FULL      0x2108
#define         MAX_AGAIN_BIN2      0x2109
#define         MIN_AGAIN           0x210A
#define         MAX_DGAIN           0x210B
#define         MIN_DGAIN           0x210C
#define         DAMPING_FACTOR      0x210D
#define         FS_CTRL             0x210E
#define         FS_60HZ_H           0x210F
#define         FS_60HZ_L           0x2110
#define         FS_50HZ_H           0x2111
#define         FS_50HZ_L           0x2112
#define         FS_HYST_TH          0x2113
// Motion detection control
#define         MD_CTRL             0x2150
#define         I2C_CLEAR           0x2153
#define         WMEAN_DIFF_TH_H     0x2155
#define         WMEAN_DIFF_TH_M     0x2156
#define         WMEAN_DIFF_TH_L     0x2157
#define         MD_THH              0x2158
#define         MD_THM1             0x2159
#define         MD_THM2             0x215A
#define         MD_THL              0x215B
//  Sensor timing control
#define         QVGA_WIN_EN         0x3010
#define         SIX_BIT_MODE_EN     0x3011
#define         PMU_AUTOSLEEP_FRAMECNT  0x3020
#define         ADVANCE_VSYNC       0x3022
#define         ADVANCE_HSYNC       0x3023
#define         EARLY_GAIN          0x3035
//  IO and clock control
#define         BIT_CONTROL         0x3059
#define         OSC_CLK_DIV         0x3060
#define         ANA_Register_11     0x3061
#define         IO_DRIVE_STR        0x3062
#define         IO_DRIVE_STR2       0x3063
#define         ANA_Register_14     0x3064
#define         OUTPUT_PIN_STATUS_CONTROL   0x3065
#define         ANA_Register_17     0x3067
#define         PCLK_POLARITY       0x3068

/*
 * Useful value of Himax registers
 */
#define         HIMAX_RESET         0x01
#define         Pclk_rising_edge    0x00
#define         Pclk_falling_edge   0x01

#define         BYPASS_BIGEND       5

enum{
    HIMAX_Standby = 0x0,
    HIMAX_Streaming = 0x1,        // I2C triggered streaming enable
    HIMAX_Streaming2 = 0x3,       // Output N frames
    HIMAX_Streaming3 = 0x5        // Hardware Trigger
};


 typedef struct {
 	uint16_t addr;
 	uint8_t data;
 }reg_cfg_t;


 reg_cfg_t himaxRegInit[] = {
     {BLC_TGT, 0x08},            //  BLC target :8  at 8 bit mode
     {BLC2_TGT, 0x08},           //  BLI target :8  at 8 bit mode
     {0x3044, 0x0A},             //  Increase CDS time for settling
     {0x3045, 0x00},             //  Make symetric for cds_tg and rst_tg
     {0x3047, 0x0A},             //  Increase CDS time for settling
     {0x3050, 0xC0},             //  Make negative offset up to 4x
     {0x3051, 0x42},
     {0x3052, 0x50},
     {0x3053, 0x00},
     {0x3054, 0x03},             //  tuning sf sig clamping as lowest
     {0x3055, 0xF7},             //  tuning dsun
     {0x3056, 0xF8},             //  increase adc nonoverlap clk
     {0x3057, 0x29},             //  increase adc pwr for missing code
     {0x3058, 0x1F},             //  turn on dsun
     {0x3059, 0x1E},
     {0x3064, 0x00},
     {0x3065, 0x04},             //  pad pull 0

     {BLC_CFG, 0x43},            //  BLC_on, IIR

     {0x1001, 0x43},             //  BLC dithering en
     {0x1002, 0x43},             //  blc_darkpixel_thd
     {0x0350, 0x00},             //  Dgain Control
     {BLI_EN, 0x01},             //  BLI enable
     {0x1003, 0x00},             //  BLI Target [Def: 0x20]

     {DPC_CTRL, 0x01},           //  DPC option 0: DPC off   1 : mono   3 : bayer1   5 : bayer2
     {0x1009, 0xA0},             //  cluster hot pixel th
     {0x100A, 0x60},             //  cluster cold pixel th
     {SINGLE_THR_HOT, 0x90},     //  single hot pixel th
     {SINGLE_THR_COLD, 0x40},    //  single cold pixel th
     {0x1012, 0x00},             //  Sync. shift disable
     {0x2000, 0x07},
     {0x2003, 0x00},
     {0x2004, 0x1C},
     {0x2007, 0x00},
     {0x2008, 0x58},
     {0x200B, 0x00},
     {0x200C, 0x7A},
     {0x200F, 0x00},
     {0x2010, 0xB8},
     {0x2013, 0x00},
     {0x2014, 0x58},
     {0x2017, 0x00},
     {0x2018, 0x9B},

     {AE_CTRL,        0x01},      //Automatic Exposure Gain Control
     {AE_TARGET_MEAN, 0x3C},      //AE target mean [Def: 0x3C]
     {AE_MIN_MEAN,    0x0A},      //AE min target mean [Def: 0x0A]

     {INTEGRATION_H,  0x00},      //Integration H [Def: 0x01]
     {INTEGRATION_L,  0x60},      //Integration L [Def: 0x08]
     {ANALOG_GAIN,    0x00},      //Analog Global Gain
     {DAMPING_FACTOR, 0x20},      //Damping Factor [Def: 0x20]
     {DIGITAL_GAIN_H, 0x01},      //Digital Gain High [Def: 0x01]
     {DIGITAL_GAIN_L, 0x00},      //Digital Gain Low [Def: 0x00]

     {0x2103, 0x03},

     {0x2104, 0x05},
     {0x2105, 0x01},

     {0x2106, 0x54},

     {0x2108, 0x03},
     {0x2109, 0x04},

     {0x210B, 0xC0},
     {0x210E, 0x00}, //Flicker Control
     {0x210F, 0x00},
     {0x2110, 0x3C},
     {0x2111, 0x00},
     {0x2112, 0x32},

     {0x2150, 0x30},
     {0x0340, 0x02},
     {0x0341, 0x16},
     {0x0342, 0x01},
     {0x0343, 0x78},
     {0x3010, 0x01},
     {0x0383, 0x01},
     {0x0387, 0x01},
     {0x0390, 0x00},
     {0x3011, 0x70},
     //{0x3059, 0x02},
	 {0x3059, 0x40},
     {0x3060, 0x01},
 //    {0x3060, 0x25}, //Clock gating and clock divisors
     {0x3068, 0x20}, //PCLK0 polarity
     {IMG_ORIENTATION, 0x01}, // change the orientation
     {0x0104, 0x01},
     {0x0100, 0x01},
 	//{0x0601, 0x11}	//Test pattern walking ones
 	//{0x0601, 0x01}	//Test pattern colour bar
 };
#endif
