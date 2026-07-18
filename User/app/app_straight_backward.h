#ifndef __APP_STRAIGHT_BACKWARD_H__
#define __APP_STRAIGHT_BACKWARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief 启动小车后退
 * @param speed_mps   线速度 (m/s)，取正值
 * @param duration_ms 持续时间 (ms)
 */
void App_StraightBackwardStart(float speed_mps, uint32_t duration_ms);

/**
 * @brief 强制停止后退
 */
void App_StraightBackwardStop(void);

/**
 * @brief 查询后退是否已完成
 * @return 0=运行中, 1=已完成
 */
uint8_t App_StraightBackwardIsDone(void);

#ifdef __cplusplus
}
#endif

#endif
