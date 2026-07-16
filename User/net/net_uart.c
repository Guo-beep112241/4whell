#include "net_uart.h"

#include "usart.h"

#include <stddef.h>


// 定义并封装uart相关量
typedef struct
{
    UART_HandleTypeDef *handle;
    uint8_t *rx_buffer;
    uint16_t rx_len;
    NET_UARTRxCallback_t rx_callback;
    NET_UARTTxCallback_t tx_callback;
    NET_UARTErrorCallback_t error_callback;
} NET_UART_t;

// 初始化uart相关量
static NET_UART_t net_uart[NET_UART_NUM] = {
    [NET_UART_USART6] = {
        &huart6,
        NULL,
        0u,
        NULL,
        NULL,
        NULL,
    },
};


// 内部检验uart是否存在
static uint8_t NET_UART_IsValid(NET_UARTId_t uart)
{
    return (uart < NET_UART_NUM) && (net_uart[uart].handle != NULL);
}

// 读取hal库传回的huart指针地址，选择与这个地址对应的uart通道，并返回这个通道的序号。
// 在这个项目中，由于只用到了1个uart通道，所以有没有这个判断其实也无所谓。
// 但对于使用了多通道通讯的项目，确认接收到的信息是针对哪个uart的很重要！
static NET_UARTId_t NET_UART_GetIdFromHandle(UART_HandleTypeDef *huart)
{
    for (uint8_t i = 0u; i < NET_UART_NUM; i++)
    {
        if (net_uart[i].handle == huart)
        {
            return (NET_UARTId_t)i;
        }
    }

    return NET_UART_NUM;
}

// 初始化uart的函数。
void NET_UART_Init(void)
{
    for (uint8_t i = 0u; i < NET_UART_NUM; i++)
    {
        net_uart[i].rx_buffer = NULL;
        net_uart[i].rx_len = 0u;
        net_uart[i].rx_callback = NULL;
        net_uart[i].tx_callback = NULL;
        net_uart[i].error_callback = NULL;
    }
}


// uart的RX回调函数（含合理性判断，相当于一个保护）
void NET_UART_RegisterRxCallback(NET_UARTId_t uart, NET_UARTRxCallback_t callback)
{
    if (!NET_UART_IsValid(uart))
    {
        return;
    }

    net_uart[uart].rx_callback = callback;
}

// uart的TX回调函数（含合理性判断，相当于一个保护）
void NET_UART_RegisterTxCallback(NET_UARTId_t uart, NET_UARTTxCallback_t callback)
{
    if (!NET_UART_IsValid(uart))
    {
        return;
    }

    net_uart[uart].tx_callback = callback;
}

// uart传出函数，含阻滞（确认发送的信息非空，且负责发送的uart接口）
HAL_StatusTypeDef NET_UART_Send(NET_UARTId_t uart, const uint8_t *data, uint16_t len, uint32_t timeout_ms)
{
    if ((!NET_UART_IsValid(uart)) || (data == NULL) || (len == 0u))
    {
        return HAL_ERROR;
    }

    return HAL_UART_Transmit(net_uart[uart].handle, data, len, timeout_ms); //逐字节发送完毕才返回，最多等待时间=timeout_ms
}

// IT版传输方式，cpu参与，没有阻滞。
// uart传出函数，不含阻滞，不能在中断里调用（确认发送的信息非空，且负责发送的uart接口）
HAL_StatusTypeDef NET_UART_SendIT(NET_UARTId_t uart, const uint8_t *data, uint16_t len)
{
    if ((!NET_UART_IsValid(uart)) || (data == NULL) || (len == 0u))
    {
        return HAL_ERROR;
    }

    return HAL_UART_Transmit_IT(net_uart[uart].handle, data, len); //启动发送后立即返回，后台中断逐字节发送
}


// DMA硬件自动传输数据，cpu只在传输开始和结束的时候参与。节约cpu开销。
// uart传出函数，用DMA寄存，不含阻滞，不能在中断里调用（确认发送的信息非空，且负责发送的uart接口）
HAL_StatusTypeDef NET_UART_SendDMA(NET_UARTId_t uart, const uint8_t *data, uint16_t len)
{
    if ((!NET_UART_IsValid(uart)) || (data == NULL) || (len == 0u))
    {
        return HAL_ERROR;
    }

    return HAL_UART_Transmit_DMA(net_uart[uart].handle, data, len);
}

// IT版接收数据方式
// 使用了IDLE空闲中断，对方发过来多少就接受多少，通过uart线路空闲时间来做接收中断
HAL_StatusTypeDef NET_UART_ReceiveToIdleIT(NET_UARTId_t uart, uint8_t *data, uint16_t max_len)
{
    if ((!NET_UART_IsValid(uart)) || (data == NULL) || (max_len == 0u))
    {
        return HAL_ERROR;
    }

    net_uart[uart].rx_buffer = data; //data是一个空指针，指向的是一块空缓存区，用来存储接收到的数据。
    net_uart[uart].rx_len = max_len; //max_len是调用这个函数时根据需求写入的，为定值。指的是用户希望接收到的信息的长度上限。

    return HAL_UARTEx_ReceiveToIdle_IT(net_uart[uart].handle, data, max_len); //STM32 HAL 库提供的增强型串口接收函数，用中断方式接收不定长数据。

}

// DMA版接收数据方式
// 使用了IDLE空闲中断，对方发过来多少就接受多少，通过uart线路空闲时间来做接收中断
HAL_StatusTypeDef NET_UART_ReceiveToIdleDMA(NET_UARTId_t uart, uint8_t *data, uint16_t max_len)
{
    HAL_StatusTypeDef status;

    if ((!NET_UART_IsValid(uart)) || (data == NULL) || (max_len == 0u))
    {
        return HAL_ERROR;
    }

    net_uart[uart].rx_buffer = data;
    net_uart[uart].rx_len = max_len;

    status = HAL_UARTEx_ReceiveToIdle_DMA(net_uart[uart].handle, data, max_len);

    if ((status == HAL_OK) && (net_uart[uart].handle->hdmarx != NULL))
    {
        __HAL_DMA_DISABLE_IT(net_uart[uart].handle->hdmarx, DMA_IT_HT);
    }

    return status;
}


// 阻塞式的接收中止函数，用于强制停止正在进行的 UART 接收（无论是中断模式还是 DMA 模式）。
HAL_StatusTypeDef NET_UART_AbortReceive(NET_UARTId_t uart)
{
    if (!NET_UART_IsValid(uart))
    {
        return HAL_ERROR;
    }

    net_uart[uart].rx_buffer = NULL;
    net_uart[uart].rx_len = 0u;

    return HAL_UART_AbortReceive(net_uart[uart].handle);
}

// 阻塞式的传输中止函数，用于强制停止正在进行的 UART 传出（无论是中断模式还是 DMA 模式）。
HAL_StatusTypeDef NET_UART_AbortTransmit(NET_UARTId_t uart)
{
    if (!NET_UART_IsValid(uart))
    {
        return HAL_ERROR;
    }

    return HAL_UART_AbortTransmit(net_uart[uart].handle);
}

// 根据实际需求，重新定义hal库的接收完成回调函数。
// 由于hal_uart.h里定义的是弱函数，所以我自己再定义的话可以直接覆盖那个版本，不用担心重复定义。
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    NET_UARTId_t uart = NET_UART_GetIdFromHandle(huart);

    if (!NET_UART_IsValid(uart))
    {
        return;
    }

    if (net_uart[uart].rx_callback != NULL)
    {
        net_uart[uart].rx_callback(uart, net_uart[uart].rx_buffer, net_uart[uart].rx_len); //回调时把接收到的buffer和len一起存在对应uart串口的回调参数里。
    }
}

// hal库的增强回调函数。不仅传入了句柄地址，还穿入了接收到的信息的size。
// 意味着这种回调方法适配应用了IDLE中断接收到的信息。信息的长度由固定长度max_len变为可变长度。
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    NET_UARTId_t uart = NET_UART_GetIdFromHandle(huart);

    if (!NET_UART_IsValid(uart))
    {
        return;
    }

    if (net_uart[uart].rx_callback != NULL)
    {
        net_uart[uart].rx_callback(uart, net_uart[uart].rx_buffer, size);
    }
}

// hal库的接收完成回调函数。（非增强版）
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    NET_UARTId_t uart = NET_UART_GetIdFromHandle(huart);

    if (!NET_UART_IsValid(uart))
    {
        return;
    }

    if (net_uart[uart].tx_callback != NULL)
    {
        net_uart[uart].tx_callback(uart);
    }
}

// hal库的错误回调函数。句柄指向UART Error code。
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    NET_UARTId_t uart = NET_UART_GetIdFromHandle(huart);

    if (!NET_UART_IsValid(uart))
    {
        return;
    }

    if (net_uart[uart].error_callback != NULL)
    {
        net_uart[uart].error_callback(uart, huart->ErrorCode);
    }
}
