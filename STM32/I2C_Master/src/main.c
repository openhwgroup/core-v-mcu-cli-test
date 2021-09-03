/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

/* Include core modules */
#include "stm32f4xx_hal.h"
#include "tm_stm32_rcc.h"
#include "tm_stm32_disco.h"
#include "tm_stm32_delay.h"
#include "tm_stm32_i2c.h"
#include "I2CProtocol.h"
#include "crc.h"

extern const unsigned char gArnold2AppFWBuf[];
/* MPU-6050 device address */


/* Byte value read from external device */
uint8_t read;
uint16_t addr = 0;
int main(void)
{
	uint32_t lCounter = 0;
	uint8_t lSts = 0, lData = 0;
	uint32_t lSize = 0;

    /* Init system clock for maximum system speed */
    TM_RCC_InitSystem();

    /* Init HAL layer */
    HAL_Init();

    /* Init leds */
    TM_DISCO_LedInit();

    /* Init delay */
    TM_DELAY_Init();

    /* Init I2C, SCL = PB8, SDA = PB9, available on Arduino headers and on all discovery boards */
    /* For STM32F4xx and STM32F7xx lines */
    TM_I2C_Init(I2C1, TM_I2C_PinsPack_3, 400000);

    readHeaderData();
    crcInit();
    /*
    for(addr=0; addr<256; addr++ )
    {
    	if( TM_I2C_Read(I2C1, addr, 0x2, &read) == TM_I2C_Result_Ok )
    	{
    		TM_DISCO_LedOn(LED_GREEN);
    	}
    }*/

    /* Read one byte, device address = MPU6050_ADDRESS, register address = 0x1A */
    TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, 0x0, &read);

    /* Check value */
    if (read == A2_BL_I2C_SLAVE_ADDRESS) {
        TM_DISCO_LedOn(LED_GREEN);
    } else {
        /* Toggle LED, indicate wrong */
        while (1) {
            /* Toggle LED */
            TM_DISCO_LedToggle(LED_ALL);

            /* Delay 100ms */
            Delayms(100);
        }
    }


    while (1) {
    	if( TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C_STATUS, &lSts) == TM_I2C_Result_Ok )
    	{
			if( lSts == 1 )
			{
				TM_I2C_Read(I2C1, A2_BL_I2C_SLAVE_ADDRESS_7BIT, I2C_MASTER_REG_MSG_APB_I2C, &lData);

				if( lData == A2_RESET_REASON_POR)
				{
					TM_DISCO_LedOff(LED_BLUE);
					TM_DISCO_LedOff(LED_ORANGE);
					programArnold2();
				}
			}
    	}
    	lCounter++;
    	if( lCounter >= 10000 )
    	{
    		TM_DISCO_LedToggle(LED_BLUE);
    		lCounter = 0;
    	}
    }
}
