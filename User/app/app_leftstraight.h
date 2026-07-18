#ifndef __APP_LEFTSTRAIGHT_H__
#define __APP_LEFTSTRAIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief 启动小车左平移
 * @param speed_mps   线速度 (m/s)，取正值
 * @param duration_ms 持续时间 (ms)
 */
void App_LeftStraightStart(float speed_mps, uint32_t duration_ms);

/**
 * @brief 强制停止左平移
 */
void App_LeftStraightStop(void);

/**
 * @brief 查询左平移是否已完成
 * @return 0=运行中, 1=已完成
 */
uint8_t App_LeftStraightIsDone(void);

#ifdef __cplusplus
}
#endif

#endif
