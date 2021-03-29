//---------------------------------//
//
// Module: apb_timer_unit
//
//---------------------------------//

typedef struct {
  union {
    __IO uint32_t CFG_REG_LO;     // Offset = 0x0000
    struct {
      __IO uint32_t  ENABLE_BIT :  1;
      __IO uint32_t  RESET_BIT  :  1;
      __IO uint32_t  IRQ_BIT    :  1;
      __IO uint32_t  IEM_BIT    :  1;
      __IO uint32_t  CMP_CLR_BIT :  1;
      __IO uint32_t  ONE_SHOT_BIT :  1;
      __IO uint32_t  PRESCALER_EN_BIT :  1;
      __IO uint32_t  REF_CLK_EN_BIT :  1;
      __IO uint32_t  PRESCALER_COUNT :  8;
      __IO uint32_t             : 14;
      __IO uint32_t  MODE_MTIME_BIT :  1;
      __IO uint32_t  MODE_64_BIT :  1;
    } CFG_REG_LO_b;
  };
  union {
    __IO uint32_t CFG_REG_HI;     // Offset = 0x0004
    struct {
      __IO uint32_t  ENABLE_BIT :  1;
      __IO uint32_t  RESET_BIT  :  1;
      __IO uint32_t  IRQ_BIT    :  1;
      __IO uint32_t  IEM_BIT    :  1;
      __IO uint32_t  CMP_CLR_BIT :  1;
      __IO uint32_t  ONE_SHOT_BIT :  1;
      __IO uint32_t  PRESCALER_EN_BIT :  1;
      __IO uint32_t  REF_CLK_EN_BIT :  1;
      __IO uint32_t  PRESCALER_COUNT :  8;
      __IO uint32_t             : 14;
      __IO uint32_t  MODE_MTIME_BIT :  1;
      __IO uint32_t  MODE_64_BIT :  1;
    } CFG_REG_HI_b;
  };
  union {
    __IO uint32_t TIMER_VAL_LO;     // Offset = 0x0008
    struct {
      __IO uint32_t  TIMER_VAL_LO : 32;
    } TIMER_VAL_LO_b;
  };
  union {
    __IO uint32_t TIMER_VAL_HI;     // Offset = 0x000c
    struct {
      __IO uint32_t  TIMER_VAL_HI : 32;
    } TIMER_VAL_HI_b;
  };
  union {
    __IO uint32_t TIMER_CMP_LO;     // Offset = 0x0010
    struct {
      __IO uint32_t  TIMER_CMP_LO : 32;
    } TIMER_CMP_LO_b;
  };
  union {
    __IO uint32_t TIMER_CMP_HI;     // Offset = 0x0014
    struct {
      __IO uint32_t  TIMER_CMP_HI : 32;
    } TIMER_CMP_HI_b;
  };
  union {
    __IO uint32_t TIMER_START_LO;     // Offset = 0x0018
    struct {
      __IO uint32_t  TIMER_START_LO :  1;
    } TIMER_START_LO_b;
  };
  union {
    __IO uint32_t TIMER_START_HI;     // Offset = 0x001c
    struct {
      __IO uint32_t  TIMER_START_HI :  1;
    } TIMER_START_HI_b;
  };
  union {
    __IO uint32_t TIMER_RESET_LO;     // Offset = 0x0020
    struct {
      __IO uint32_t  TIMER_RESET_LO :  1;
    } TIMER_RESET_LO_b;
  };
  union {
    __IO uint32_t TIMER_RESET_HI;     // Offset = 0x0024
    struct {
      __IO uint32_t  TIMER_RESET_HI :  1;
    } TIMER_RESET_HI_b;
  };
} apb_timer_unit_t;


#define REG_CFG_REG_LO                 0x000
#define   REG_CFG_REG_LO_MODE_64_BIT_LSB           31
#define   REG_CFG_REG_LO_MODE_64_BIT_MASK          0x1
#define   REG_CFG_REG_LO_MODE_MTIME_BIT_LSB        30
#define   REG_CFG_REG_LO_MODE_MTIME_BIT_MASK       0x1
#define   REG_CFG_REG_LO_PRESCALER_COUNT_LSB       8
#define   REG_CFG_REG_LO_PRESCALER_COUNT_MASK      0xff
#define   REG_CFG_REG_LO_REF_CLK_EN_BIT_LSB        7
#define   REG_CFG_REG_LO_REF_CLK_EN_BIT_MASK       0x1
#define   REG_CFG_REG_LO_PRESCALER_EN_BIT_LSB      6
#define   REG_CFG_REG_LO_PRESCALER_EN_BIT_MASK     0x1
#define   REG_CFG_REG_LO_ONE_SHOT_BIT_LSB          5
#define   REG_CFG_REG_LO_ONE_SHOT_BIT_MASK         0x1
#define   REG_CFG_REG_LO_CMP_CLR_BIT_LSB           4
#define   REG_CFG_REG_LO_CMP_CLR_BIT_MASK          0x1
#define   REG_CFG_REG_LO_IEM_BIT_LSB               3
#define   REG_CFG_REG_LO_IEM_BIT_MASK              0x1
#define   REG_CFG_REG_LO_IRQ_BIT_LSB               2
#define   REG_CFG_REG_LO_IRQ_BIT_MASK              0x1
#define   REG_CFG_REG_LO_RESET_BIT_LSB             1
#define   REG_CFG_REG_LO_RESET_BIT_MASK            0x1
#define   REG_CFG_REG_LO_ENABLE_BIT_LSB            0
#define   REG_CFG_REG_LO_ENABLE_BIT_MASK           0x1
#define REG_CFG_REG_HI                 0x004
#define   REG_CFG_REG_HI_MODE_64_BIT_LSB           31
#define   REG_CFG_REG_HI_MODE_64_BIT_MASK          0x1
#define   REG_CFG_REG_HI_MODE_MTIME_BIT_LSB        30
#define   REG_CFG_REG_HI_MODE_MTIME_BIT_MASK       0x1
#define   REG_CFG_REG_HI_PRESCALER_COUNT_LSB       8
#define   REG_CFG_REG_HI_PRESCALER_COUNT_MASK      0xff
#define   REG_CFG_REG_HI_REF_CLK_EN_BIT_LSB        7
#define   REG_CFG_REG_HI_REF_CLK_EN_BIT_MASK       0x1
#define   REG_CFG_REG_HI_PRESCALER_EN_BIT_LSB      6
#define   REG_CFG_REG_HI_PRESCALER_EN_BIT_MASK     0x1
#define   REG_CFG_REG_HI_ONE_SHOT_BIT_LSB          5
#define   REG_CFG_REG_HI_ONE_SHOT_BIT_MASK         0x1
#define   REG_CFG_REG_HI_CMP_CLR_BIT_LSB           4
#define   REG_CFG_REG_HI_CMP_CLR_BIT_MASK          0x1
#define   REG_CFG_REG_HI_IEM_BIT_LSB               3
#define   REG_CFG_REG_HI_IEM_BIT_MASK              0x1
#define   REG_CFG_REG_HI_IRQ_BIT_LSB               2
#define   REG_CFG_REG_HI_IRQ_BIT_MASK              0x1
#define   REG_CFG_REG_HI_RESET_BIT_LSB             1
#define   REG_CFG_REG_HI_RESET_BIT_MASK            0x1
#define   REG_CFG_REG_HI_ENABLE_BIT_LSB            0
#define   REG_CFG_REG_HI_ENABLE_BIT_MASK           0x1
#define REG_TIMER_VAL_LO               0x008
#define   REG_TIMER_VAL_LO_TIMER_VAL_LO_LSB        0
#define   REG_TIMER_VAL_LO_TIMER_VAL_LO_MASK       0xffffffff
#define REG_TIMER_VAL_HI               0x00C
#define   REG_TIMER_VAL_HI_TIMER_VAL_HI_LSB        0
#define   REG_TIMER_VAL_HI_TIMER_VAL_HI_MASK       0xffffffff
#define REG_TIMER_CMP_LO               0x010
#define   REG_TIMER_CMP_LO_TIMER_CMP_LO_LSB        0
#define   REG_TIMER_CMP_LO_TIMER_CMP_LO_MASK       0xffffffff
#define REG_TIMER_CMP_HI               0x014
#define   REG_TIMER_CMP_HI_TIMER_CMP_HI_LSB        0
#define   REG_TIMER_CMP_HI_TIMER_CMP_HI_MASK       0xffffffff
#define REG_TIMER_START_LO             0x018
#define   REG_TIMER_START_LO_TIMER_START_LO_LSB    0
#define   REG_TIMER_START_LO_TIMER_START_LO_MASK   0x1
#define REG_TIMER_START_HI             0x01C
#define   REG_TIMER_START_HI_TIMER_START_HI_LSB    0
#define   REG_TIMER_START_HI_TIMER_START_HI_MASK   0x1
#define REG_TIMER_RESET_LO             0x020
#define   REG_TIMER_RESET_LO_TIMER_RESET_LO_LSB    0
#define   REG_TIMER_RESET_LO_TIMER_RESET_LO_MASK   0x1
#define REG_TIMER_RESET_HI             0x024
#define   REG_TIMER_RESET_HI_TIMER_RESET_HI_LSB    0
#define   REG_TIMER_RESET_HI_TIMER_RESET_HI_MASK   0x1

static inline uint32_t regfield_read(uint32_t reg, uint32_t mask, uint32_t lsb) {
  return (reg >> lsb) & mask;
}
static inline uint32_t regfield_write(uint32_t reg, uint32_t mask, uint32_t lsb, uint32_t value) {
  reg &= ~(mask << lsb);
  reg |= (value & mask) << lsb;
  return reg;
}
