#ifndef __BSP_TIMER_H__
#define __BSP_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


// 设置1kHz和100Hz的指针用于回调。
typedef void (*BSP_Timer1kHzCallback_t)(void);

typedef void (*BSP_Timer100HzCallback_t)(void);


// 启用TImer1kHz计时器
HAL_StatusTypeDef BSP_Timer1kHz_Init(void);

// 启用TImer100Hz计时器
HAL_StatusTypeDef BSP_Timer100Hz_Init(void);

// 声明1kHz回调函数
void BSP_Timer1kHz_RegisterCallback(BSP_Timer1kHzCallback_t callback);

// 声明100Hz回调函数
void BSP_Timer100Hz_RegisterCallback(BSP_Timer100HzCallback_t callback);

// 声明1kHz停止回调函数
HAL_StatusTypeDef BSP_Timer1kHz_Stop(void);

// 声明100Hz停止回调函数
HAL_StatusTypeDef BSP_Timer100Hz_Stop(void);

// 声明可读取计数值函数窗口
uint32_t BSP_Timer1kHz_GetTick(void);

uint32_t BSP_Timer100Hz_GetTick(void);



#ifdef __cplusplus
}
#endif

#endif