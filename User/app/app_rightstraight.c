#include "app_rightstraight.h"

#include "app_chasis.h"
#include "app_tuner.h"
#include "alg_mecanum.h"
#include "bsp_timer.h"

#define APP_RS_PERIOD_MS 10u

/* 0=运行中, 1=已完成 */
static volatile uint8_t  app_rs_phase = 1u;
static volatile uint32_t app_rs_tick = 0u;
static volatile uint32_t app_rs_total_ticks = 0u;
static volatile float    app_rs_speed_mps = 0.3f;

static void App_RightStraightCallback(void)
{
    ChassisMecanumCmd_t cmd;

    if (app_rs_phase != 0u) { return; }

    if (app_rs_tick >= app_rs_total_ticks)
    {
        App_ChasisStop();
        app_rs_phase = 1u;
        return;
    }

    /* 右平移: vy 取负值 */
    cmd.vx_mps   = 0.0f;
    cmd.vy_mps   = -app_rs_speed_mps;
    cmd.wz_radps = 0.0f;
    App_ChasisVelocityControl(cmd, APP_RS_PERIOD_MS);
    app_rs_tick++;
}

void App_RightStraightStart(float speed_mps, uint32_t duration_ms)
{
    float max_mps;
    uint32_t ticks;

    App_ChasisInit();

    max_mps = CHASSIS_MECANUM_MAX_WHEEL_RPM
              * (6.28318530718f / 60.0f)
              * CHASSIS_MECANUM_WHEEL_RADIUS_M;
    if (speed_mps < 0.0f)  { speed_mps = -speed_mps; }
    if (speed_mps > max_mps) { speed_mps = max_mps; }

    ticks = duration_ms / APP_RS_PERIOD_MS;
    if (ticks == 0u) { ticks = 1u; }

    app_rs_speed_mps   = speed_mps;
    app_rs_total_ticks = ticks;
    app_rs_tick        = 0u;
    app_rs_phase       = 0u;

    BSP_Timer100Hz_Init();
    BSP_Timer100Hz_RegisterCallback(App_RightStraightCallback);
}

void App_RightStraightStop(void)
{
    app_rs_phase = 1u;
    App_ChasisStop();
}

uint8_t App_RightStraightIsDone(void)
{
    return (app_rs_phase == 1u) ? 1u : 0u;
}
