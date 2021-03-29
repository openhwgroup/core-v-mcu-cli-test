//---------------------------------//
//
// Module: SOC_CTRL
//
//---------------------------------//

typedef struct {
  union {
    __IO uint32_t INFO;     // Offset = 0x0000
    struct {
      __IO uint32_t  N_CLUSTERS : 16;
      __IO uint32_t  N_CORES    : 16;
    } INFO_b;
  };
  __I uint32_t    RESERVED0[28];
  union {
    __IO uint32_t JTAGREG;     // Offset = 0x0074
  };
  __I uint32_t    RESERVED1[10];
  union {
    __IO uint32_t CORESTATUS;     // Offset = 0x00a0
    struct {
      __IO uint32_t  STATUS     : 31;
      __IO uint32_t  EOC        :  1;
    } CORESTATUS_b;
  };
  __I uint32_t    RESERVED2[7];
  union {
    __IO uint32_t CS_RO;     // Offset = 0x00c0
    struct {
      __IO uint32_t  STATUS     : 31;
      __IO uint32_t  EOC        :  1;
    } CS_RO_b;
  };
  union {
    __IO uint32_t BOOTSEL;     // Offset = 0x00c4
  };
  union {
    __IO uint32_t CLKSEL;     // Offset = 0x00c8
  };
  __I uint32_t    RESERVED3[3];
  union {
    __IO uint32_t CLK_DIV_CLU;     // Offset = 0x00d8
  };
  __I uint32_t    RESERVED4[1];
  union {
    __IO uint32_t SEL_CLK_DC_FIFO_EFPGA;     // Offset = 0x00e0
  };
  union {
    __IO uint32_t CLK_GATING_DC_FIFO_EFPGA;     // Offset = 0x00e4
  };
  union {
    __IO uint32_t RESET_TYPE1_EFPGA;     // Offset = 0x00e8
  };
  union {
    __IO uint32_t ENABLE_IN_OUT_EFPGA;     // Offset = 0x00ec
  };
  __I uint32_t    RESERVED5[196];
  union {
    __IO uint32_t IO_CTRL;     // Offset = 0x0400
    struct {
      __IO uint32_t  MUX        :  2;
      __IO uint32_t             :  6;
      __IO uint32_t  CFG        :  6;
    } IO_CTRL_b;
  };
} SOC_CTRL_t;


#define REG_INFO                       0x0000
#define   REG_INFO_N_CORES_LSB                     16
#define   REG_INFO_N_CORES_MASK                    0xffff
#define   REG_INFO_N_CLUSTERS_LSB                  0
#define   REG_INFO_N_CLUSTERS_MASK                 0xffff
#define REG_JTAGREG                    0x0074
#define REG_CORESTATUS                 0x00A0
#define   REG_CORESTATUS_EOC_LSB                   31
#define   REG_CORESTATUS_EOC_MASK                  0x1
#define   REG_CORESTATUS_STATUS_LSB                0
#define   REG_CORESTATUS_STATUS_MASK               0x7fffffff
#define REG_CS_RO                      0x00C0
#define   REG_CS_RO_EOC_LSB                        31
#define   REG_CS_RO_EOC_MASK                       0x1
#define   REG_CS_RO_STATUS_LSB                     0
#define   REG_CS_RO_STATUS_MASK                    0x7fffffff
#define REG_BOOTSEL                    0x00C4
#define REG_CLKSEL                     0x00C8
#define REG_CLK_DIV_CLU                0x00D8
#define REG_SEL_CLK_DC_FIFO_EFPGA      0x00E0
#define REG_CLK_GATING_DC_FIFO_EFPGA   0x00E4
#define REG_RESET_TYPE1_EFPGA          0x00E8
#define REG_ENABLE_IN_OUT_EFPGA        0x00EC
#define REG_IO_CTRL                    0x0400
#define   REG_IO_CTRL_CFG_LSB                      8
#define   REG_IO_CTRL_CFG_MASK                     0x3f
#define   REG_IO_CTRL_MUX_LSB                      0
#define   REG_IO_CTRL_MUX_MASK                     0x3

static inline uint32_t regfield_read(uint32_t reg, uint32_t mask, uint32_t lsb) {
  return (reg >> lsb) & mask;
}
static inline uint32_t regfield_write(uint32_t reg, uint32_t mask, uint32_t lsb, uint32_t value) {
  reg &= ~(mask << lsb);
  reg |= (value & mask) << lsb;
  return reg;
}
