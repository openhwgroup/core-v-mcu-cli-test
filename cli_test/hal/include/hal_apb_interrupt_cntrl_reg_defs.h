//---------------------------------//
//
// Module: APB_INTERRUPT_CNTRL
//
//---------------------------------//

typedef struct {
  union {
    __IO uint32_t REG_MASK;     // Offset = 0x0000
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_MASK_b;
  };
  union {
    __IO uint32_t REG_MASK_SET;     // Offset = 0x0004
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_MASK_SET_b;
  };
  union {
    __IO uint32_t REG_MASK_CLEAR;     // Offset = 0x0008
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_MASK_CLEAR_b;
  };
  union {
    __IO uint32_t REG_INT;     // Offset = 0x000c
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_INT_b;
  };
  union {
    __IO uint32_t REG_INT_SET;     // Offset = 0x0010
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_INT_SET_b;
  };
  union {
    __IO uint32_t REG_INT_CLEAR;     // Offset = 0x0014
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_INT_CLEAR_b;
  };
  union {
    __IO uint32_t REG_ACK;     // Offset = 0x0018
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_ACK_b;
  };
  union {
    __IO uint32_t REG_ACK_SET;     // Offset = 0x001c
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_ACK_SET_b;
  };
  union {
    __IO uint32_t REG_ACK_CLEAR;     // Offset = 0x0020
    struct {
      __IO uint32_t  reserved   :  7;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  unused2    :  8;
      __IO uint32_t  timer_lo_event :  1;
      __IO uint32_t  timer_hi_event :  1;
      __IO uint32_t  gpio_event :  1;
      __IO uint32_t  adv_timer_evens :  4;
      __IO uint32_t  ref_change_event :  1;
      __IO uint32_t  unused1    :  5;
      __IO uint32_t  fc_err_events :  1;
      __IO uint32_t  fc_hp_events :  2;
    } REG_ACK_CLEAR_b;
  };
  union {
    __IO uint32_t REG_FIFO;     // Offset = 0x0024
    struct {
      __IO uint32_t  EVENT_ID   :  8;
    } REG_FIFO_b;
  };
} APB_INTERRUPT_CNTRL_t;


#define REG_REG_MASK                   0x000
#define   REG_REG_MASK_fc_hp_events_LSB            30
#define   REG_REG_MASK_fc_hp_events_MASK           0x3
#define   REG_REG_MASK_fc_err_events_LSB           29
#define   REG_REG_MASK_fc_err_events_MASK          0x1
#define   REG_REG_MASK_unused1_LSB                 24
#define   REG_REG_MASK_unused1_MASK                0x1f
#define   REG_REG_MASK_ref_change_event_LSB        23
#define   REG_REG_MASK_ref_change_event_MASK       0x1
#define   REG_REG_MASK_adv_timer_evens_LSB         19
#define   REG_REG_MASK_adv_timer_evens_MASK        0xf
#define   REG_REG_MASK_gpio_event_LSB              18
#define   REG_REG_MASK_gpio_event_MASK             0x1
#define   REG_REG_MASK_timer_hi_event_LSB          17
#define   REG_REG_MASK_timer_hi_event_MASK         0x1
#define   REG_REG_MASK_timer_lo_event_LSB          16
#define   REG_REG_MASK_timer_lo_event_MASK         0x1
#define   REG_REG_MASK_unused2_LSB                 8
#define   REG_REG_MASK_unused2_MASK                0xff
#define   REG_REG_MASK_timer_lo_event_LSB          7
#define   REG_REG_MASK_timer_lo_event_MASK         0x1
#define   REG_REG_MASK_reserved_LSB                0
#define   REG_REG_MASK_reserved_MASK               0x7f
#define REG_REG_MASK_SET               0x004
#define   REG_REG_MASK_SET_fc_hp_events_LSB        30
#define   REG_REG_MASK_SET_fc_hp_events_MASK       0x3
#define   REG_REG_MASK_SET_fc_err_events_LSB       29
#define   REG_REG_MASK_SET_fc_err_events_MASK      0x1
#define   REG_REG_MASK_SET_unused1_LSB             24
#define   REG_REG_MASK_SET_unused1_MASK            0x1f
#define   REG_REG_MASK_SET_ref_change_event_LSB    23
#define   REG_REG_MASK_SET_ref_change_event_MASK   0x1
#define   REG_REG_MASK_SET_adv_timer_evens_LSB     19
#define   REG_REG_MASK_SET_adv_timer_evens_MASK    0xf
#define   REG_REG_MASK_SET_gpio_event_LSB          18
#define   REG_REG_MASK_SET_gpio_event_MASK         0x1
#define   REG_REG_MASK_SET_timer_hi_event_LSB      17
#define   REG_REG_MASK_SET_timer_hi_event_MASK     0x1
#define   REG_REG_MASK_SET_timer_lo_event_LSB      16
#define   REG_REG_MASK_SET_timer_lo_event_MASK     0x1
#define   REG_REG_MASK_SET_unused2_LSB             8
#define   REG_REG_MASK_SET_unused2_MASK            0xff
#define   REG_REG_MASK_SET_timer_lo_event_LSB      7
#define   REG_REG_MASK_SET_timer_lo_event_MASK     0x1
#define   REG_REG_MASK_SET_reserved_LSB            0
#define   REG_REG_MASK_SET_reserved_MASK           0x7f
#define REG_REG_MASK_CLEAR             0x008
#define   REG_REG_MASK_CLEAR_fc_hp_events_LSB      30
#define   REG_REG_MASK_CLEAR_fc_hp_events_MASK     0x3
#define   REG_REG_MASK_CLEAR_fc_err_events_LSB     29
#define   REG_REG_MASK_CLEAR_fc_err_events_MASK    0x1
#define   REG_REG_MASK_CLEAR_unused1_LSB           24
#define   REG_REG_MASK_CLEAR_unused1_MASK          0x1f
#define   REG_REG_MASK_CLEAR_ref_change_event_LSB  23
#define   REG_REG_MASK_CLEAR_ref_change_event_MASK 0x1
#define   REG_REG_MASK_CLEAR_adv_timer_evens_LSB   19
#define   REG_REG_MASK_CLEAR_adv_timer_evens_MASK  0xf
#define   REG_REG_MASK_CLEAR_gpio_event_LSB        18
#define   REG_REG_MASK_CLEAR_gpio_event_MASK       0x1
#define   REG_REG_MASK_CLEAR_timer_hi_event_LSB    17
#define   REG_REG_MASK_CLEAR_timer_hi_event_MASK   0x1
#define   REG_REG_MASK_CLEAR_timer_lo_event_LSB    16
#define   REG_REG_MASK_CLEAR_timer_lo_event_MASK   0x1
#define   REG_REG_MASK_CLEAR_unused2_LSB           8
#define   REG_REG_MASK_CLEAR_unused2_MASK          0xff
#define   REG_REG_MASK_CLEAR_timer_lo_event_LSB    7
#define   REG_REG_MASK_CLEAR_timer_lo_event_MASK   0x1
#define   REG_REG_MASK_CLEAR_reserved_LSB          0
#define   REG_REG_MASK_CLEAR_reserved_MASK         0x7f
#define REG_REG_INT                    0x00C
#define   REG_REG_INT_fc_hp_events_LSB             30
#define   REG_REG_INT_fc_hp_events_MASK            0x3
#define   REG_REG_INT_fc_err_events_LSB            29
#define   REG_REG_INT_fc_err_events_MASK           0x1
#define   REG_REG_INT_unused1_LSB                  24
#define   REG_REG_INT_unused1_MASK                 0x1f
#define   REG_REG_INT_ref_change_event_LSB         23
#define   REG_REG_INT_ref_change_event_MASK        0x1
#define   REG_REG_INT_adv_timer_evens_LSB          19
#define   REG_REG_INT_adv_timer_evens_MASK         0xf
#define   REG_REG_INT_gpio_event_LSB               18
#define   REG_REG_INT_gpio_event_MASK              0x1
#define   REG_REG_INT_timer_hi_event_LSB           17
#define   REG_REG_INT_timer_hi_event_MASK          0x1
#define   REG_REG_INT_timer_lo_event_LSB           16
#define   REG_REG_INT_timer_lo_event_MASK          0x1
#define   REG_REG_INT_unused2_LSB                  8
#define   REG_REG_INT_unused2_MASK                 0xff
#define   REG_REG_INT_timer_lo_event_LSB           7
#define   REG_REG_INT_timer_lo_event_MASK          0x1
#define   REG_REG_INT_reserved_LSB                 0
#define   REG_REG_INT_reserved_MASK                0x7f
#define REG_REG_INT_SET                0x010
#define   REG_REG_INT_SET_fc_hp_events_LSB         30
#define   REG_REG_INT_SET_fc_hp_events_MASK        0x3
#define   REG_REG_INT_SET_fc_err_events_LSB        29
#define   REG_REG_INT_SET_fc_err_events_MASK       0x1
#define   REG_REG_INT_SET_unused1_LSB              24
#define   REG_REG_INT_SET_unused1_MASK             0x1f
#define   REG_REG_INT_SET_ref_change_event_LSB     23
#define   REG_REG_INT_SET_ref_change_event_MASK    0x1
#define   REG_REG_INT_SET_adv_timer_evens_LSB      19
#define   REG_REG_INT_SET_adv_timer_evens_MASK     0xf
#define   REG_REG_INT_SET_gpio_event_LSB           18
#define   REG_REG_INT_SET_gpio_event_MASK          0x1
#define   REG_REG_INT_SET_timer_hi_event_LSB       17
#define   REG_REG_INT_SET_timer_hi_event_MASK      0x1
#define   REG_REG_INT_SET_timer_lo_event_LSB       16
#define   REG_REG_INT_SET_timer_lo_event_MASK      0x1
#define   REG_REG_INT_SET_unused2_LSB              8
#define   REG_REG_INT_SET_unused2_MASK             0xff
#define   REG_REG_INT_SET_timer_lo_event_LSB       7
#define   REG_REG_INT_SET_timer_lo_event_MASK      0x1
#define   REG_REG_INT_SET_reserved_LSB             0
#define   REG_REG_INT_SET_reserved_MASK            0x7f
#define REG_REG_INT_CLEAR              0x014
#define   REG_REG_INT_CLEAR_fc_hp_events_LSB       30
#define   REG_REG_INT_CLEAR_fc_hp_events_MASK      0x3
#define   REG_REG_INT_CLEAR_fc_err_events_LSB      29
#define   REG_REG_INT_CLEAR_fc_err_events_MASK     0x1
#define   REG_REG_INT_CLEAR_unused1_LSB            24
#define   REG_REG_INT_CLEAR_unused1_MASK           0x1f
#define   REG_REG_INT_CLEAR_ref_change_event_LSB   23
#define   REG_REG_INT_CLEAR_ref_change_event_MASK  0x1
#define   REG_REG_INT_CLEAR_adv_timer_evens_LSB    19
#define   REG_REG_INT_CLEAR_adv_timer_evens_MASK   0xf
#define   REG_REG_INT_CLEAR_gpio_event_LSB         18
#define   REG_REG_INT_CLEAR_gpio_event_MASK        0x1
#define   REG_REG_INT_CLEAR_timer_hi_event_LSB     17
#define   REG_REG_INT_CLEAR_timer_hi_event_MASK    0x1
#define   REG_REG_INT_CLEAR_timer_lo_event_LSB     16
#define   REG_REG_INT_CLEAR_timer_lo_event_MASK    0x1
#define   REG_REG_INT_CLEAR_unused2_LSB            8
#define   REG_REG_INT_CLEAR_unused2_MASK           0xff
#define   REG_REG_INT_CLEAR_timer_lo_event_LSB     7
#define   REG_REG_INT_CLEAR_timer_lo_event_MASK    0x1
#define   REG_REG_INT_CLEAR_reserved_LSB           0
#define   REG_REG_INT_CLEAR_reserved_MASK          0x7f
#define REG_REG_ACK                    0x018
#define   REG_REG_ACK_fc_hp_events_LSB             30
#define   REG_REG_ACK_fc_hp_events_MASK            0x3
#define   REG_REG_ACK_fc_err_events_LSB            29
#define   REG_REG_ACK_fc_err_events_MASK           0x1
#define   REG_REG_ACK_unused1_LSB                  24
#define   REG_REG_ACK_unused1_MASK                 0x1f
#define   REG_REG_ACK_ref_change_event_LSB         23
#define   REG_REG_ACK_ref_change_event_MASK        0x1
#define   REG_REG_ACK_adv_timer_evens_LSB          19
#define   REG_REG_ACK_adv_timer_evens_MASK         0xf
#define   REG_REG_ACK_gpio_event_LSB               18
#define   REG_REG_ACK_gpio_event_MASK              0x1
#define   REG_REG_ACK_timer_hi_event_LSB           17
#define   REG_REG_ACK_timer_hi_event_MASK          0x1
#define   REG_REG_ACK_timer_lo_event_LSB           16
#define   REG_REG_ACK_timer_lo_event_MASK          0x1
#define   REG_REG_ACK_unused2_LSB                  8
#define   REG_REG_ACK_unused2_MASK                 0xff
#define   REG_REG_ACK_timer_lo_event_LSB           7
#define   REG_REG_ACK_timer_lo_event_MASK          0x1
#define   REG_REG_ACK_reserved_LSB                 0
#define   REG_REG_ACK_reserved_MASK                0x7f
#define REG_REG_ACK_SET                0x01C
#define   REG_REG_ACK_SET_fc_hp_events_LSB         30
#define   REG_REG_ACK_SET_fc_hp_events_MASK        0x3
#define   REG_REG_ACK_SET_fc_err_events_LSB        29
#define   REG_REG_ACK_SET_fc_err_events_MASK       0x1
#define   REG_REG_ACK_SET_unused1_LSB              24
#define   REG_REG_ACK_SET_unused1_MASK             0x1f
#define   REG_REG_ACK_SET_ref_change_event_LSB     23
#define   REG_REG_ACK_SET_ref_change_event_MASK    0x1
#define   REG_REG_ACK_SET_adv_timer_evens_LSB      19
#define   REG_REG_ACK_SET_adv_timer_evens_MASK     0xf
#define   REG_REG_ACK_SET_gpio_event_LSB           18
#define   REG_REG_ACK_SET_gpio_event_MASK          0x1
#define   REG_REG_ACK_SET_timer_hi_event_LSB       17
#define   REG_REG_ACK_SET_timer_hi_event_MASK      0x1
#define   REG_REG_ACK_SET_timer_lo_event_LSB       16
#define   REG_REG_ACK_SET_timer_lo_event_MASK      0x1
#define   REG_REG_ACK_SET_unused2_LSB              8
#define   REG_REG_ACK_SET_unused2_MASK             0xff
#define   REG_REG_ACK_SET_timer_lo_event_LSB       7
#define   REG_REG_ACK_SET_timer_lo_event_MASK      0x1
#define   REG_REG_ACK_SET_reserved_LSB             0
#define   REG_REG_ACK_SET_reserved_MASK            0x7f
#define REG_REG_ACK_CLEAR              0x020
#define   REG_REG_ACK_CLEAR_fc_hp_events_LSB       30
#define   REG_REG_ACK_CLEAR_fc_hp_events_MASK      0x3
#define   REG_REG_ACK_CLEAR_fc_err_events_LSB      29
#define   REG_REG_ACK_CLEAR_fc_err_events_MASK     0x1
#define   REG_REG_ACK_CLEAR_unused1_LSB            24
#define   REG_REG_ACK_CLEAR_unused1_MASK           0x1f
#define   REG_REG_ACK_CLEAR_ref_change_event_LSB   23
#define   REG_REG_ACK_CLEAR_ref_change_event_MASK  0x1
#define   REG_REG_ACK_CLEAR_adv_timer_evens_LSB    19
#define   REG_REG_ACK_CLEAR_adv_timer_evens_MASK   0xf
#define   REG_REG_ACK_CLEAR_gpio_event_LSB         18
#define   REG_REG_ACK_CLEAR_gpio_event_MASK        0x1
#define   REG_REG_ACK_CLEAR_timer_hi_event_LSB     17
#define   REG_REG_ACK_CLEAR_timer_hi_event_MASK    0x1
#define   REG_REG_ACK_CLEAR_timer_lo_event_LSB     16
#define   REG_REG_ACK_CLEAR_timer_lo_event_MASK    0x1
#define   REG_REG_ACK_CLEAR_unused2_LSB            8
#define   REG_REG_ACK_CLEAR_unused2_MASK           0xff
#define   REG_REG_ACK_CLEAR_timer_lo_event_LSB     7
#define   REG_REG_ACK_CLEAR_timer_lo_event_MASK    0x1
#define   REG_REG_ACK_CLEAR_reserved_LSB           0
#define   REG_REG_ACK_CLEAR_reserved_MASK          0x7f
#define REG_REG_FIFO                   0x024
#define   REG_REG_FIFO_EVENT_ID_LSB                0
#define   REG_REG_FIFO_EVENT_ID_MASK               0xff

static inline uint32_t regfield_read(uint32_t reg, uint32_t mask, uint32_t lsb) {
  return (reg >> lsb) & mask;
}
static inline uint32_t regfield_write(uint32_t reg, uint32_t mask, uint32_t lsb, uint32_t value) {
  reg &= ~(mask << lsb);
  reg |= (value & mask) << lsb;
  return reg;
}
