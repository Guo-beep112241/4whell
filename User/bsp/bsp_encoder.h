#ifndef __BSP_ENCODER_H__
#define __BSP_ENCODER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


// 把四个电机对应的编码器编号定义为枚举类型，方便后续使用
typedef enum
{
    /** Encoder channel driven by TIM1.负责电机A，RR */
    PWM_ENCODER_CH1 = 0,
    /** Encoder channel driven by TIM2.负责电机B，FR */
    PWM_ENCODER_CH2,
    /** Encoder channel driven by TIM3.负责电机C，FL */
    PWM_ENCODER_CH3,
    /** Encoder channel driven by TIM4.负责电机D，RL */
    PWM_ENCODER_CH4,
    /** Number of encoder channels. */
    PWM_ENCODER_NUM
} PWM_EncoderId_t;


// 定义了一个储存最新编码器状态的结构体
typedef struct
{
    /** 编码器速度，单位是"编码器计数值/秒"。 */
    int32_t speed_cnt_per_s;
    /** 计数差值，正数表示正转，负数表示反转。 */
    int32_t delta_cnt;
    /** 原始定时器计数器值，这是最近一次采样时从 TIM 的 CNT 寄存器直接读出的原始值。 */
    uint32_t counter;
} PWM_EncoderState_t;


// 声明编码器启动
void PWM_EncoderInit(void);

// 声明编码器更新函数，dt_ms是上次调用到本次调用的时间间隔，单位毫秒
void PWM_EncoderUpdate(uint32_t dt_ms);


// 读取指定编码器的速度，单位是"编码器计数值/秒"
int32_t PWM_GetEncoderSpeedCntPerS(PWM_EncoderId_t encoder);

// 读取指定编码器的速度变化值
int32_t PWM_GetEncoderDelta(PWM_EncoderId_t encoder);

// 读取暂存的指定编码器计数值
PWM_EncoderState_t PWM_GetEncoderState(PWM_EncoderId_t encoder);


#ifdef __cplusplus
}
#endif

#endif
