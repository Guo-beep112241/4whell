#include "bsp_timer.h"
#include "stm32f4xx_hal_tim.h"
#include "tim.h"

static volatile uint32_t bsp_timer_1khz_tick = 0u;
static volatile uint32_t bsp_timer_100hz_tick = 0u;
static BSP_Timer1kHzCallback_t bsp_timer_1khz_callback = NULL;
static BSP_Timer100HzCallback_t bsp_timer_100hz_callback = NULL;

// 定义1kHz的回调函数（优先级高，负责4个轮子的pid等实时任务）：调用hal库中的计时器设定函数，指针指向系统时钟tim10
HAL_StatusTypeDef BSP_Timer1kHz_Init(void)
{
    HAL_TIM_Base_Stop_IT(&htim10);
    bsp_timer_1khz_tick = 0u;
    __HAL_TIM_SET_COUNTER(&htim10, 0u);
    __HAL_TIM_CLEAR_FLAG(&htim10, TIM_FLAG_UPDATE);

    return HAL_TIM_Base_Start_IT(&htim10);
}

// 定义100Hz的回调函数（优先级较低，负责通信等非实时任务）
HAL_StatusTypeDef BSP_Timer100Hz_Init(void)
{
    HAL_TIM_Base_Stop_IT(&htim9);
    bsp_timer_100hz_tick = 0u;
    __HAL_TIM_SET_COUNTER(&htim9, 0u);
    __HAL_TIM_CLEAR_FLAG(&htim9, TIM_FLAG_UPDATE);

    return HAL_TIM_Base_Start_IT(&htim9);
}

// 定义1kHz的回调函数
void BSP_Timer1kHz_RegisterCallback(BSP_Timer1kHzCallback_t callback)
{
    bsp_timer_1khz_callback = callback;
}

// 定义100Hz的回调函数
void BSP_Timer100Hz_RegisterCallback(BSP_Timer100HzCallback_t callback)
{
    bsp_timer_100hz_callback = callback;
}

// 定义1kHz的停止回调函数
HAL_StatusTypeDef BSP_Timer1kHz_Stop(void)
{
    return HAL_TIM_Base_Stop_IT(&htim10);
}

// 定义100Hz的停止回调函数
HAL_StatusTypeDef BSP_Timer100Hz_Stop(void)
{
    return HAL_TIM_Base_Stop_IT(&htim9);
}

// 定义外界可访问的读取计时器计数值接口
uint32_t BSP_Timer1kHz_GetTick(void)
{
    return bsp_timer_1khz_tick;
}

uint32_t BSP_Timer100Hz_GetTick(void)
{
    return bsp_timer_100hz_tick;
}

// 计时器实际计数函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM10)
    {
        bsp_timer_1khz_tick++;

        if (bsp_timer_1khz_callback != NULL)
        {
            bsp_timer_1khz_callback();
        }

        return;
    }

    if (htim->Instance == TIM9)
    {
        bsp_timer_100hz_tick++;

        if (bsp_timer_100hz_callback != NULL)
        {
            bsp_timer_100hz_callback();
        }
    }
}