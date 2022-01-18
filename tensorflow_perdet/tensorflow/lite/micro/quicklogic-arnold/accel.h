#ifndef ACCEL_H
#define ACCEL_H

enum accel  {					// NOTE: these will be used as bits, so make them powers of 2
	accel_none 	= 0x00,
	accel_sw		= 0x01,		
	accel_fpga	= 0x02,
	accel_print		= 0x04
};

#endif	//ACCEL_H