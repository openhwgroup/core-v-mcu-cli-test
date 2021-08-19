/*==========================================================
 * Copyright 2020 QuickLogic Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *==========================================================*/

/*==========================================================
 *                                                          
 *    File   : cli_platform.c
 *    Purpose: command line interface for smart remote demo application. 
 * $Revision: 4224$
 * $Date: 2011-02-22$
 *=========================================================*/

/** @file cli_platform.c */


#include "libs/cli/include/cli.h"
#include "FreeRTOS.h"
#include "task.h"
//#include <eoss3_hal_uart.h>
#include "drivers/include/udma_uart_driver.h"
//#include "RtosTask.h"
#include "SDKConfig.h"
#include <setjmp.h>
#include "libs/utils/include/dbg_uart.h"
#include "string.h"
/* These are PLATFORM specific functions that the CLI code requires */

struct cli CLI_common;
extern const char *SOFTWARE_VERSION_STR;
void CLI_dispatch(void);
extern uint8_t gSimulatorEnabledFlg;
extern uint8_t gFilterPrintMsgFlg;
uint8_t gSimulatorCmdTableIndex = 0;

const char *gSimulatorCmdTable[] = {
		"exit",
		"misc info",
		"i2cs on",
		"advtmr all",
		"efpga tcdm",
		"efpga tcdm_st",
		"efpga math0mult0",
		"efpga math0mult1",
		"efpga math1mult0",
		"efpga math1mult1",
		"efpga ram",
		"efpga mlt",
		"efpga rw",
		"efpga auto",
		"efpgaio all",
		"fcb pgm",
		"gpio event",
		"gpio evnt",
		"gpio all",
		"io setmux 23 2",
		"io setmux 24 2",
		"i2cm0 writebyte 0xDE 0x10 0x3D",
		"i2cm0 readbyte 0xDE 0x10 0xFF",
		"i2cm0 writebyte 0xC4 0x10 0x5C",
		"i2cm0 readbyte 0xC4 0x10 0xFF",
		"io setmux 23 0",
		"io setmux 24 0",
		"i2cm0 writebyte 0xDE 0x10 0x5C",
		"i2cm0 readbyte 0xDE 0x10 0x5C",
		"i2cm0 testsinglebyte 0xDE 0x10",
		"i2cm0 writebyte 0xC4 0x10 0x5C",
		"i2cm0 readbyte 0xC4 0x10 0x5C",
		"i2cm0 testsinglebyte 0xC4 0x10",
		"io setmux 46 2",
		"io setmux 47 2",
		"i2cm1 readbyte 0x96 0x0B 0xFF",
		"io setmux 46 0",
		"io setmux 47 0",
		"i2cm1 temp",
		"i2cm1 dev_id",
		"io setmux 11 2",
		"mem start",
		"mem test check",
		"mem test barr",
		"qspi flashid",
		"qspi erase 0x00100000",
		"qspi init",
		"qspi flash_qpoke 0x00100000 0x01234567",
		"qspi flash_qpeek 0x00100000 0x01234567",
		"qspi flash_qpoke 0x00100004 0x89ABCDEF",
		"qspi flash_qpeek 0x00100004 0x89ABCDEF",
		"qspi flash_qpoke 0x00100008 0xDEADCAFE",
		"qspi flash_qpeek 0x00100008 0xDEADCAFE",
		"qspi flash_qpoke 0x0010000C 0xDEADBEEF",
		"qspi flash_qpeek 0x0010000C 0xDEADBEEF",
		"qspi flash_qpoke 0x00100010 0xAAAAAAAA",
		"qspi flash_qpeek 0x00100010 0xAAAAAAAA",
		"qspi flash_qpoke 0x00100014 0x55555555",
		"qspi flash_qpeek 0x00100014 0x55555555",
		"qspi reset",
		"qspi erase 0x00100000",
		"qspi flash_poke 0x00100000 0x01000000",
		"qspi flash_peek 0x00100000 0x01000000",
		"qspi flash_poke 0x00100004 0x02000000",
		"qspi flash_peek 0x00100004 0x02000000",
		"qspi flash_poke 0x00100008 0x04000000",
		"qspi flash_peek 0x00100008 0x04000000",
		"qspi flash_poke 0x0010000C 0x08000000",
		"qspi flash_peek 0x0010000C 0x08000000",
		"qspi flash_poke 0x00100010 0x10000000",
		"qspi flash_peek 0x00100010 0x10000000",
		"qspi flash_poke 0x00100014 0x20000000",
		"qspi flash_peek 0x00100014 0x20000000",
		"intr all",
		"i2cs i2c2apbmsg",
		"i2cs apb2i2cmsg",
		"i2cs i2c2apbfifo",
		"i2cs apb2i2cfifo",
		"i2cs i2c2apbfifowm",
		"i2cs apb2i2cfifowm",
		"i2cs i2c2apbflush",
		"i2cs apb2i2cflush",
		"uart1 tx abcdefghijklmnopqrstuvwxyz",
		"uart1 tx `~!@#$%^&*()_-=+",
		"uart1 tx []{};':",
		"uart1 tx <>,.?",
		"uart1 tx 0123456789",
		"uart1 tx hello",
		"uart1 tx hello.this.is.a.test.",
		"efpga tcdm_status",
		"efpga tcdm_sp",
		NULL
};
#ifdef DISABLE_UART_PRINTS
#define uart_tx(x,y)		do{}while(0);
#else
//extern void uart_tx(int, int);
#endif

xTaskHandle xHandleTestCli;

uint32_t CLI_time_now(void)
{
  return xTaskGetTickCount();
}

intptr_t CLI_timeout_start(void)
{
  return (intptr_t)CLI_time_now();
}

void CLI_beep(void)
{
  /* ascii bell */
  CLI_putc( 0x07 );
}

int CLI_timeout_expired( intptr_t token, int timeout )
{
  int32_t delta;
  if( timeout == 0 ){
    return 1;
  }
  if( timeout < 0 ){
    /* forever */
    return 0;
  }
  uint32_t now;
  
  now = CLI_time_now();
  
  delta = ((int32_t)(now))- (int32_t)(token);
  if( delta > timeout ){
    return 1;
  } else {
    return 0;
  }
}

void CLI_putc_raw(int c)
{
	udma_uart_writeraw(UART_ID_CONSOLE, 1, (uint8_t*) &c);
}

int CLI_getkey_raw( int timeout )
{
  intptr_t tstart;
  uint16_t x;
  
  tstart = CLI_timeout_start();
  for(;;){
    //uart_rx_wait( UART_ID_CONSOLE, timeout );
    if( udma_uart_control( UART_ID_CONSOLE , kUartDataValid, NULL) ){
      x = udma_uart_getchar( UART_ID_CONSOLE );
      if (x != 0)							// FIXME: why do we get nullls?
    	  return (int)x;
    }
    /* no key */
    if( CLI_timeout_expired(tstart, timeout ) ){
      break;
    }
    vTaskDelay( 10 );
  }
  return EOF;
}

void CLI_task( void *pParameter )
{
    (void)(pParameter);
    int k;

    //wait_ffe_fpga_load();
    /* set to 1 to have a timestamp on the side */
    vTaskDelay(100);
    
    CLI_common.timestamps = 0;

    CLI_printf("#*******************\n");
    CLI_printf("Command Line Interface\n");
    CLI_printf("App SW Version: %s\n", SOFTWARE_VERSION_STR );
    CLI_printf("#*******************\n");
    CLI_print_prompt();
    for(;;){
    	if( gSimulatorEnabledFlg == 0 )
    	{
			k = CLI_getkey( 10*1000 );
			if( k == EOF ){
				continue;
			}
			CLI_rx_byte( k );
    	}
    	else
    	{
    		if( gSimulatorCmdTable[gSimulatorCmdTableIndex] != NULL )
    		{
    			memcpy( (void *)(&CLI_common.cmdline[0]), gSimulatorCmdTable[gSimulatorCmdTableIndex], strlen(gSimulatorCmdTable[gSimulatorCmdTableIndex]) );
    			CLI_dispatch();
				/*
				 * NOTE: Above dispatch() call might not return!
				 * If an error occurs, the long jump will occur.
				 */
    			/* clean up from last */
				memset( (void *)(&CLI_common.cmdline[0]), 0, sizeof(CLI_common.cmdline) );
    			gSimulatorCmdTableIndex++;
    		}
    		else
    		{
    			gSimulatorEnabledFlg = 0;
    			if( gFilterPrintMsgFlg == 1 )
    				gFilterPrintMsgFlg = 0;

    			CLI_cmd_stack_clear();
				memset( (void *)(&(CLI_common.cmdline[0])), 0, sizeof(CLI_common.cmdline) );
				CLI_printf("Simul Done\n");
				CLI_print_prompt();
    		}
    	}

    }
}



void CLI_start_task(const struct cli_cmd_entry *pMainMenu)
{
    CLI_init( pMainMenu );
    xTaskCreate ( CLI_task, "CLI", 14 * CLI_TASK_STACKSIZE, NULL, (UBaseType_t)(tskIDLE_PRIORITY+2), &xHandleTestCli);
    configASSERT( xHandleTestCli );
}
