#ifndef __I2C_PROTOCOL_H__
#define __I2C_PROTOCOL_H__

#define MAX_NB_AREA		16

typedef struct {
  uint32_t nextDesc;
  uint32_t nbAreas;
  uint32_t entry;
  uint32_t bootaddr;
} __packed flash_v2_header_t;

typedef struct {
  uint32_t start;
  uint32_t ptr;
  uint32_t size;
  uint32_t blocks;
} __packed flash_v2_mem_area_t;

typedef struct {	//16+256 = 272 bytes of header.
  flash_v2_header_t header;
  flash_v2_mem_area_t memArea[MAX_NB_AREA];
} __packed boot_code_t;

typedef struct {
  uint16_t SOF;
  uint32_t A2RamAddress;
  uint8_t CmdType;
  uint8_t DataLen;
  uint8_t Data[2];    //Indicate start of data.
}__packed I2CProtocolFrame_t;

typedef union {
  uint32_t w;
  uint8_t b[4];
} split_4Byte_t ;

typedef union {
  uint16_t hw;
  uint8_t b[2];
} split_2Byte_t ;

#define I2C_MASTER_REG_DEV_ADDRESS                	0x00
#define I2C_MASTER_REG_ENABLE                  		0x01
#define I2C_MASTER_REG_DEBOUNCE_LENGTH              0x02
#define I2C_MASTER_REG_SCL_DELAY_LENGTH              0x03
#define I2C_MASTER_REG_SDA_DELAY_LENGTH              0x04
#define I2C_MASTER_REG_MSG_I2C_APB                		0x10
#define I2C_MASTER_REG_MSG_I2C_APB_STATUS            0x11
#define I2C_MASTER_REG_MSG_APB_I2C                		0x12
#define I2C_MASTER_REG_MSG_APB_I2C_STATUS            0x13
#define I2C_MASTER_REG_FIFO_I2C_APB_WRITE_DATA_PORT        0x20
#define I2C_MASTER_REG_FIFO_I2C_APB_READ_DATA_PORT        0x21
#define I2C_MASTER_REG_FIFO_I2C_APB_FLUSH            0x22
#define I2C_MASTER_REG_FIFO_I2C_APB_WRITE_FLAGS          0x23
#define I2C_MASTER_REG_FIFO_I2C_APB_READ_FLAGS          0x24
#define I2C_MASTER_REG_FIFO_APB_I2C_WRITE_DATA_PORT        0x30
#define I2C_MASTER_REG_FIFO_APB_I2C_READ_DATA_PORT        0x31
#define I2C_MASTER_REG_FIFO_APB_I2C_FLUSH            0x32
#define I2C_MASTER_REG_FIFO_APB_I2C_WRITE_FLAGS          0x33
#define I2C_MASTER_REG_FIFO_APB_I2C_READ_FLAGS          0x34
#define I2C_MASTER_REG_INTERRUPT_STATUS              0x40
#define I2C_MASTER_REG_INTERRUPT_ENABLE              0x41
#define I2C_MASTER_REG_INTERRUPT_I2C_APB_WRITE_FLAGS_SELECT    0x42
#define I2C_MASTER_REG_INTERRUPT_APB_I2C_READ_FLAGS_SELECT    0x43
#define I2C_MASTER_REG_INTERRUPT_TO_APB_STATUS          0x50
#define I2C_MASTER_REG_INTERRUPT_TO_APB_ENABLE          0x51
#define I2C_MASTER_REG_INTERRUPT_APB_I2C_WRITE_FLAGS_SELECT   0x52
#define I2C_MASTER_REG_INTERRUPT_I2C_APB_READ_FLAGS_SELECT    0x53

#define A2_APP_I2C_SLAVE_ADDRESS			0x62
#define A2_APP_I2C_SLAVE_ADDRESS_7BIT     	( A2_APP_I2C_SLAVE_ADDRESS << 1 ) //0xDE//0x40//0xD0
#define A2_BL_I2C_SLAVE_ADDRESS  			0x62
#define A2_BL_I2C_SLAVE_ADDRESS_7BIT     	( A2_BL_I2C_SLAVE_ADDRESS << 1 )


#define I2C_PROTOCOL_MAX_PAYLOAD_SIZE            240

#define I2C_PROTOCOL_FRAME_HEADER_SIZE      (sizeof(I2CProtocolFrame_t) - 2)
#define I2C_PROTOCOL_FRAME_CRC_SIZE        2
#define A2_TO_HOST_FRAME_HEADER      0x5A70
#define HOST_TO_A2_FRAME_HEADER      0xA507

#define A2_I2C_BL_WITHOUT_CRC_IS_READY_CHECK_CMD   		0x19
#define A2_I2C_BL_WITH_CRC_IS_READY_CHECK_CMD     		0x20
#define A2_I2C_BL_IS_READY_CHECK_RSP_YES  				0x21
#define A2_I2C_BL_IS_READY_CHECK_RSP_NO    				0x22
#define I2C_NEW_FRAME_READY_BYTE       					0x23
#define A2_LOAD_MEMORY_CMD          					0x24
#define A2_READ_MEMORY_CMD          					0x25
#define A2_JUMP_TO_ADDRESS_CMD        					0x26
#define A2_RESET_REASON_POR          					0x27
#define A2_RESET_REASON_WDT          					0x28
#define A2_RESET_REASON_BUTTON_PRESS    				0x29
#define A2_CORRUPTED_FRAME_RCVD           				0x30
#define A2_GOOD_FRAME_RCVD              				0x31

uint16_t formI2CProtocolFrame(uint8_t *aBuf, uint16_t aBufSize, uint8_t aCmdType, uint32_t aA2RamAddress, const uint8_t *aData, uint8_t aDataLen, uint8_t aCRCOnOff);
void processI2CProtocolFrames(void);
uint16_t sendI2CProtocolFrame(uint8_t *aI2CProtocolFrameBuf, uint16_t aFrameSize);
void readHeaderData(void);
uint8_t programArnold2(uint8_t aCRCOnOff);
uint32_t getSizeOfA2FwBuf(void);

#endif
