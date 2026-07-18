#include "app_action.h"

#include "app_chasis.h"
#include "alg_mecanum.h"
#include "bsp_timer.h"

/* ─── 常量 ─── */
#define APP_ACTION_PERIOD_MS   10u        /* 100Hz 控制周期 */
#define APP_ACTION_MAX_COUNT   32u        /* 最大动作数量 */

/* ─── 状态机枚举 ─── */
typedef enum
{
    APP_ACTION_STATE_IDLE = 0,
    APP_ACTION_STATE_RUNNING,             /* 正在执行当前动作 */
    APP_ACTION_STATE_PAUSING,             /* 动作间停顿 */
    APP_ACTION_STATE_DONE                 /* 序列全部完成 */
} AppActionInternalState_t;

/* ─── 全局变量 ─── */
static const AppAction_t *app_action_sequence = NULL;
static uint8_t app_action_count = 0u;
static uint8_t app_action_index = 0u;
static AppActionInternalState_t app_action_state = APP_ACTION_STATE_IDLE;
static uint32_t app_action_tick = 0u;     /* 当前阶段已过去的 tick 数 */
static uint32_t app_action_duration_ticks = 0u;
static uint32_t app_action_pause_ticks = 0u;
static uint8_t app_action_stop_called = 0u; /* 当前停顿是否已调用过 Stop */

/* ─── 工具函数 ─── */

/**
 * @brief 根据动作类型构造底盘速度指令
 */
static ChassisMecanumCmd_t App_ActionBuildCmd(const AppAction_t *action)
{
    ChassisMecanumCmd_t cmd = {0.0f, 0.0f, 0.0f};
    float s = action->speed;

    if (s < 0.0f)
    {
        s = -s;  /* speed 取绝对值，方向由类型决定 */
    }

    switch (action->type)
    {
        case APP_ACTION_FORWARD:
            cmd.vx_mps = s;
            break;

        case APP_ACTION_BACKWARD:
            cmd.vx_mps = -s;
            break;

        case APP_ACTION_LEFT:
            cmd.vy_mps = s;        /* 麦轮: vy>0 左平移 */
            break;

        case APP_ACTION_RIGHT:
            cmd.vy_mps = -s;       /* 麦轮: vy<0 右平移 */
            break;

        case APP_ACTION_SPIN_CW:
            cmd.wz_radps = -s;     /* wz<0 顺时针 */
            break;

        case APP_ACTION_SPIN_CCW:
            cmd.wz_radps = s;      /* wz>0 逆时针 */
            break;

        default:
            break;
    }

    return cmd;
}

/**
 * @brief 进入下一个动作（或结束）
 */
static void App_ActionAdvance(void)
{
    app_action_index++;

    if (app_action_index >= app_action_count)
    {
        /* 序列结束 */
        app_action_state = APP_ACTION_STATE_DONE;
        App_ChasisStop();
        return;
    }

    /* 检查是否遇到结束标记 */
    if (app_action_sequence[app_action_index].type == APP_ACTION_END)
    {
        app_action_state = APP_ACTION_STATE_DONE;
        App_ChasisStop();
        return;
    }

    /* 进入新动作 */
    app_action_state = APP_ACTION_STATE_RUNNING;
    app_action_tick = 0u;
    app_action_duration_ticks = app_action_sequence[app_action_index].duration_ms
                                / APP_ACTION_PERIOD_MS;
    app_action_stop_called = 0u;
}

/**
 * @brief 自动计算序列长度（遇到 APP_ACTION_END 停止）
 */
static uint8_t App_ActionCountSequence(const AppAction_t *sequence)
{
    uint8_t n = 0u;

    while (n < APP_ACTION_MAX_COUNT && sequence[n].type != APP_ACTION_END)
    {
        n++;
    }

    return n;
}

/* ─── 100Hz 控制回调 ─── */
static void App_ActionControlCallback(void)
{
    const AppAction_t *action;

    switch (app_action_state)
    {
        case APP_ACTION_STATE_IDLE:
        case APP_ACTION_STATE_DONE:
            return;

        case APP_ACTION_STATE_RUNNING:
            /* 检查当前动作是否到时 */
            if (app_action_tick >= app_action_duration_ticks)
            {
                /* 进入停顿阶段 */
                app_action_state = APP_ACTION_STATE_PAUSING;
                app_action_tick = 0u;
                app_action_pause_ticks = app_action_sequence[app_action_index].pause_ms
                                         / APP_ACTION_PERIOD_MS;
                app_action_stop_called = 0u;

                /* 停顿时间为0则直接跳到下一个 */
                if (app_action_pause_ticks == 0u)
                {
                    App_ActionAdvance();
                    return;
                }
            }
            else
            {
                /* 执行当前动作 */
                action = &app_action_sequence[app_action_index];
                ChassisMecanumCmd_t cmd = App_ActionBuildCmd(action);

                App_ChasisVelocityControl(cmd, APP_ACTION_PERIOD_MS);
                app_action_tick++;
            }
            break;

        case APP_ACTION_STATE_PAUSING:
            /* 停顿开始时停一次电机 */
            if (app_action_stop_called == 0u)
            {
                App_ChasisStop();
                app_action_stop_called = 1u;
            }

            /* 检查停顿是否到时 */
            if (app_action_tick >= app_action_pause_ticks)
            {
                App_ActionAdvance();
            }
            else
            {
                app_action_tick++;
            }
            break;

        default:
            break;
    }
}

/* ─── 公开接口 ─── */

void App_ActionSequenceStart(const AppAction_t *sequence, uint8_t count)
{
    if (sequence == NULL)
    {
        return;
    }

    /* 自动计数 */
    if (count == 0u)
    {
        count = App_ActionCountSequence(sequence);
    }

    if (count == 0u || count > APP_ACTION_MAX_COUNT)
    {
        return;
    }

    /* 初始化底盘 */
    App_ChasisInit();

    /* 保存序列 */
    app_action_sequence = sequence;
    app_action_count = count;
    app_action_index = 0u;
    app_action_tick = 0u;
    app_action_duration_ticks = sequence[0].duration_ms / APP_ACTION_PERIOD_MS;
    app_action_stop_called = 0u;
    app_action_state = APP_ACTION_STATE_RUNNING;

    /* 注册 100Hz 回调 */
    BSP_Timer100Hz_Init();
    BSP_Timer100Hz_RegisterCallback(App_ActionControlCallback);
}

void App_ActionSequenceStop(void)
{
    app_action_state = APP_ACTION_STATE_IDLE;
    app_action_sequence = NULL;
    app_action_count = 0u;
    app_action_index = 0u;
    App_ChasisStop();
}

uint8_t App_ActionSequenceIsDone(void)
{
    return (app_action_state == APP_ACTION_STATE_DONE) ? 1u : 0u;
}

uint8_t App_ActionSequenceGetCurrentIndex(void)
{
    return app_action_index;
}
