#ifndef __PRINT_TASK_H__
#define __PRINT_TASK_H__

#include <FreeRTOS.h>
#include <queue.h>

#ifdef __PRINT_TASK_C__
#define EXTERN
#else
#define EXTERN extern
#endif

typedef struct {
	volatile uint8_t *rx_saddr;
	volatile uint32_t rx_size;
	volatile uint32_t rx_cfg;
	volatile uint32_t unused1;
	volatile uint8_t *tx_saddr;
	volatile uint32_t tx_size;
	volatile uint32_t tx_cfg;
	volatile uint32_t unused2;
	volatile uint32_t status;
	volatile uint32_t setup;
	volatile uint32_t error;
	volatile uint32_t irq_en;
	volatile uint32_t valid;
	volatile uint32_t data;
} uart_channel_t;

#define PrintQueueLength	(256)
typedef struct {
	uint8_t *str;
	uint32_t len;
} print_t;

void 	vUartTxBuf(uint8_t uart_id, char *pcBuf, uint32_t len);
void 	vUartTxChar(uint8_t uart_id, char c);

uint8_t ucUartCharAvailable(uint8_t uart_id);
int		xUartRxChar(uint8_t uart_id);

void prvWriteUart0Task (void *pvParameters);
void prvWriteUart1Task (void *pvParameters);


#endif
