#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include "hal/include/hal_apb_i2cs_reg_defs.h"


uint8_t hal_get_apb_i2c_slave_address(void)
{

	ApbI2cs_t *apbI2cSlave = (ApbI2cs_t*)I2CS_START_ADDR;
	return (uint8_t )apbI2cSlave->i2cs_dev_address_b.slave_addr;
}

void hal_set_apb_i2c_slave_address(uint8_t aSlaveAddress)
{

	ApbI2cs_t *apbI2cSlave = (ApbI2cs_t*) I2CS_START_ADDR;
	apbI2cSlave->i2cs_dev_address_b.slave_addr = aSlaveAddress;
}

uint8_t hal_set_apb_i2c_slave_on_off(uint8_t aStatus)
{

	ApbI2cs_t *apbI2cSlave = (ApbI2cs_t*)I2CS_START_ADDR;
	if( aStatus == 1 )
		apbI2cSlave->i2cs_enable_b.ip_enable = 1;
	else if( aStatus == 0 )
		apbI2cSlave->i2cs_enable_b.ip_enable = 0;
	return (uint8_t)apbI2cSlave->i2cs_enable_b.ip_enable;
}


