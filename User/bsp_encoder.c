#include "bsp_encoder.h"
#include "tim.h"

// 编码器线数（每圈脉冲数*4倍频），每圈脉冲数由电机的编码器决定，需要调整！！
#define ENCODER_PPR (100 * 4)   // FIXME: 这里的100是编码器的脉冲数，需要根据实际编码器调整

/* ========== 静态变量：上次采样时的计数值 ========== */
static int32_t encoder_last_count[4];   // 上一次读到的原始计数
static int32_t encoder_current_count[4]; // 当前累积计数（带溢出处理）
static float   encoder_speed_rpm[4];    // 计算出的转速

/* ========== 更新间隔（秒），在 Encoder_Init 中设置 ========== */
static float encoder_update_period = 0.01f;  // 默认 10ms


// 初始化函数

void Encoder_Init(void)
{
    // 启动 4 路编码器
    HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);  // 编码器 A
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);  // 编码器 B
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);  // 编码器 C
    HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);  // 编码器 D

    // 初始化静态变量
    for (int i = 0; i < 4; i++)
    {
        encoder_last_count[i] = 0;
        encoder_current_count[i] = 0;
        encoder_speed_rpm[i] = 0.0f;
    }
}

/* ==========读取累计脉冲，处理16位溢出 ========== */





