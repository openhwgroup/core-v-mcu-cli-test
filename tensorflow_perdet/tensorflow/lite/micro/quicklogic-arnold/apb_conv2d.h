#ifndef _ARNOLD_APB_CONV2D
#define _ARNOLD_APB_CONV2D

typedef struct {
  volatile unsigned int control;  // 0x00
  volatile unsigned int width;    // 0x04
  volatile unsigned int height;    // 0x08
  volatile unsigned int channels;    // 0x0C
  volatile unsigned int filters;    // 0x10
  volatile unsigned int *filter_base;    // 0x14
  volatile unsigned int *pixel_base;    // 0x18
  volatile unsigned int *bias_base;    // 0x1C
  volatile unsigned int *result_base;    // 0x20
  volatile unsigned int total_pixels;    // 0x24
  volatile unsigned int intr_en; // 28
  volatile unsigned int intr_st; // 2c
  volatile unsigned int int_addr;    // 0x30
  volatile unsigned int pixel_data;    // 0x34
  volatile unsigned int filter_data;    // 0x38
  volatile unsigned int bias_data;    // 0x3C
  volatile unsigned int debug_mux;    // 0x40
  volatile unsigned int clocks;    // 0x44
  volatile unsigned int quant;
  volatile unsigned int i2c;
} fpga_typedef ;

#define FPGA_APB_BASE (0x1A200000)
#define efpga ((fpga_typedef *) FPGA_APB_BASE)



#endif /*  _ARNOLD_APB_CONV2D */
