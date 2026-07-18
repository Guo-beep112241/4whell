#include "app_straight_backward.h"

#include "app_chasis.h"
#include "app_tuner.h"
#include "alg_mecanum.h"
#include "bsp_timer.h"

#define APP_SB_PERIOD_MS 10u

/* 0=运行中, 1=已完成 */
static volatile uint8_t  app_sb_phase = 1u;
static volatile uint32_t app_sb_tick = 0u;
static volatile uint32_t app_sb_total_ticks = 0u;
static volatile float    app_sb_speed_mps = 0.3f;

static void App_StraightBackwardCallback(void)
{
    ChassisMecanumCmd_t cmd;

    if (app_sb_phase != 0u) { return; }

    if (app_sb_tick >= app_sb_total_ticks)
    {
        App_ChasisStop();
        app_sb_phase = 1u;
        return;
    }

    /* 后退: vx 取负值 */
    cmd.vx_mps  = -app_sb_speed_mps;
    cmd.vy_mps  = 0.0f;
    cmd.wz_radps = 0.0f;
    App_ChasisVelocityControl(cmd, APP_SB_PERIOD_MS);
    app_sb_tick++;
}

void App_StraightBackwardStart(float speed_mps, uint32_t duration_ms)
{
    float max_mps;
    uint32_t ticks;

    App_ChasisInit();

    max_mps = CHASSIS_MECANUM_MAX_WHEEL_RPM
              * (6.28318530718f / 60.0f)
              * CHASSIS_MECANUM_WHEEL_RADIUS_M;
    if (speed_mps < 0.0f)  { speed_mps = -speed_mps; }
    if (speed_mps > max_mps) { speed_mps = max_mps; }

    ticks = duration_ms / APP_SB_PERIOD_MS;
    if (ticks == 0u) { ticks = 1u; }

    app_sb_speed_mps   = speed_mps;
    app_sb_total_ticks = ticks;
    app_sb_tick        = 0u;
    app_sb_phase       = 0u;

    BSP_Timer100Hz_Init();
    BSP_Timer100Hz_RegisterCallback(App_StraightBackwardCallback);
}

void App_StraightBackwardStop(void)
{
    app_sb_phase = 1u;
    App_ChasisStop();
}

uint8_t App_StraightBackwardIsDone(void)
{
    return (app_sb_phase == 1u) ? 1u : 0u;
}
