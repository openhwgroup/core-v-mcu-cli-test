/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (C) 2020 ETH Zurich
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 8 spaces!
 */


/*
 * Create implementation of vPortSetupTimerInterrupt() if the CLINT is not
 * available, but make sure the configCLINT_BASE_ADDRESS constant is still
 * defined.
 *
 * Define vPortHandleInterrupt to whatever the interrupt handler is called.  In
 * this case done by defining vPortHandleInterrupt=SystemIrqHandler on the
 * assembler command line as SystemIrqHandler is referenced from both FreeRTOS
 * code and the libraries that come with the Vega board.
 */

/* PULPissimo includes. */
#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include "target/core-v-mcu/include/core-v-mcu-system.h"
#include "hal/include/hal_timer_irq.h"
#include "hal/include/hal_fll.h"
#include "hal/include/hal_irq.h"
#include "drivers/include/udma_uart_driver.h"

/******************************************************************************
 * This project provides two demo applications.  A simple blinky style project,
 * and a more comprehensive test and demo application.  The
 * mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting (defined in this file) is used to
 * select between the two.  The simply blinky demo is implemented and described
 * in main_blinky.c.  The more comprehensive test and demo application is
 * implemented and described in main_full.c.
 *
 * This file implements the code that is not demo specific, including the
 * hardware setup and standard FreeRTOS hook functions.
 *
 * ENSURE TO READ THE DOCUMENTATION PAGE FOR THIS PORT AND DEMO APPLICATION ON
 * THE http://www.FreeRTOS.org WEB SITE FOR FULL INFORMATION ON USING THIS DEMO
 * APPLICATION, AND ITS ASSOCIATE FreeRTOS ARCHITECTURE PORT!
 *
 */


#include "hal/include/hal_udma_i2cm_reg_defs.h"
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

char gsPrintfBuffer[256] = {0};
uint8_t gDebugEnabledFlg = 0;

/* Prepare hardware to run the demo. */
static void prvSetupHardware( void );
void person_detection_task( void *pParameter );

int MicroVsnprintf(char* output, int len, const char* format, va_list args);
int oPrintf(const char* format, ...);
void CLI_printf( const char *fmt, ... );


int main(void)
{
    prvSetupHardware();

	/* The mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is described at the top
	of this file. */

	//CLI_start_task( my_main_menu );
    //person_detect_task_start(NULL);
    person_detection_task(0);
	/* Start the tasks and timer running. */
	//vTaskStartScheduler();
	/* If all is well, the scheduler will now be running, and the following
		line will never be reached.  If the following line does execute, then
		there was insufficient FreeRTOS heap memory available for the Idle and/or
		timer tasks to be created.  See the memory management section on the
		FreeRTOS web site for more details on the FreeRTOS heap
		http://www.freertos.org/a00111.html. */

	while(1);
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{
	/* Init board hardware. */
	system_init();
}
/*-----------------------------------------------------------*/

void vToggleLED( void )
{
	gpio_pin_toggle( 0x5 );
}
/*-----------------------------------------------------------*/

void DebugLog(const char* s)
{
	uint8_t lChar = 0;
	uint8_t lCR = 0x0D;
	while(*s != '\0')
	{
		lChar = *s;
		if( lChar == '\n')
			udma_uart_writeraw(0,1,&lCR);
		udma_uart_writeraw(0,1,&lChar);
		s++;
	}
}

/*
 * Use oPrintf to print debug messages as it can be turned off with gDebugEnabledFlg
 *
 *
 * */
int oPrintf(const char* format, ...)
{
	int lRetVal = 0;
	if( gDebugEnabledFlg == 0 )
		return 0;
	va_list args;
	va_start(args, format);
	lRetVal = MicroVsnprintf(gsPrintfBuffer, 256, format, args);
	DebugLog(gsPrintfBuffer);
	va_end(args);
	return lRetVal;
}

/* workhorse for printf() */
void CLI_vprintf( const char *fmt, va_list ap )
{
    vsnprintf( gsPrintfBuffer, sizeof(gsPrintfBuffer)-1, fmt, ap );
    gsPrintfBuffer[ sizeof(gsPrintfBuffer) - 1 ] = 0;
    DebugLog(gsPrintfBuffer);
}

/*
 * Use CLI_printf to print normal messages to indicate to user and not for debug
 *
 *
 * */
/* printf for test purposes */
void CLI_printf( const char *fmt, ... )
{
    va_list ap;

    va_start(ap,fmt);
    CLI_vprintf( fmt, ap );
    va_end(ap);
}



