/**
 * @file app_uart.h
 * @brief UART6 指令控制 LED —— 用于验证 UART 通信硬件配置
 */

#ifndef __APP_UART_H__
#define __APP_UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief 初始化 UART 接收，注册回调，开始监听指令。
 * @note  调用前需确保 MX_USART6_UART_Init() 和 NET_UART_Init() 已完成。
 */
void APP_UART_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_UART_H__ */
