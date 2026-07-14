#include "drv_motor.h"
#include "bsp_encoder.h"
#include "drv_tb6612.h"

// 创建结构体，封装电机转速和编码器信息
typedef struct
{
    /** Low-level BSP encoder channel used by this motor. */
    PWM_EncoderId_t encoder_id;
    /** Encoder counts per output-shaft revolution. */
    float cnt_per_output_rev;
    /** Cached speed values. */
    MotorSpeed_t speed;
} Motor_t;


// 封装每个电机的核心数据表
static Motor_t motor[MOTOR_NUM] = {
    [MOTOR_RR] = {
        PWM_ENCODER_CH1,
        MOTOR_ENCODER_CNT_PER_OUTPUT_REV(MOTOR_RR_ENCODER_RESOLUTION, MOTOR_RR_REDUCTION_RATIO),
        {0, 0.0f, 0, 0}, //速度初始化为0
    },
    [MOTOR_FR] = {
        PWM_ENCODER_CH2,
        MOTOR_ENCODER_CNT_PER_OUTPUT_REV(MOTOR_FR_ENCODER_RESOLUTION, MOTOR_FR_REDUCTION_RATIO),
        {0, 0.0f, 0, 0},
    },
    [MOTOR_FL] = {
        PWM_ENCODER_CH3,
        MOTOR_ENCODER_CNT_PER_OUTPUT_REV(MOTOR_FL_ENCODER_RESOLUTION, MOTOR_FL_REDUCTION_RATIO),
        {0, 0.0f, 0, 0},
    },
    [MOTOR_RL] = {
        PWM_ENCODER_CH4,
        MOTOR_ENCODER_CNT_PER_OUTPUT_REV(MOTOR_RL_ENCODER_RESOLUTION, MOTOR_RL_REDUCTION_RATIO),
        {0, 0.0f, 0, 0},
    },
};

// 五个查表函数
// 1. 查询不同电机驱动对应的计时器通道
// FL → BSP_TB6612_CH_FL → 对应某一路 IN1/IN2/PWM 引脚
static DRV_TB6612Channel_t Motor_GetDriveChannel(MotorId_t motor_id)
{
    static const DRV_TB6612Channel_t channel_map[MOTOR_NUM] = {
        [MOTOR_FL] = DRV_TB6612_CH_FL,
        [MOTOR_FR] = DRV_TB6612_CH_FR,
        [MOTOR_RL] = DRV_TB6612_CH_RL,
        [MOTOR_RR] = DRV_TB6612_CH_RR,
    };

    return channel_map[motor_id];
}


// 2. 查询驱动设置的电机状态，将电机与驱动的方向关联
static DRV_TB6612Direction_t Motor_GetDriveDirection(MotorOutputMode_t mode)
{
    switch (mode)
    {
        case MOTOR_OUTPUT_FORWARD:
            return DRV_TB6612_DIR_FORWARD;

        case MOTOR_OUTPUT_REVERSE:
            return DRV_TB6612_DIR_REVERSE;

        case MOTOR_OUTPUT_BRAKE:
            return DRV_TB6612_DIR_BRAKE;

        case MOTOR_OUTPUT_COAST:
        default:
            return DRV_TB6612_DIR_STOP;
    }
}


// 3. 查询调试时设置的正反方向，并在后续调用后判断是否需要操控电机执行反转。
static uint8_t Motor_IsOutputReversed(MotorId_t motor_id)
{
    static const uint8_t reverse_map[MOTOR_NUM] = {
        [MOTOR_FL] = MOTOR_FL_OUTPUT_REVERSE,
        [MOTOR_FR] = MOTOR_FR_OUTPUT_REVERSE,
        [MOTOR_RL] = MOTOR_RL_OUTPUT_REVERSE,
        [MOTOR_RR] = MOTOR_RR_OUTPUT_REVERSE,
    };

    return reverse_map[motor_id] != 0u;
}

// 4. 查询编码器是否需要反向
static uint8_t Motor_IsEncoderReversed(MotorId_t motor_id)
{
    static const uint8_t reverse_map[MOTOR_NUM] = {
        [MOTOR_FL] = MOTOR_FL_ENCODER_REVERSE,
        [MOTOR_FR] = MOTOR_FR_ENCODER_REVERSE,
        [MOTOR_RL] = MOTOR_RL_ENCODER_REVERSE,
        [MOTOR_RR] = MOTOR_RR_ENCODER_REVERSE,
    };

    return reverse_map[motor_id] != 0u;
}


// 5. 执行输出方向修正
static MotorOutputMode_t Motor_ApplyOutputReverse(MotorId_t motor_id, MotorOutputMode_t mode)
{
    if (!Motor_IsOutputReversed(motor_id))
    {
        return mode;
    }

    if (mode == MOTOR_OUTPUT_FORWARD)
    {
        return MOTOR_OUTPUT_REVERSE;
    }

    if (mode == MOTOR_OUTPUT_REVERSE)
    {
        return MOTOR_OUTPUT_FORWARD;
    }

    return mode;
}

// 封装RPM计算公式
static float Motor_CalcRPM(int32_t speed_cnt_per_s, float cnt_per_output_rev)
{
    if (cnt_per_output_rev <= 0.0f)
    {
        return 0.0f;
    }

    return ((float)speed_cnt_per_s * 60.0f) / cnt_per_output_rev;
}

// 电机启动函数定义
// 封装编码器和驱动，保证两者同时开始，并用for循环把电机的初速度归零
void Motor_Init(void)
{
    PWM_EncoderInit();
    DRV_TB6612_Init();

    for (uint8_t i = 0; i < MOTOR_NUM; i++)
    {
        motor[i].speed.speed_cnt_per_s = 0;
        motor[i].speed.speed_rpm = 0.0f;
        motor[i].speed.delta_cnt = 0;
        motor[i].speed.counter = 0;
    }
}

// 读取所有电机指令，自查指令的可行性，若可行，返回电机占空比。
HAL_StatusTypeDef Motor_SetOutput(MotorId_t motor_id, MotorOutputMode_t mode, uint16_t duty)
{
    DRV_TB6612Channel_t channel;
    DRV_TB6612Direction_t direction;

    if (motor_id >= MOTOR_NUM)
    {
        return HAL_ERROR;
    }

    channel = Motor_GetDriveChannel(motor_id);
    mode = Motor_ApplyOutputReverse(motor_id, mode);
    direction = Motor_GetDriveDirection(mode);

    if (mode == MOTOR_OUTPUT_COAST)
    {
        if (DRV_TB6612_SetDuty(channel, 0u) != HAL_OK)
        {
            return HAL_ERROR;
        }

        return DRV_TB6612_SetDirection(channel, direction);
    }

    if (mode == MOTOR_OUTPUT_BRAKE)
    {
        if (DRV_TB6612_SetDirection(channel, direction) != HAL_OK)
        {
            return HAL_ERROR;
        }

        return DRV_TB6612_SetDuty(channel, DRV_TB6612_GetMaxDuty());
    }

    if (DRV_TB6612_SetDirection(channel, direction) != HAL_OK)
    {
        return HAL_ERROR;
    }

    return DRV_TB6612_SetDuty(channel, duty);
}


// 定义可外部调用更改的——设置电机占空比的函数
HAL_StatusTypeDef Motor_SetOutputPercent(MotorId_t motor_id, float speed_percent)
{
    MotorOutputMode_t mode = MOTOR_OUTPUT_FORWARD;
    uint16_t max_duty = DRV_TB6612_GetMaxDuty();
    uint16_t duty;

    if (speed_percent < 0.0f)
    {
        speed_percent = -speed_percent;
        mode = MOTOR_OUTPUT_REVERSE;
    }

    if (speed_percent > 100.0f)
    {
        speed_percent = 100.0f;
    }

    if (speed_percent == 0.0f)
    {
        return Motor_Stop(motor_id);
    }

    duty = (uint16_t)(((float)max_duty * speed_percent) / 100.0f);

    return Motor_SetOutput(motor_id, mode, duty);
}


// 定义单个电机滑行的函数
HAL_StatusTypeDef Motor_Stop(MotorId_t motor_id)
{
    return Motor_SetOutput(motor_id, MOTOR_OUTPUT_COAST, 0u);
}

// 定义单个电机锁死的函数（停止）
HAL_StatusTypeDef Motor_Brake(MotorId_t motor_id)
{
    return Motor_SetOutput(motor_id, MOTOR_OUTPUT_BRAKE, DRV_TB6612_GetMaxDuty());
}

// 定义多电机停止的函数
HAL_StatusTypeDef Motor_StopAll(void)
{
    return DRV_TB6612_StopAll();
}

// 定义电机更新速度的函数
void Motor_UpdateSpeed(uint32_t dt_ms)
{
    if (dt_ms == 0u)
    {
        return;
    }

    PWM_EncoderUpdate(dt_ms);

    for (uint8_t i = 0; i < MOTOR_NUM; i++)
    {
        PWM_EncoderState_t encoder_state = PWM_GetEncoderState(motor[i].encoder_id);
        int32_t speed_cnt_per_s = encoder_state.speed_cnt_per_s;
        int32_t delta_cnt = encoder_state.delta_cnt;

        if (Motor_IsEncoderReversed((MotorId_t)i))
        {
            speed_cnt_per_s = -speed_cnt_per_s;
            delta_cnt = -delta_cnt;
        }

        motor[i].speed.speed_cnt_per_s = speed_cnt_per_s;
        motor[i].speed.speed_rpm = Motor_CalcRPM(speed_cnt_per_s, motor[i].cnt_per_output_rev);
        motor[i].speed.delta_cnt = delta_cnt;
        motor[i].speed.counter = encoder_state.counter;
    }
}


// 三个外界可访问的电机速度相关函数
// 读取电机此时输出轴转速rpm（转/min）
float Motor_GetSpeedRPM(MotorId_t motor_id)
{
    if (motor_id >= MOTOR_NUM)
    {
        return 0.0f;
    }

    return motor[motor_id].speed.speed_rpm;
}

// 读取电机此时输出轴脉冲速率（脉冲/s）
int32_t Motor_GetSpeedCntPerS(MotorId_t motor_id)
{
    if (motor_id >= MOTOR_NUM)
    {
        return 0;
    }

    return motor[motor_id].speed.speed_cnt_per_s;
}

// 读取电机此时速度相关所有值
MotorSpeed_t Motor_GetSpeedState(MotorId_t motor_id)
{
    MotorSpeed_t state = {0, 0.0f, 0, 0};

    if (motor_id >= MOTOR_NUM)
    {
        return state;
    }

    return motor[motor_id].speed;
}