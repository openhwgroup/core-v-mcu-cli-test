//---------------------------------//
//
// Module: UDMA_I2C
//
//---------------------------------//

typedef struct {
  union {
    __IO uint32_t RX_SADDR;     // Offset = 0x0000
    struct {
      __IO uint32_t  SADDR      : 12;
    } RX_SADDR_b;
  };
  union {
    __IO uint32_t RX_SIZE;     // Offset = 0x0004
    struct {
      __IO uint32_t  SIZE       : 16;
    } RX_SIZE_b;
  };
  union {
    __IO uint32_t RX_CFG;     // Offset = 0x0008
    struct {
      __IO uint32_t  CONTINUOUS :  1;
      __IO uint32_t             :  3;
      __IO uint32_t  EN         :  1;
      __IO uint32_t  PENDING    :  1;
      __IO uint32_t  CLR        :  1;
    } RX_CFG_b;
  };
  __I uint32_t    RESERVED0[1];
  union {
    __IO uint32_t TX_SADDR;     // Offset = 0x0010
    struct {
      __IO uint32_t  SADDR      : 12;
    } TX_SADDR_b;
  };
  union {
    __IO uint32_t TX_SIZE;     // Offset = 0x0014
    struct {
      __IO uint32_t  SIZE       : 16;
    } TX_SIZE_b;
  };
  union {
    __IO uint32_t TX_CFG;     // Offset = 0x0018
    struct {
      __IO uint32_t  CONTINUOUS :  1;
      __IO uint32_t             :  3;
      __IO uint32_t  EN         :  1;
      __IO uint32_t  PENDING    :  1;
      __IO uint32_t  CLR        :  1;
    } TX_CFG_b;
  };
  __I uint32_t    RESERVED1[1];
  union {
    __IO uint32_t STATUS;     // Offset = 0x0020
    struct {
      __IO uint32_t  BUSY       :  1;
      __IO uint32_t  AL         :  1;
    } STATUS_b;
  };
  union {
    __IO uint32_t SETUP;     // Offset = 0x0024
    struct {
      __IO uint32_t  RESET      :  1;
    } SETUP_b;
  };
} UDMA_I2C_t;


#define REG_RX_SADDR                   0x00
#define   REG_RX_SADDR_SADDR_LSB                   0
#define   REG_RX_SADDR_SADDR_MASK                  0xfff
#define REG_RX_SIZE                    0x04
#define   REG_RX_SIZE_SIZE_LSB                     0
#define   REG_RX_SIZE_SIZE_MASK                    0xffff
#define REG_RX_CFG                     0x08
#define   REG_RX_CFG_CLR_LSB                       6
#define   REG_RX_CFG_CLR_MASK                      0x1
#define   REG_RX_CFG_PENDING_LSB                   5
#define   REG_RX_CFG_PENDING_MASK                  0x1
#define   REG_RX_CFG_EN_LSB                        4
#define   REG_RX_CFG_EN_MASK                       0x1
#define   REG_RX_CFG_CONTINUOUS_LSB                0
#define   REG_RX_CFG_CONTINUOUS_MASK               0x1
#define REG_TX_SADDR                   0x10
#define   REG_TX_SADDR_SADDR_LSB                   0
#define   REG_TX_SADDR_SADDR_MASK                  0xfff
#define REG_TX_SIZE                    0x14
#define   REG_TX_SIZE_SIZE_LSB                     0
#define   REG_TX_SIZE_SIZE_MASK                    0xffff
#define REG_TX_CFG                     0x18
#define   REG_TX_CFG_CLR_LSB                       6
#define   REG_TX_CFG_CLR_MASK                      0x1
#define   REG_TX_CFG_PENDING_LSB                   5
#define   REG_TX_CFG_PENDING_MASK                  0x1
#define   REG_TX_CFG_EN_LSB                        4
#define   REG_TX_CFG_EN_MASK                       0x1
#define   REG_TX_CFG_CONTINUOUS_LSB                0
#define   REG_TX_CFG_CONTINUOUS_MASK               0x1
#define REG_STATUS                     0x20
#define   REG_STATUS_AL_LSB                        1
#define   REG_STATUS_AL_MASK                       0x1
#define   REG_STATUS_BUSY_LSB                      0
#define   REG_STATUS_BUSY_MASK                     0x1
#define REG_SETUP                      0x24
#define   REG_SETUP_RESET_LSB                      0
#define   REG_SETUP_RESET_MASK                     0x1

static inline uint32_t regfield_read(uint32_t reg, uint32_t mask, uint32_t lsb) {
  return (reg >> lsb) & mask;
}
static inline uint32_t regfield_write(uint32_t reg, uint32_t mask, uint32_t lsb, uint32_t value) {
  reg &= ~(mask << lsb);
  reg |= (value & mask) << lsb;
  return reg;
}
