#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "tm_stm32_i2c.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_disco.h"
#include "I2CProtocol.h"
#include "crc.h"

extern const unsigned char gArnold2AppFWBuf[];
uint8_t gsI2CProtocolFrameRxBuf[256] = {0};


static uint8_t gsI2CProtocolFrameTxBuf[256] = {0};
static uint16_t gsI2CProtocolFrameCounter = 0;

boot_code_t *gHeaderData = (boot_code_t *)NULL;

uint16_t I2CProtocolFrameCalChksum(uint8_t *cbuf, uint8_t pkt_size)
{
    uint16_t lCRC = 0;
    lCRC = (uint16_t)crcFast(cbuf, pkt_size);
    return lCRC;
}

uint16_t formI2CProtocolFrame(uint8_t *aBuf, uint16_t aBufSize, uint8_t aCmdType, uint32_t aA2RamAddress, const uint8_t *aData, uint8_t aDataLen)
{
	uint8_t i = 0;
	uint16_t ui16Crc = 0;
	I2CProtocolFrame_t *lFillPtr = (I2CProtocolFrame_t *)aBuf;
	if( aDataLen <= I2C_PROTOCOL_MAX_PAYLOAD_SIZE )
	{
		lFillPtr->SOF = HOST_TO_A2_FRAME_HEADER;
		lFillPtr->A2RamAddress = aA2RamAddress;
		lFillPtr->CmdType = aCmdType;
		lFillPtr->DataLen = aDataLen;
		for( i=0; i<aDataLen; i++)
		{
			if( aData )
			{
				lFillPtr->Data[i] = aData[i];
			}
		}

		ui16Crc = I2CProtocolFrameCalChksum(aBuf, i+I2C_PROTOCOL_FRAME_HEADER_SIZE);
		lFillPtr->Data[i++] = (uint8_t)(ui16Crc >> 8 );
		lFillPtr->Data[i++] = (uint8_t)ui16Crc;
		return i + I2C_PROTOCOL_FRAME_HEADER_SIZE;
	}
	return 0;
}

uint16_t sendI2CProtocolFrame(uint8_t *aI2CProtocolFrameBuf, uint16_t aFrameSize)
{
	uint16_t i = 0;
	uint16_t lFrameSize = 0;
	uint8_t *lI2CProtocolFramePtr = (uint8_t *)NULL;
	static uint16_t lastSentFrameSize = 0;

	//if NULL and 0 are passed as parameters, last sent frame will be sent again.
	if( ( aI2CProtocolFrameBuf == NULL ) && ( aFrameSize == 0 ) )
	{
		lI2CProtocolFramePtr = gsI2CProtocolFrameTxBuf;
		lFrameSize = lastSentFrameSize;
	}
	else
	{

		lI2CProtocolFramePtr = aI2CProtocolFrameBuf;
		lFrameSize = aFrameSize;
		lastSentFrameSize = aFrameSize;
	}

	TM_I2C_Write(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_APB_I2C_FLUSH, 0x1);
	Delay(50);
	TM_I2C_Write(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_APB_I2C_FLUSH, 0x0);
	for(i=0; i<lFrameSize; i++)
	{
		TM_I2C_Write(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_I2C_APB_WRITE_DATA_PORT, lI2CProtocolFramePtr[i]);
	}
	TM_I2C_Write(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_I2C_APB, I2C_NEW_FRAME_READY_BYTE);

	gsI2CProtocolFrameCounter++;		//Will roll over.
	return i;
}

void parseI2CProtocolFrame(uint8_t *aBuf, uint16_t aLen)
{
	uint16_t i = 0;
	split_4Byte_t lAddr;
	split_2Byte_t lDataLen;
	I2CProtocolFrame_t *lParseFramePtr = (I2CProtocolFrame_t *)aBuf;

	if(lParseFramePtr->SOF == HOST_TO_A2_FRAME_HEADER )
	{

	}
}

void programArnold2(void)
{
	uint32_t lStartOffset = 0, lFwBufOffset = 0;
	uint32_t lTotalSize = 0;
	uint32_t lLoadAddressStart = 0, lLoadAddressOffset = 0;
	uint32_t lRemainingbytes = 0, lBytesToSend = 0;
	uint16_t lCounter = 0, len = 0;
	uint8_t lSts = 0, lData = 0;
	uint32_t lIndicatorCnt = 0;

	if( gHeaderData != NULL )
	{
		if( gHeaderData->header.nbAreas == 1 )	//Currently supporting only one NB Area
		{
			lStartOffset = gHeaderData->memArea[0].start;
			lTotalSize = getSizeOfA2FwBuf();
			lRemainingbytes = lTotalSize - gHeaderData->memArea[0].start;
			lLoadAddressStart = gHeaderData->memArea[0].ptr;
			//Check if A2 BL slave is ready?
			while( lRemainingbytes != 0 )
			{
				do{
					TM_I2C_Write(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_I2C_APB, A2_I2C_BL_IS_READY_CHECK_CMD);

					do{
						TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C_STATUS, &lSts);
						lCounter++;
					}while( lSts == 0 );

					TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C, &lData);
				}while(lData != A2_I2C_BL_IS_READY_CHECK_RSP_YES);
				if( lRemainingbytes > I2C_PROTOCOL_MAX_PAYLOAD_SIZE )
					lBytesToSend = I2C_PROTOCOL_MAX_PAYLOAD_SIZE;
				else
					lBytesToSend = lRemainingbytes;
				len = formI2CProtocolFrame(gsI2CProtocolFrameTxBuf, 256, A2_LOAD_MEMORY_CMD, (lLoadAddressStart + lLoadAddressOffset), &gArnold2AppFWBuf[lStartOffset + lFwBufOffset], lBytesToSend);
				sendI2CProtocolFrame(gsI2CProtocolFrameTxBuf, len);

				lRemainingbytes -= lBytesToSend;
				lLoadAddressOffset += lBytesToSend;
				lFwBufOffset += lBytesToSend;

				lIndicatorCnt++;
				if( lIndicatorCnt >= 20 )
				{
					lIndicatorCnt = 0;
					TM_DISCO_LedToggle(LED_RED);
				}
			}
			TM_DISCO_LedOff(LED_RED);

			do{
				TM_I2C_Write(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_I2C_APB, A2_I2C_BL_IS_READY_CHECK_CMD);

				do{
					TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C_STATUS, &lSts);
					lCounter++;
				}while( lSts == 0 );

				TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C, &lData);
			}while(lData != A2_I2C_BL_IS_READY_CHECK_RSP_YES);

			len = formI2CProtocolFrame(gsI2CProtocolFrameTxBuf, 256, A2_JUMP_TO_ADDRESS_CMD, gHeaderData->header.entry, NULL, 0);
			sendI2CProtocolFrame(gsI2CProtocolFrameTxBuf, len);

			TM_DISCO_LedOn(LED_ORANGE);
		}
	}
#if 0
	if( lData == I2C_NEW_MSG_READY_BYTE )
	{
		//APB reads out the written data.
		//If the read FIFO is not empty
		i = 0;
		while( 1 )
		{
			TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_APB_I2C_READ_FLAGS, &lData);
			if( lData != 0 )
			{
				TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_FIFO_APB_I2C_READ_DATA_PORT, &lData);
				gsI2CProtocolFrameRxBuf[i] = lData;
				i++;
			}
			else
				break;
		}
		parseI2CProtocolFrame(gsI2CProtocolFrameRxBuf, i);
	}
#endif

}


void readHeaderData(void)
{
	gHeaderData = (boot_code_t *)gArnold2AppFWBuf;
}

