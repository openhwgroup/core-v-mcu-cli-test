#ifndef _ARNOLD_APB_CTL
#define _ARNOLD_APB_CTL

/* this list from arnold_soc/rtl/components/soc_peripherals.sv */

typedef struct {
  volatile unsigned int info;  // 0x0
  volatile unsigned int fcboot; // 0x4
  volatile unsigned int fcfetch; // 0x08
  volatile unsigned int reseved_0C;
  volatile unsigned int padfunc0;
  volatile unsigned int padfunc1;
  volatile unsigned int padfunc2;
  volatile unsigned int padfunc3;  // 0x1C
  volatile unsigned int padcfg0;// 0x20
  volatile unsigned int padcfg1;
  volatile unsigned int padcfg2;
  volatile unsigned int padcfg3;
  volatile unsigned int padcfg4;//0x30
  volatile unsigned int padcfg5;
  volatile unsigned int padcfg6;
  volatile unsigned int padcfg7;
  volatile unsigned int padcfg8;//0x40
  volatile unsigned int padcfg9;
  volatile unsigned int padcfg10;
  volatile unsigned int padcfg11;
  volatile unsigned int padcfg12;//0x50
  volatile unsigned int padcfg13;
  volatile unsigned int padcfg14;
  volatile unsigned int padcfg15;//0x5C
  volatile unsigned int reserved_60[4];
  volatile unsigned int cluster_ctrl; //0x70
  volatile unsigned int jtag; // 0x74
  volatile unsigned int ctrl_per;
  volatile unsigned int cluster_irq;
  volatile unsigned int cluster_boot_0;
  volatile unsigned int cluster_boot_1; //0x84
  volatile unsigned int reserved_88[6]; // 88,8c,90,94,98,9c
  volatile unsigned int corestatus;
  volatile unsigned int reserved_A4[7]; // A4, A8, AC - BC
  volatile unsigned int cs_ro; // 0xC0
  volatile unsigned int boot_sel;
  volatile unsigned int clock_sel;
  volatile unsigned int reserved_CC;
  volatile unsigned int reserved_D0[2];
  volatile unsigned int clk_div_cluster; // 0xd8
  volatile unsigned int reserved_DC;
  volatile unsigned int fpga_clk; //0xe0
  volatile unsigned int fpga_clk_gate;// 0xe4 
  volatile unsigned int fpga_reset; //0xe8
  volatile unsigned int fpga_gate; //0xec
} apb_ctl_typedef ;
#define ARNOLD_APB_CTL (0x1A104000)
#define apb ((apb_ctl_typedef *) ARNOLD_APB_CTL)

typedef struct {
  volatile unsigned int dir31_00;       // 0x00   
  volatile unsigned int enable31_00;    // 0x04   
  volatile unsigned int in31_00;        // 0x08   
  volatile unsigned int out31_00;       // 0x0c   
  volatile unsigned int set31_00;       // 0x10   
  volatile unsigned int clear31_00;     // 0x14   
  volatile unsigned int int_en31_00;    // 0x18   
  volatile unsigned int int_type15_00;  // 0x1c   
  volatile unsigned int int_type31_16;  // 0x20   
  volatile unsigned int int_status31_00;// 0x24   
  volatile unsigned int pad_cfg07_00;   // 0x28   
  volatile unsigned int pad_cfg15_08;   // 0x2c   
  volatile unsigned int pad_cfg23_16;   // 0x30   
  volatile unsigned int pad_cfg31_24;   // 0x34   
  volatile unsigned int dir63_32;       // 0x38   
  volatile unsigned int enable63_32;    // 0x3c   
  volatile unsigned int in63_32;        // 0x40   
  volatile unsigned int out63_32;       // 0x44   
  volatile unsigned int set63_32;       // 0x48   
  volatile unsigned int clear63_32;     // 0x4c   
  volatile unsigned int int_en63_32;    // 0x50   
  volatile unsigned int int_type47_32;  // 0x54   
  volatile unsigned int int_type63_48;  // 0x58   
  volatile unsigned int int_status63_32;// 0x5c   
  volatile unsigned int pad_cfg39_32;   // 0x60   
  volatile unsigned int pad_cfg47_40;   // 0x64   
  volatile unsigned int pad_cfg55_48;   // 0x68   
  volatile unsigned int pad_cfg63_56;   // 0x6C   
} gpio_typedef ;
#define ARNOLD_GPIO_BASE (0x1A101000)
#define gpio ((gpio_typedef *)ARNOLD_GPIO_BASE)
  
  
#endif /* _ARNOLD_APB_CTL */

