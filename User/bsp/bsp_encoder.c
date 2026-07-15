#include "bsp_encoder.h"

#include "tim.h"

// 内部定义一个结构体Encoder_t,用于存储每个编码器的自身状态，对比.h中定义的结构体PWM_EncoderState_t更详细，且包含tim指针，内外两套定义避免外部强制更改编码器状态。
typedef struct
{
    /** HAL timer handle configured in encoder mode. */
    TIM_HandleTypeDef *htim;
    /** Timer counter maximum value, used to distinguish 16-bit and 32-bit counters. */
    uint32_t max_cnt;
    /** Previous raw timer counter value. */
    uint32_t last_cnt;
    /** Latest signed counter delta. */
    int32_t delta_cnt;
    /** Latest speed in encoder timer counts per second. */
    int32_t speed_cnt_per_s;
} Encoder_t;


// 定义一个静态数组，存储四个编码器的状态，使用初始化列表指定每个编码器的定时器句柄和最大计数值。
static Encoder_t encoder[PWM_ENCODER_NUM] = {
    [PWM_ENCODER_CH1] = {&htim1, 0x0000FFFFu, 0u, 0, 0},
    [PWM_ENCODER_CH2] = {&htim2, 0xFFFFFFFFu, 0u, 0, 0},
    [PWM_ENCODER_CH3] = {&htim3, 0x0000FFFFu, 0u, 0, 0},
    [PWM_ENCODER_CH4] = {&htim4, 0x0000FFFFu, 0u, 0, 0},
};




// 处理16位计数器溢出问题，32位足够大所以直接减即可
static int32_t Encoder_GetDelta(uint32_t now_cnt, uint32_t last_cnt, uint32_t max_cnt)
{
    if (max_cnt == 0x0000FFFFu)
    {
        return (int32_t)(int16_t)((uint16_t)(now_cnt - last_cnt));
    }

    return (int32_t)(now_cnt - last_cnt);
}

// 限幅保护，防止32转16时意外溢出，导致上位机无法判断。
/** @brief Maximum positive speed value returned by Encoder_CalcSpeed(). */
#define ENCODER_SPEED_MAX 2147483647LL
/** @brief Minimum negative speed value returned by Encoder_CalcSpeed(). */
#define ENCODER_SPEED_MIN (-2147483647LL - 1LL)

// 把编码器的脉冲增量换算成速度（单位：计数值/秒），并加上溢出保护。
static int32_t Encoder_CalcSpeed(int32_t delta_cnt, uint32_t dt_s)
{
    // 计算速度 = 计数差值 / 时间间隔，单位是"编码器计数值/秒",(int64_t)强行把delta_cnt从16制转成32，防止乘1000后溢出，*1000是为了把毫秒转换为秒
    int64_t speed = ((int64_t)delta_cnt ) / (int64_t)dt_s; 

    if (speed > ENCODER_SPEED_MAX)
    {
        return (int32_t)ENCODER_SPEED_MAX;
    }

    if (speed < ENCODER_SPEED_MIN)
    {
        return (int32_t)ENCODER_SPEED_MIN;
    }

    return (int32_t)speed;
}


// 编码器启动函数
void PWM_EncoderInit(void)
{
    for (uint8_t i = 0; i < PWM_ENCODER_NUM; i++)
    {
        HAL_TIM_Encoder_Start(encoder[i].htim, TIM_CHANNEL_ALL);
        encoder[i].last_cnt = __HAL_TIM_GET_COUNTER(encoder[i].htim);
        encoder[i].delta_cnt = 0;
        encoder[i].speed_cnt_per_s = 0;
    }
}

// 编码器更新函数
void PWM_EncoderUpdate(uint32_t dt_ms)
{
    if (dt_ms == 0u)
    {
        return;
    }

    for (uint8_t i = 0; i < PWM_ENCODER_NUM; i++)
    {
        uint32_t now_cnt = __HAL_TIM_GET_COUNTER(encoder[i].htim);

        encoder[i].delta_cnt = Encoder_GetDelta(now_cnt, encoder[i].last_cnt, encoder[i].max_cnt);
        encoder[i].last_cnt = now_cnt;
        encoder[i].speed_cnt_per_s = Encoder_CalcSpeed(encoder[i].delta_cnt, dt_ms);
    }
}


// 定义函数以读取指定编码器的速度，单位是"编码器计数值/秒"
int32_t PWM_GetEncoderSpeedCntPerS(PWM_EncoderId_t encoder_id)
{
    if (encoder_id >= PWM_ENCODER_NUM)
    {
        return 0;
    }

    return encoder[encoder_id].speed_cnt_per_s;
}

// 定义函数以读取指定编码器的速度变化值
int32_t PWM_GetEncoderDelta(PWM_EncoderId_t encoder_id)
{
    if (encoder_id >= PWM_ENCODER_NUM)
    {
        return 0;
    }

    return encoder[encoder_id].delta_cnt;
}

// 定义函数以读取暂存的指定编码器计数值
PWM_EncoderState_t PWM_GetEncoderState(PWM_EncoderId_t encoder_id)
{
    PWM_EncoderState_t state = {0, 0, 0};

    if (encoder_id >= PWM_ENCODER_NUM)
    {
        return state;
    }

    state.speed_cnt_per_s = encoder[encoder_id].speed_cnt_per_s;
    state.delta_cnt = encoder[encoder_id].delta_cnt;
    state.counter = encoder[encoder_id].last_cnt;

    return state;
}
