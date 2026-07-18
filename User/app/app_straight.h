#ifndef __APP_STRAIGHT_H__
#define __APP_STRAIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief 启动小车直线行驶（基于麦轮速度PID）
 * @param target_speed_mps 目标前进速度，单位m/s，正值前进，负值后退
 *
 * @note  调用前需确保硬件外设已初始化（TIM、GPIO等）
 * @note  内部会调用 App_ChasisInit() 初始化电机和PID
 * @note  PID参数（kp/ki/kd）和电机方向均从 app_tuner 读取，
 *         运行时可通过调试器修改 tuner 全局变量实时调参
 */
void App_StraightStart(float target_speed_mps);

/**
 * @brief 停止直线行驶，电机滑行
 */
void App_StraightStop(void);

/**
 * @brief 获取当前目标速度，用于调试
 */
float App_StraightGetTargetSpeed(void);

/**
 * @brief 运行时修改目标速度
 */
void App_StraightSetTargetSpeed(float target_speed_mps);

#ifdef __cplusplus
}
#endif

#endif
