#ifndef __NET_UART_H__
#define __NEt_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// 封装usart接口（因为这个项目里只用到这一个uart，所以无所谓）
typedef enum
{
    /** CubeMX-configured USART6. */
    NET_UART_USART6 = 0,
    /** Number of UART channels. */
    NET_UART_NUM
} NET_UARTId_t;

// 定义uart接收回调指针，接收到Rx数据后传回接受的uart接口/数据指针/数据长度。
typedef void (*NET_UARTRxCallback_t)(NET_UARTId_t uart, uint8_t *data, uint16_t len);

// 定义uart传出回调指针，返回对应的传出uart接口
typedef void (*NET_UARTTxCallback_t)(NET_UARTId_t uart);

// 定义uart报错回调指针，传回报错的uart通道和hal返回的错误码
typedef void (*NET_UARTErrorCallback_t)(NET_UARTId_t uart, uint32_t error_code);

// 声明uart初始化函数
void NET_UART_Init(void);

// 声明uart接收回调函数
void NET_UART_RegisterRxCallback(NET_UARTId_t uart, NET_UARTRxCallback_t callback);

//  声明uart输出回调函数
void NET_UART_RegisterTxCallback(NET_UARTId_t uart, NET_UARTTxCallback_t callback);

//  声明uart回调错误函数
void NET_UART_RegisterErrorCallback(NET_UARTId_t uart, NET_UARTErrorCallback_t callback);

// 声明uart发送函数（polling含阻滞）
HAL_StatusTypeDef NET_UART_Send(NET_UARTId_t uart, const uint8_t *data, uint16_t len, uint32_t timeout_ms);

// 声明uart发送函数（IT不含阻滞）
HAL_StatusTypeDef NET_UART_SendIT(NET_UARTId_t uart, const uint8_t *data, uint16_t len);

// 声明uart发送函数（DMA不含阻滞）
HAL_StatusTypeDef NET_UART_SendDMA(NET_UARTId_t uart, const uint8_t *data, uint16_t len);

// 声明uart接收函数（IT不含阻滞）
HAL_StatusTypeDef NET_UART_ReceiveIT(NET_UARTId_t uart, uint8_t *data, uint16_t len);

// 声明uart接收函数（DMA）
HAL_StatusTypeDef NET_UART_ReceiveDMA(NET_UARTId_t uart, uint8_t *data, uint16_t len);

// 声明uart接收函数（IT➕IDLE自动中断）
HAL_StatusTypeDef NET_UART_ReceiveToIdleIT(NET_UARTId_t uart, uint8_t *data, uint16_t max_len);

// 声明uart接收函数（DMA➕IDLE自动中断）
HAL_StatusTypeDef NET_UART_ReceiveToIdleDMA(NET_UARTId_t uart, uint8_t *data, uint16_t max_len);

// 声明阻塞式的接收中止函数
HAL_StatusTypeDef NET_UART_AbortReceive(NET_UARTId_t uart);

// 声明阻塞式的发送中止函数
HAL_StatusTypeDef NET_UART_AbortTransmit(NET_UARTId_t uart);

#ifdef __cplusplus
}

#endif

#endif