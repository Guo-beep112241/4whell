#include "app_straight_forward.h"

#include "app_chasis.h"
#include "app_tuner.h"
#include "alg_mecanum.h"
#include "bsp_timer.h"

#define APP_SF_PERIOD_MS 10u

/* 0=运行中, 1=已完成 */
static volatile uint8_t  app_sf_phase = 1u;
static volatile uint32_t app_sf_tick = 0u;
static volatile uint32_t app_sf_total_ticks = 0u;
static volatile float    app_sf_speed_mps = 0.3f;

static void App_StraightForwardCallback(void)
{
    ChassisMecanumCmd_t cmd;

    if (app_sf_phase != 0u) { return; }

    if (app_sf_tick >= app_sf_total_ticks)
    {
        App_ChasisStop();
        app_sf_phase = 1u;
        return;
    }

    cmd.vx_mps  =  app_sf_speed_mps;
    cmd.vy_mps  = 0.0f;
    cmd.wz_radps = 0.0f;
    App_ChasisVelocityControl(cmd, APP_SF_PERIOD_MS);
    app_sf_tick++;
}

void App_StraightForwardStart(float speed_mps, uint32_t duration_ms)
{
    float max_mps;
    uint32_t ticks;

    App_ChasisInit();

    max_mps = CHASSIS_MECANUM_MAX_WHEEL_RPM
              * (6.28318530718f / 60.0f)
              * CHASSIS_MECANUM_WHEEL_RADIUS_M;
    if (speed_mps < 0.0f)  { speed_mps = -speed_mps; }
    if (speed_mps > max_mps) { speed_mps = max_mps; }

    ticks = duration_ms / APP_SF_PERIOD_MS;
    if (ticks == 0u) { ticks = 1u; }

    app_sf_speed_mps   = speed_mps;
    app_sf_total_ticks = ticks;
    app_sf_tick        = 0u;
    app_sf_phase       = 0u;

    BSP_Timer100Hz_Init();
    BSP_Timer100Hz_RegisterCallback(App_StraightForwardCallback);
}

void App_StraightForwardStop(void)
{
    app_sf_phase = 1u;
    App_ChasisStop();
}

uint8_t App_StraightForwardIsDone(void)
{
    return (app_sf_phase == 1u) ? 1u : 0u;
}
