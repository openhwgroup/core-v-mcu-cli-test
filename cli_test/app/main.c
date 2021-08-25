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

/* FreeRTOS kernel includes. */
#include <FreeRTOS.h>
#include <hal/include/hal_gpio_pulp.h>
#include <task.h>

/* c stdlib */
#include <stdio.h>
#include "string.h"

/* PULPissimo includes. */
#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include "target/core-v-mcu/include/core-v-mcu-system.h"
#include "target/core-v-mcu/include/csr.h"

#include "hal/include/hal_timer_irq.h"
#include "hal/include/hal_fll.h"
#include "hal/include/hal_irq.h"
#include "drivers/include/udma_uart_driver.h"
#include <app/include/i2c_task.h>
#include <app/include/efpga_tests.h>

#include "libs/cli/include/cli.h"
#include "hal/include/hal_udma_i2cm_reg_defs.h"
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

/* Set mainCREATE_SIMPLE_BLINKY_DEMO_ONLY to one to run the simple blinky demo,
or 0 to run the more comprehensive test and demo application. */
/* #define mainCREATE_SIMPLE_BLINKY_DEMO_ONLY	0*/

/*
 * main_blinky() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 1.
 * main_full() is used when mainCREATE_SIMPLE_BLINKY_DEMO_ONLY is set to 0.
 */

extern void main_blinky( void );

/* Prototypes for the standard FreeRTOS callback/hook functions implemented
within this file.  See https://www.freertos.org/a00016.html */
void vApplicationMallocFailedHook( void );
void vApplicationIdleHook( void );
void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName );
void vApplicationTickHook( void );

/* Prepare hardware to run the demo. */
static void prvSetupHardware( void );
void CLI_dispatch(void);
uint8_t runSimulatorCommands(void);
uint8_t UartGetString(uint8_t aPortNum, char *aCmdLineBuf, int16_t aBufSize, uint8_t aTerminatingChar);

/*-----------------------------------------------------------*/


char* SOFTWARE_VERSION_STR = "cli_test v0.2 - NoInt \n";
extern const struct cli_cmd_entry my_main_menu[];
extern uint8_t gSimulatorEnabledFlg;

#define CMD_LINE_BUF_SIZE       32
#define TERMINATING_CHARACTER   '\r'
static char gsCommandLineBuf[CMD_LINE_BUF_SIZE] = {0};

int main(void)
{
	uint32_t RegReadVal = 0;
    prvSetupHardware();

	/* The mainCREATE_SIMPLE_BLINKY_DEMO_ONLY setting is described at the top
	of this file. */
#if ( USE_FREE_RTOS == 1 )
	CLI_start_task( my_main_menu );

	/* Start the tasks and timer running. */
	vTaskStartScheduler();
	/* If all is well, the scheduler will now be running, and the following
		line will never be reached.  If the following line does execute, then
		there was insufficient FreeRTOS heap memory available for the Idle and/or
		timer tasks to be created.  See the memory management section on the
		FreeRTOS web site for more details on the FreeRTOS heap
		http://www.freertos.org/a00111.html. */
#else

	RegReadVal = csr_read(CSR_MSTATUS);
	if( ( RegReadVal & MSTATUS_IE ) != 0 )	//Check if global interrupt is enabled.
	{
		//Do nothing.
	}
	else
	{
		//enable global interrupt.
		csr_read_set(CSR_MSTATUS, MSTATUS_IE);
		RegReadVal = csr_read(CSR_MSTATUS);
		//CLI_printf("CSR_MSTATUS 0x%08x\n", RegReadVal);
		//dbg_str("<<DONE>>\r\n");

	}

	RegReadVal = csr_read(CSR_MIE);
	if( ( RegReadVal & BIT(11) ) != 0 )	//Check if the event interrupt mask is open.
	{
		//Do nothing.
	}
	else
	{
		//open the event interrupt mask.
		csr_read_set(CSR_MIE, BIT(11));
		RegReadVal = csr_read(CSR_MIE);
		//CLI_printf("CSR_MIE 0x%08x\n", RegReadVal);
		//dbg_str("<<DONE>>\r\n");

	}
    //CLI_printf("#*******************\n");
    //CLI_printf("Command Line Interface\n");
    CLI_printf("App SW Version: %s\n", SOFTWARE_VERSION_STR );
    //CLI_printf("#*******************\n");
    //CLI_init( NULL );//my_main_menu );
    CLI_init( my_main_menu );
    CLI_print_prompt();
#endif

	while(1)
	{
#if ( USE_FREE_RTOS == 0 )
		if(UartGetString(0,gsCommandLineBuf, CMD_LINE_BUF_SIZE, TERMINATING_CHARACTER) == 1)
		{
			memcpy( (void *)(&CLI_common.cmdline[0]), gsCommandLineBuf, strlen(gsCommandLineBuf) );
			CLI_dispatch();
			/*
			 * NOTE: Above dispatch() call might not return!
			 * If an error occurs, the long jump will occur.
			 */
			/* clean up from last */
			memset( (void *)(&CLI_common.cmdline[0]), 0, sizeof(CLI_common.cmdline) );
			CLI_print_prompt();
		}

		//Execute simulator command table
		if(gSimulatorEnabledFlg == 1 )
		{
			if( runSimulatorCommands() == 1 )	//Once done, switch off the flag
				gSimulatorEnabledFlg = 0;
		}
#endif
	}
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


void vApplicationMallocFailedHook( void )
{
	/* vApplicationMallocFailedHook() will only be called if
	configUSE_MALLOC_FAILED_HOOK is set to 1 in FreeRTOSConfig.h.  It is a hook
	function that will get called if a call to pvPortMalloc() fails.
	pvPortMalloc() is called internally by the kernel whenever a task, queue,
	timer or semaphore is created.  It is also called by various parts of the
	demo application.  If heap_1.c or heap_2.c are used, then the size of the
	heap available to pvPortMalloc() is defined by configTOTAL_HEAP_SIZE in
	FreeRTOSConfig.h, and the xPortGetFreeHeapSize() API function can be used
	to query the size of free heap space that remains (although it does not
	provide information on how the remaining heap might be fragmented). */
	taskDISABLE_INTERRUPTS();
//	printf( "error: application malloc failed\n" );
	__asm volatile( "ebreak" );
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
	/* vApplicationIdleHook() will only be called if configUSE_IDLE_HOOK is set
	to 1 in FreeRTOSConfig.h.  It will be called on each iteration of the idle
	task.  It is essential that code added to this hook function never attempts
	to block in any way (for example, call xQueueReceive() with a block time
	specified, or call vTaskDelay()).  If the application makes use of the
	vTaskDelete() API function (as this demo application does) then it is also
	important that vApplicationIdleHook() is permitted to return to its calling
	function, because it is the responsibility of the idle task to clean up
	memory allocated by the kernel to any task that has since been deleted. */
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	__asm volatile( "ebreak" );
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationTickHook( void )
{
	/* The tests in the full demo expect some interaction with interrupts. */
	#if( mainCREATE_SIMPLE_BLINKY_DEMO_ONLY != 1 )
	{
		extern void vFullDemoTickHook( void );
		vFullDemoTickHook();
	}
	#endif
}
/*-----------------------------------------------------------*/

/*
 Uart2GetString does not store the terminating character. It will give out a
 * NULL terminated string.
 */
uint8_t UartGetString(uint8_t aPortNum, char *aCmdLineBuf, int16_t aBufSize, uint8_t aTerminatingChar)
{
    static int16_t sCmdIndex = 0;
    uint8_t lStatus = 0, lChar = 0;

    //lChar = uart_getchar(aPortNum);
    lStatus = bm_uart_getchar(aPortNum, &lChar);
    if( lStatus == 1 )
    {
		if( lChar != aTerminatingChar )
		{
			if(sCmdIndex < aBufSize-1)
				aCmdLineBuf[sCmdIndex++] = lChar;
			else    //Ran out of buffer size, return a truncated message.
			{
				aCmdLineBuf[sCmdIndex] = '\0';
				sCmdIndex = 0;
				return 1;
			}
		}
		else    //rxd a terminating character, return with the formed string.
		{
			aCmdLineBuf[sCmdIndex] = '\0';
			sCmdIndex = 0;
			return 1;
		}
    }
	return 0;       //Keep returning 0, until the string is formed.
}



