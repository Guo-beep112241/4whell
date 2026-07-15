#include "alg_mecanum.h"

#include <stddef.h>

// 宏定义：小车底盘角速度到轮子转速的单位转换
#define CHASSIS_MECANUM_RADPS_TO_RPM (60.0f / 6.28318530718f)
#define CHASSIS_MECANUM_RPM_TO_RADPS (6.28318530718f / 60.0f)

//定义静态函数求绝对值
static float ChassisMecanum_Abs(float value)
{
    return (value >= 0.0f) ? value : -value; //三元运算符 条件 ? 真 : 假 
}


//定义通过擂台比较求最大值的静态函数，最后返回出四个轮子中的最大转速
static float ChassisMecanum_GetMaxAbsRPM(ChassisMecanumWheelRPM_t wheel_rpm)
{
    float max_abs = ChassisMecanum_Abs(wheel_rpm.fl_rpm);
    float value = ChassisMecanum_Abs(wheel_rpm.fr_rpm);

    if (value > max_abs)
    {
        max_abs = value;
    }

    value = ChassisMecanum_Abs(wheel_rpm.rl_rpm);
    if (value > max_abs)
    {
        max_abs = value;
    }

    value = ChassisMecanum_Abs(wheel_rpm.rr_rpm);
    if (value > max_abs)
    {
        max_abs = value;
    }

    return max_abs;
}


// 定义将底盘速度计算成每个轮子的转速的函数（逆运动学计算公式）
ChassisMecanumWheelRPM_t ChassisMecanum_InverseKinematics(ChassisMecanumCmd_t cmd)
{
    ChassisMecanumWheelRPM_t wheel_rpm = {0.0f, 0.0f, 0.0f, 0.0f};
    float wheel_base = CHASSIS_MECANUM_HALF_LENGTH_M + CHASSIS_MECANUM_HALF_WIDTH_M;
    float radius = CHASSIS_MECANUM_WHEEL_RADIUS_M;
    float rpm_per_mps;

    if (radius <= 0.0f)
    {
        return wheel_rpm;
    }

    rpm_per_mps = CHASSIS_MECANUM_RADPS_TO_RPM / radius;

    wheel_rpm.fl_rpm = (cmd.vx_mps - cmd.vy_mps - wheel_base * cmd.wz_radps) * rpm_per_mps;
    wheel_rpm.fr_rpm = (cmd.vx_mps + cmd.vy_mps + wheel_base * cmd.wz_radps) * rpm_per_mps;
    wheel_rpm.rl_rpm = (cmd.vx_mps + cmd.vy_mps - wheel_base * cmd.wz_radps) * rpm_per_mps;
    wheel_rpm.rr_rpm = (cmd.vx_mps - cmd.vy_mps + wheel_base * cmd.wz_radps) * rpm_per_mps;

    return wheel_rpm;
}

// 定义正运动学函数：将轮子的转速转换计算为小车底盘的三个运动速度
ChassisMecanumCmd_t ChassisMecanum_ForwardKinematics(ChassisMecanumWheelRPM_t wheel_rpm)
{
    ChassisMecanumCmd_t cmd = {0.0f, 0.0f, 0.0f};
    float radius = CHASSIS_MECANUM_WHEEL_RADIUS_M;
    float wheel_base = CHASSIS_MECANUM_HALF_LENGTH_M + CHASSIS_MECANUM_HALF_WIDTH_M;
    float fl_mps;
    float fr_mps;
    float rl_mps;
    float rr_mps;

    if (radius <= 0.0f)
    {
        return cmd;
    }

    fl_mps = wheel_rpm.fl_rpm * CHASSIS_MECANUM_RPM_TO_RADPS * radius;
    fr_mps = wheel_rpm.fr_rpm * CHASSIS_MECANUM_RPM_TO_RADPS * radius;
    rl_mps = wheel_rpm.rl_rpm * CHASSIS_MECANUM_RPM_TO_RADPS * radius;
    rr_mps = wheel_rpm.rr_rpm * CHASSIS_MECANUM_RPM_TO_RADPS * radius;

    cmd.vx_mps = (fl_mps + fr_mps + rl_mps + rr_mps) * 0.25f;
    cmd.vy_mps = (-fl_mps + fr_mps + rl_mps - rr_mps) * 0.25f;

    if (wheel_base > 0.0f)
    {
        cmd.wz_radps = (-fl_mps + fr_mps - rl_mps + rr_mps) / (4.0f * wheel_base);
    }

    return cmd;
}

// 定义限速函数，等比例缩小每个轮速以保证运动方向不变
void ChassisMecanum_LimitWheelRPM(ChassisMecanumWheelRPM_t *wheel_rpm, float max_abs_rpm)
{
    float current_max;
    float scale;

    if ((wheel_rpm == NULL) || (max_abs_rpm <= 0.0f))
    {
        return;
    }

    current_max = ChassisMecanum_GetMaxAbsRPM(*wheel_rpm);

    if (current_max <= max_abs_rpm)
    {
        return;
    }

    scale = max_abs_rpm / current_max;
    wheel_rpm->fl_rpm *= scale;
    wheel_rpm->fr_rpm *= scale;
    wheel_rpm->rl_rpm *= scale;
    wheel_rpm->rr_rpm *= scale;
}
