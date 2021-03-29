//---------------------------------//
//
// Module: UDMA_UART
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
      __IO uint32_t  TX_BUSY    :  1;
      __IO uint32_t  RX_BUSY    :  1;
    } STATUS_b;
  };
  union {
    __IO uint32_t UART_SETUP;     // Offset = 0x0024
    struct {
      __IO uint32_t  PARITY_EN  :  1;
      __IO uint32_t  BITS       :  2;
      __IO uint32_t  STOP_BITS  :  1;
      __IO uint32_t  RX_POLLING_EN :  1;
      __IO uint32_t  RX_CLEAN_FIFO :  1;
      __IO uint32_t             :  2;
      __IO uint32_t  EN_TX      :  1;
      __IO uint32_t  EN_RX      :  1;
      __IO uint32_t             :  6;
      __IO uint32_t  DIV        : 16;
    } UART_SETUP_b;
  };
  union {
    __IO uint32_t ERROR;     // Offset = 0x0028
    struct {
      __IO uint32_t  OVERFLOW_ERR :  1;
      __IO uint32_t  PARITY_ERR :  1;
    } ERROR_b;
  };
  union {
    __IO uint32_t IRQ_EN;     // Offset = 0x002c
    struct {
      __IO uint32_t  RX_IRQ_EN  :  1;
      __IO uint32_t  ERR_IRQ_EN :  1;
    } IRQ_EN_b;
  };
  union {
    __IO uint32_t VALID;     // Offset = 0x0030
    struct {
      __IO uint32_t  RX_DATA_VALID :  1;
    } VALID_b;
  };
  union {
    __IO uint32_t DATA;     // Offset = 0x0034
    struct {
      __IO uint32_t  RX_DATA    :  8;
    } DATA_b;
  };
} UDMA_UART_t;


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
#define   REG_STATUS_RX_BUSY_LSB                   1
#define   REG_STATUS_RX_BUSY_MASK                  0x1
#define   REG_STATUS_TX_BUSY_LSB                   0
#define   REG_STATUS_TX_BUSY_MASK                  0x1
#define REG_UART_SETUP                 0x24
#define   REG_UART_SETUP_DIV_LSB                   16
#define   REG_UART_SETUP_DIV_MASK                  0xffff
#define   REG_UART_SETUP_EN_RX_LSB                 9
#define   REG_UART_SETUP_EN_RX_MASK                0x1
#define   REG_UART_SETUP_EN_TX_LSB                 8
#define   REG_UART_SETUP_EN_TX_MASK                0x1
#define   REG_UART_SETUP_RX_CLEAN_FIFO_LSB         5
#define   REG_UART_SETUP_RX_CLEAN_FIFO_MASK        0x1
#define   REG_UART_SETUP_RX_POLLING_EN_LSB         4
#define   REG_UART_SETUP_RX_POLLING_EN_MASK        0x1
#define   REG_UART_SETUP_STOP_BITS_LSB             3
#define   REG_UART_SETUP_STOP_BITS_MASK            0x1
#define   REG_UART_SETUP_BITS_LSB                  1
#define   REG_UART_SETUP_BITS_MASK                 0x3
#define   REG_UART_SETUP_PARITY_EN_LSB             0
#define   REG_UART_SETUP_PARITY_EN_MASK            0x1
#define REG_ERROR                      0x28
#define   REG_ERROR_PARITY_ERR_LSB                 1
#define   REG_ERROR_PARITY_ERR_MASK                0x1
#define   REG_ERROR_OVERFLOW_ERR_LSB               0
#define   REG_ERROR_OVERFLOW_ERR_MASK              0x1
#define REG_IRQ_EN                     0x2C
#define   REG_IRQ_EN_ERR_IRQ_EN_LSB                1
#define   REG_IRQ_EN_ERR_IRQ_EN_MASK               0x1
#define   REG_IRQ_EN_RX_IRQ_EN_LSB                 0
#define   REG_IRQ_EN_RX_IRQ_EN_MASK                0x1
#define REG_VALID                      0x30
#define   REG_VALID_RX_DATA_VALID_LSB              0
#define   REG_VALID_RX_DATA_VALID_MASK             0x1
#define REG_DATA                       0x34
#define   REG_DATA_RX_DATA_LSB                     0
#define   REG_DATA_RX_DATA_MASK                    0xff

static inline uint32_t regfield_read(uint32_t reg, uint32_t mask, uint32_t lsb) {
  return (reg >> lsb) & mask;
}
static inline uint32_t regfield_write(uint32_t reg, uint32_t mask, uint32_t lsb, uint32_t value) {
  reg &= ~(mask << lsb);
  reg |= (value & mask) << lsb;
  return reg;
}
