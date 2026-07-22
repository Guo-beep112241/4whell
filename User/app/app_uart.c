/**
 * @file app_uart.c
 * @brief 通过 UART6 接收指令，点亮/熄灭/翻转 C 板 LED（PC13），验证通信硬件。
 *
 * 支持的指令（以 \\r\\n 结尾，大小写敏感）：
 *   "ON"      → 点亮 LED
 *   "OFF"     → 熄灭 LED
 *   "TOG"     → 翻转 LED
 *
 * 例如用串口助手发送 "ON\r\n"，LED 点亮并返回 "LED ON\r\n"。
 */

#include "app_uart.h"
#include "bsp_led.h"
#include "net_uart.h"

#include <string.h>

/* -------------------------------------------------------------------------- */
/* 宏定义                                                                    */
/* -------------------------------------------------------------------------- */

/** 接收缓冲区大小（字节） */
#define APP_UART_RX_BUF_SIZE  64U

/* -------------------------------------------------------------------------- */
/* 静态变量                                                                  */
/* -------------------------------------------------------------------------- */

/** UART 接收缓冲区 */
static uint8_t rx_buf[APP_UART_RX_BUF_SIZE];

/* -------------------------------------------------------------------------- */
/* 内部函数                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief 去掉字符串末尾的 \\r 和 \\n。
 */
static void APP_UART_TrimNewline(char *str, uint16_t *len)
{
    while (*len > 0U && (str[*len - 1U] == '\r' || str[*len - 1U] == '\n'))
    {
        str[--(*len)] = '\0';
    }
}

/**
 * @brief UART 接收回调（在中断上下文中执行，请保持简短）。
 *
 * 根据收到的指令操作 LED，并回传确认信息。
 */
static void APP_UART_RxCallback(NET_UARTId_t uart, uint8_t *data, uint16_t len)
{
    /* 防止越界 */
    if (len >= APP_UART_RX_BUF_SIZE)
    {
        len = APP_UART_RX_BUF_SIZE - 1U;
    }
    data[len] = '\0';

    /* 去掉换行符 */
    APP_UART_TrimNewline((char *)data, &len);

    /* ---------- 指令解析 ---------- */
    if (strcmp((const char *)data, "ON") == 0)
    {
        BSP_LED_On();
        NET_UART_Send(uart, (const uint8_t *)"LED ON\r\n", 8U, 50U);
    }
    else if (strcmp((const char *)data, "OFF") == 0)
    {
        BSP_LED_Off();
        NET_UART_Send(uart, (const uint8_t *)"LED OFF\r\n", 9U, 50U);
    }
    else if (strcmp((const char *)data, "TOG") == 0)
    {
        BSP_LED_Toggle();
        NET_UART_Send(uart, (const uint8_t *)"LED TOGGLE\r\n", 13U, 50U);
    }
    else
    {
        /* 未知指令 — 提示用法 */
        NET_UART_Send(uart,
                      (const uint8_t *)"CMD: ON | OFF | TOG\r\n",
                      22U, 50U);
    }

    /* 重新使能接收（因为 IDLE 中断接收是一次性的） */
    NET_UART_ReceiveToIdleIT(uart, rx_buf, APP_UART_RX_BUF_SIZE);
}

/* -------------------------------------------------------------------------- */
/* 公共函数                                                                  */
/* -------------------------------------------------------------------------- */

/**
 * @brief 初始化应用层 UART 功能。
 */
void APP_UART_Init(void)
{
    /* 注册接收回调 */
    NET_UART_RegisterRxCallback(NET_UART_USART6, APP_UART_RxCallback);

    /* 启动第一次 IDLE 中断接收 */
    NET_UART_ReceiveToIdleIT(NET_UART_USART6, rx_buf, APP_UART_RX_BUF_SIZE);
}
