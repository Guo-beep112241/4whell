#ifndef __APP_RIGHTSTRAIGHT_H__
#define __APP_RIGHTSTRAIGHT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief 启动小车右平移
 * @param speed_mps   线速度 (m/s)，取正值
 * @param duration_ms 持续时间 (ms)
 */
void App_RightStraightStart(float speed_mps, uint32_t duration_ms);

/**
 * @brief 强制停止右平移
 */
void App_RightStraightStop(void);

/**
 * @brief 查询右平移是否已完成
 * @return 0=运行中, 1=已完成
 */
uint8_t App_RightStraightIsDone(void);

#ifdef __cplusplus
}
#endif

#endif
