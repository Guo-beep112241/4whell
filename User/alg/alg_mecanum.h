#ifndef __MODULE_CHASSIS_MECANUM_H__
#define __MODULE_CHASSIS_MECANUM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// 小车轮子半径
#define CHASSIS_MECANUM_WHEEL_RADIUS_M 0.0295f

// 小车前后轮距的一半
#define CHASSIS_MECANUM_HALF_LENGTH_M 0.1f

// 小车前轮间距的一半
#define CHASSIS_MECANUM_HALF_WIDTH_M 0.119f

// 轮子最大转速
#define CHASSIS_MECANUM_MAX_WHEEL_RPM 282.0f

// 封装小车的底盘的3个运动速度
typedef struct
{
    float vx_mps; //x轴运动速度（m/s）
    float vy_mps; //y轴运动速度（m/s）
    float wz_radps; //z轴转向角速度（rad/s）
} ChassisMecanumCmd_t;


// 封装四个轮子的转速
typedef struct
{
    float fl_rpm;
    float fr_rpm;
    float rl_rpm;
    float rr_rpm;
} ChassisMecanumWheelRPM_t;

// 声明逆运动学函数：将底盘目标速度（前后/横移/旋转）解算为四个麦克纳姆轮的目标转速（rpm）。
ChassisMecanumWheelRPM_t ChassisMecanum_InverseKinematics(ChassisMecanumCmd_t cmd);

// 声明正运动学函数：将四个麦轮的目标转速解算为底盘目标速度。
ChassisMecanumCmd_t ChassisMecanum_ForwardKinematics(ChassisMecanumWheelRPM_t wheel_rpm);

// 声明限制轮速的函数，若单个轮子超速，等比例缩小每一个轮子的转速以保证小车的运动方向不变。
void ChassisMecanum_LimitWheelRPM(ChassisMecanumWheelRPM_t *wheel_rpm, float max_abs_rpm);

#ifdef __cplusplus
}
#endif

#endif