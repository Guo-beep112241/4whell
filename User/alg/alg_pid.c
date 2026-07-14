#include "alg_pid.h"
#include <stddef.h>  //include标准库的原因是后面用了NULL

// 创建一个结构体，用于设置上限和下限。（所有超出部分都以边界值代替）
static float PID_Clamp(float value, float min_value, float max_value)
{
    if (value > max_value)
    {
        return max_value;
    }

    if (value < min_value)
    {
        return min_value;
    }

    return value;
}

// 定义pid的初始化函数
void PID_Init(PID_t *pid, float kp, float ki, float kd)
{
    if (pid == NULL)
    {
        return;
    }

    pid->kp = kp;  // 比例增益 (Proportional)
    pid->ki = ki;  // 积分增益 (Integral)
    pid->kd = kd;  // 微分增益 (Derivative)

    pid->output_min = -100.0f; //输出下限
    pid->output_max = 100.0f;  //输出上限
    pid->integral_min = -100.0f;  //积分下限
    pid->integral_max = 100.0f;   //积分上限

    PID_Reset(pid);
}

// 定义pid赋值函数
void PID_SetParam(PID_t *pid, float kp, float ki, float kd)
{
    if (pid == NULL)
    {
        return;
    }

    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
}

// 定义pid限制输出的函数，可以通过传入的参数改变pid的输出边界。
void PID_SetOutputLimit(PID_t *pid, float output_min, float output_max)
{
    if ((pid == NULL) || (output_min > output_max))
    {
        return;
    }

    pid->output_min = output_min;
    pid->output_max = output_max;
    pid->output = PID_Clamp(pid->output, output_min, output_max);
}


// 定义pid限制积分的函数，可以通过传入的参数改变pid的积分边界。
void PID_SetIntegralLimit(PID_t *pid, float integral_min, float integral_max)
{
    if ((pid == NULL) || (integral_min > integral_max))
    {
        return;
    }

    pid->integral_min = integral_min;
    pid->integral_max = integral_max;
    pid->integral = PID_Clamp(pid->integral, integral_min, integral_max);
}

// 定义pid重启的函数，把过程性参数归零。
void PID_Reset(PID_t *pid)
{
    if (pid == NULL)
    {
        return;
    }

    pid->setpoint = 0.0f;
    pid->feedback = 0.0f;
    pid->error = 0.0f;
    pid->last_error = 0.0f;
    pid->integral = 0.0f;
    pid->derivative = 0.0f;
    pid->output = 0.0f;
}


// 定义pid的核心算法函数。
float PID_Calc(PID_t *pid, float setpoint, float feedback, float dt_s)
{
    float output;

    if ((pid == NULL) || (dt_s <= 0.0f))
    {
        return 0.0f;
    }

    pid->setpoint = setpoint;  //保存目标值
    pid->feedback = feedback;  //保存反馈值
    pid->error = setpoint - feedback;  //计算误差

    pid->integral += pid->error * dt_s;  //矩形近似积分。累加误差值和时间变化量的乘积
    pid->integral = PID_Clamp(pid->integral, pid->integral_min, pid->integral_max);  //用PID_Clamp函数保证积分在边界值内，防止过冲。

    pid->derivative = (pid->error - pid->last_error) / dt_s;  //求出当前时间点的近似误差导数

    output = pid->kp * pid->error + pid->ki * pid->integral + pid->kd * pid->derivative;  //计算pid输出值
    pid->output = PID_Clamp(output, pid->output_min, pid->output_max);   //用PID_Clamp函数保证pid输出结果在边界值内，防止过冲。
    pid->last_error = pid->error;  //更新误差值

    return pid->output;
}