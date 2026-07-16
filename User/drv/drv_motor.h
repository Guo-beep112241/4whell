// 宏定义前进行一个宏定义检测，防止重复宏定义
#ifndef __DRV_MOTOR_H__
#define __DRV_MOTOR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "app_tuner.h"
// 电机编码器的正交倍频系数
#define MOTOR_ENCODER_QUADRATURE_MULTIPLE 4u

// 电机编码器的线数/分辨率（每转脉冲数），单位：脉冲/转
#define MOTOR_FL_ENCODER_RESOLUTION 11u
#define MOTOR_FR_ENCODER_RESOLUTION 11u
#define MOTOR_RL_ENCODER_RESOLUTION 11u
#define MOTOR_RR_ENCODER_RESOLUTION 11u

// 电机的减速比（电机转速/输出轴转速），单位：无量纲
#define MOTOR_FL_REDUCTION_RATIO 21.3f
#define MOTOR_FR_REDUCTION_RATIO 21.3f
#define MOTOR_RL_REDUCTION_RATIO 21.3f
#define MOTOR_RR_REDUCTION_RATIO 21.3f


// 宏定义
// #define MOTOR_FL_OUTPUT_REVERSE
// #define MOTOR_FR_OUTPUT_REVERSE
// #define MOTOR_RL_OUTPUT_REVERSE
// #define MOTOR_RR_OUTPUT_REVERSE

// #define MOTOR_FL_ENCODER_REVERSE
// #define MOTOR_FR_ENCODER_REVERSE
// #define MOTOR_RL_ENCODER_REVERSE
// #define MOTOR_RR_ENCODER_REVERSE


// 这个宏的作用：算"输出轴转一圈 = 多少个编码器脉冲"
// 电机轴转 21.3 圈 ──减速箱──→ 输出轴转 1 圈
                                // │
// 编码器产生脉冲：11线×4倍频×21.3圈 = 937.2 个脉冲
#define MOTOR_ENCODER_CNT_PER_OUTPUT_REV(encoder_resolution, reduction_ratio) \
    ((float)(encoder_resolution) * MOTOR_ENCODER_QUADRATURE_MULTIPLE * (reduction_ratio))


// 结构体定义
// 封装4个电机
typedef enum
{
    /** Front-left motor. */
    MOTOR_FL = 0,
    /** Front-right motor. */
    MOTOR_FR,
    /** Rear-left motor. */
    MOTOR_RL,
    /** Rear-right motor. */
    MOTOR_RR,
    /** Number of motors. */
    MOTOR_NUM
} MotorId_t;
 
// 封装每个电机的4种输出状态
typedef enum
{
    /** IN1=0, IN2=0, PWM=0.滑行 */
    MOTOR_OUTPUT_COAST = 0,
    /** IN1=1, IN2=0. */
    MOTOR_OUTPUT_FORWARD,
    /** IN1=0, IN2=1. */
    MOTOR_OUTPUT_REVERSE,
    /** IN1=1, IN2=1, PWM=max. 刹车*/
    MOTOR_OUTPUT_BRAKE
} MotorOutputMode_t;


// 封装电机的速度状态
typedef struct
{
    /** Latest speed in encoder counts per second. */
    int32_t speed_cnt_per_s;
    /** Latest output-shaft speed in rpm. */
    float speed_rpm;
    /** Encoder count difference during the latest update period. */
    int32_t delta_cnt;
    /** Raw timer counter value sampled during the latest update. */
    uint32_t counter;
} MotorSpeed_t;

// 电机初始化
void Motor_Init(void);

// 声明用于设置电机占空比的函数，在Motor_SetOutputPercent中调用，其参数均由Motor_SetOutputPercent函数内部提供
HAL_StatusTypeDef Motor_SetOutput(MotorId_t motor, MotorOutputMode_t mode, uint16_t duty);

// 声明实际执行时设置电机占空比的函数
HAL_StatusTypeDef Motor_SetOutputPercent(MotorId_t motor, float speed_percent);

// 声明电机滑行
HAL_StatusTypeDef Motor_Stop(MotorId_t motor);

// 声明电机锁死
HAL_StatusTypeDef Motor_Brake(MotorId_t motor);

// 声明全车失能
HAL_StatusTypeDef Motor_StopAll(void);

// 声明电机速度更新
void Motor_UpdateSpeed(uint32_t dt_ms);

//声明三个读取电机速度值的函数
float Motor_GetSpeedRPM(MotorId_t motor);

int32_t Motor_GetSpeedCntPerS(MotorId_t motor);

MotorSpeed_t Motor_GetSpeedState(MotorId_t motor);

#ifdef __cplusplus
}
#endif

#endif