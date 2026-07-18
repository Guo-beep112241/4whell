#ifndef __APP_ROTATE_H__
#define __APP_ROTATE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief 启动小车原地旋转
 * @param angular_speed_radps 角速度 (rad/s)
 *            wz>0 → 反转(逆时针)
 *            wz<0 → 正转(顺时针)
 * @param duration_ms         持续时间 (ms)
 */
void App_RotateStart(float angular_speed_radps, uint32_t duration_ms);

/**
 * @brief 强制停止旋转
 */
void App_RotateStop(void);

/**
 * @brief 查询旋转是否已完成
 * @return 0=运行中, 1=已完成
 */
uint8_t App_RotateIsDone(void);

#ifdef __cplusplus
}
#endif

#endif
