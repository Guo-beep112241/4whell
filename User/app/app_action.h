#ifndef __APP_ACTION_H__
#define __APP_ACTION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/**
 * @brief 动作类型枚举
 */
typedef enum
{
    APP_ACTION_FORWARD = 0,   /* 前进 */
    APP_ACTION_BACKWARD,      /* 后退 */
    APP_ACTION_LEFT,          /* 麦轮左平移（横移） */
    APP_ACTION_RIGHT,         /* 麦轮右平移（横移） */
    APP_ACTION_SPIN_CW,       /* 原地顺时针旋转 */
    APP_ACTION_SPIN_CCW,      /* 原地逆时针旋转 */
    APP_ACTION_END            /* 序列结束标记（speed/duration/pause 无意义） */
} AppActionType_t;

/**
 * @brief 单个动作定义
 */
typedef struct
{
    AppActionType_t type;       /* 动作类型 */
    float speed;                /* 线速度(m/s) 或 角速度(rad/s)，取绝对值 */
    uint32_t duration_ms;       /* 动作持续时间(ms) */
    uint32_t pause_ms;          /* 动作结束后的停顿时间(ms) */
} AppAction_t;

/**
 * @brief 启动动作序列
 * @param sequence  动作数组指针
 * @param count     动作数量（不含 APP_ACTION_END，以该标记结尾则传0由函数自行计数）
 *
 * @note  内部调用 App_ChasisInit()，PID参数来自 app_tuner
 * @note  每个动作之间会先停止再停顿，然后执行下一个
 *
 * @code
 * static const AppAction_t demo[] = {
 *     { APP_ACTION_FORWARD,  0.3f, 2000, 500 },
 *     { APP_ACTION_BACKWARD, 0.3f, 2000, 500 },
 *     { APP_ACTION_LEFT,     0.2f, 1500, 500 },
 *     { APP_ACTION_RIGHT,    0.2f, 1500, 500 },
 *     { APP_ACTION_SPIN_CW,  1.0f, 2000, 500 },
 *     { APP_ACTION_END,      0.0f, 0,    0   },
 * };
 * App_ActionSequenceStart(demo, 0);  // 传0自动计数
 * @endcode
 */
void App_ActionSequenceStart(const AppAction_t *sequence, uint8_t count);

/**
 * @brief 强制终止序列，电机滑行
 */
void App_ActionSequenceStop(void);

/**
 * @brief 查询序列是否已执行完毕
 * @return 0=运行中, 1=已完成
 */
uint8_t App_ActionSequenceIsDone(void);

/**
 * @brief 获取当前正在执行的动作索引（调试用）
 */
uint8_t App_ActionSequenceGetCurrentIndex(void);

#ifdef __cplusplus
}
#endif

#endif
