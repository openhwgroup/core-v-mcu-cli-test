/*
 * print_task.c
 *
 *  Created on: Jan 26, 2021
 *      Author: gregmartin
 */

#define __PRINT_TASK_C__

#include "target/core-v-mcu/include/core-v-mcu-config.h"
#include <periph-tasks/include/write_uart_task.h>
#include <string.h>
#include "hal/include/hal_fc_event.h"
#include "hal/include/hal_udma_ctrl_reg_defs.h"

static TaskHandle_t xTasktoNotify[N_UART];
static QueueHandle_t xPrtQueue[N_UART];  // Print queue
void vUartTxBuf(uint8_t uart_id, char *pcString, uint32_t len)
{
	print_t mymessage;
	
	if (len == 1) {
		vUartTxChar(uart_id, *pcString);
	} else {
		configASSERT(mymessage.str = pvPortMalloc(len));
		memcpy (mymessage.str, pcString, len);
		mymessage.len = len;
		xQueueSend(xPrtQueue[uart_id], &mymessage, portMAX_DELAY);
	}
}

void vUartTxChar(uint8_t uart_id, char c)
{
	print_t mymessage;
	
	mymessage.str = (uint8_t*)c;
	mymessage.len = 1;
	xQueueSend(xPrtQueue[uart_id], &mymessage, portMAX_DELAY);
}

uint32_t getline(uart_channel_t *uart, uint8_t *str, uint32_t len) {
	uint32_t ret = 0;
	uint8_t last_char = 0;

	uart = (uart_channel_t*)UDMA_CH_ADDR_UART0;
	while ( (ret < (len-2)) && (last_char != 0xd)) {
		if (uart->valid == 1) {
			last_char = (uint8_t)(uart->data & 0xff);
			if (last_char == 0xd)  // if cr add
				str[ret++] = 0xa;  // linefeed
			str[ret++] = last_char;
		}
	}
	return ret--;
}

uint8_t ucUartCharAvailable(uint8_t uart_id) {
	uart_channel_t* puartchan = (uart_channel_t*)(UDMA_CH_ADDR_UART + (uart_id * UDMA_CH_SIZE));
	return puartchan->valid;
}

int	xUartRxChar(uint8_t uart_id) {
	uart_channel_t* puartchan = (uart_channel_t*)(UDMA_CH_ADDR_UART + (uart_id * UDMA_CH_SIZE));
	return (int)(puartchan->data & 0xff);
}



static void ISR_uart0_handler() {
	BaseType_t *pxHigherPriorityTaskWoken;
	configASSERT(xTasktoNotify[0]);

	vTaskNotifyGiveFromISR(xTasktoNotify[0],
	                       &pxHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( pxHigherPriorityTaskWoken );
}

static void ISR_uart1_handler() {
	BaseType_t *pxHigherPriorityTaskWoken;
	configASSERT(xTasktoNotify[0]);

	vTaskNotifyGiveFromISR(xTasktoNotify[1],
	                       &pxHigherPriorityTaskWoken );
	portYIELD_FROM_ISR( pxHigherPriorityTaskWoken );
}

void prvWriteUart0Task (void *pvParameters)
{
	uart_channel_t*	uart;
	print_t 				str_struct;
	volatile UDMA_CTRL_t*		pudma_ctrl = UDMA_CH_ADDR_CTRL;
	volatile uint32_t *udma_cg = (uint32_t*)UDMA_CH_ADDR_CTRL;
	
	xPrtQueue[0] = xQueueCreate(PrintQueueLength, sizeof(print_t));
	configASSERT(xPrtQueue[0]);
    /* Set handlers. */
    pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_RX(0), ISR_uart0_handler);
    pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_TX(0), ISR_uart0_handler);
    /* Enable SOC events propagation to FC. */
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_RX(0));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_TX(0));

	pudma_ctrl->REG_CG |= UDMA_CTRL_UART0_CLKEN;
	uart = (uart_channel_t*)UDMA_CH_ADDR_UART0;
	uart->setup = (5000000/115200) << 16 | // Baud rate divisor
			(3 << 1) | // 8-bits
			(1 << 4) | // use polling on read
			(1 << 8) | // tx enable
			(1 << 9) ; // rx enable
	xTasktoNotify[0] = xTaskGetCurrentTaskHandle();
	for (;;) {
		xQueueReceive(xPrtQueue[0], &str_struct, portMAX_DELAY);
		if (str_struct.len == 1) {
			uart->tx_saddr = &(str_struct.str);
		} else {
			uart->tx_saddr = str_struct.str;
		}
		uart->tx_size = str_struct.len;
		uart->tx_cfg = 0x10; //enable the transfer
        ulTaskNotifyTake(pdTRUE,          /* Clear the  value before exiting. */
                         portMAX_DELAY );
        if (str_struct.len != 1)
        	vPortFree(str_struct.str);UDMA_CTRL_t*		pudma_ctrl;

	}
}

void prvWriteUart1Task (void *pvParameters)
{
	uart_channel_t *uart;
	print_t str_struct;
	volatile UDMA_CTRL_t*		pudma_ctrl = UDMA_CH_ADDR_CTRL;
	volatile uint32_t *udma_cg = (uint32_t*)UDMA_CH_ADDR_CTRL;


	xPrtQueue[1] = xQueueCreate(PrintQueueLength, sizeof(print_t));
	configASSERT(xPrtQueue[1]);
    /* Set handlers. */
    pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_RX(1), ISR_uart1_handler);
    pi_fc_event_handler_set(SOC_EVENT_UDMA_UART_TX(1), ISR_uart1_handler);
    /* Enable SOC events propagation to FC. */
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_RX(1));
    hal_soc_eu_set_fc_mask(SOC_EVENT_UDMA_UART_TX(1));

    pudma_ctrl->REG_CG |= UDMA_CTRL_UART1_CLKEN;


	uart = (uart_channel_t*)UDMA_CH_ADDR_UART1;
	uart->setup = (5000000/115200) << 16 | // Baud rate divisor
			(3 << 1) | // 8-bits
			(1 << 4) | // use polling on read
			(1 << 8) | // tx enable
			(1 << 9) ; // rx enable
	xTasktoNotify[1] = xTaskGetCurrentTaskHandle();
	for (;;) {
		xQueueReceive(xPrtQueue[1], &str_struct, portMAX_DELAY);
		if (str_struct.len == 1) {
			uart->tx_saddr = &(str_struct.str);
		} else {
			uart->tx_saddr = str_struct.str;
		}
		uart->tx_size = str_struct.len;
		uart->tx_cfg = 0x10; //enable the transfer
        ulTaskNotifyTake(pdTRUE,          /* Clear the  value before exiting. */
                         portMAX_DELAY );
        if (str_struct.len != 1)
        	vPortFree(str_struct.str);

	}
}
