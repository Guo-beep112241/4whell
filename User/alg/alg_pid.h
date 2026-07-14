#ifndef __ALG_PID_H__
#define __ALG_PID_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"


// 创建结构体PID_t,封装pid相关量
typedef struct
{
    float kp;
    float ki;
    float kd;

    float output_min;
    float output_max;
    float integral_min;
    float integral_max;

    float setpoint;
    float feedback;
    float error;
    float last_error;
    float integral;
    float derivative;
    float output;
} PID_t;

// 初始化pid
void PID_Init(PID_t *pid, float kp, float ki, float kd);

// pid参数设置
void PID_SetParam(PID_t *pid, float kp, float ki, float kd);

// pid输出边界设置
void PID_SetOutputLimit(PID_t *pid, float output_min, float output_max);

// pid积分边界设置
void PID_SetIntegralLimit(PID_t *pid, float integral_min, float integral_max);

// 重启pid，除3个参数外，其他东西归零。
void PID_Reset(PID_t *pid);


float PID_Calc(PID_t *pid, float setpoint, float feedback, float dt_s);

#ifdef __cplusplus
}
#endif

#endif
