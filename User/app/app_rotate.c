#include "app_rotate.h"

#include "app_chasis.h"
#include "app_tuner.h"
#include "alg_mecanum.h"
#include "bsp_timer.h"

#define APP_ROT_PERIOD_MS 10u
#define APP_ROT_RAMP_TICKS 50u  /* 斜坡时间 = 50×10ms = 500ms */

/* 0=运行中, 1=已完成 */
static volatile uint8_t  app_rot_phase = 1u;
static volatile uint32_t app_rot_tick = 0u;
static volatile uint32_t app_rot_total_ticks = 0u;
static volatile float    app_rot_angular_speed_radps = 1.0f;

static void App_RotateCallback(void)
{
    ChassisMecanumCmd_t cmd;
    float current_wz;

    if (app_rot_phase != 0u) { return; }

    if (app_rot_tick >= app_rot_total_ticks)
    {
        App_ChasisStop();
        app_rot_phase = 1u;
        return;
    }

    /* 加速度斜坡: 在RAMP_TICKS内从0线性过渡到目标角速度 */
    if (app_rot_tick < APP_ROT_RAMP_TICKS)
    {
        current_wz = app_rot_angular_speed_radps
                     * (float)app_rot_tick / (float)APP_ROT_RAMP_TICKS;
    }
    else if (app_rot_tick > app_rot_total_ticks - APP_ROT_RAMP_TICKS)
    {
        /* 减速斜坡: 结束前RAMP_TICKS内线性减速到0 */
        current_wz = app_rot_angular_speed_radps
                     * (float)(app_rot_total_ticks - app_rot_tick)
                     / (float)APP_ROT_RAMP_TICKS;
    }
    else
    {
        current_wz = app_rot_angular_speed_radps;
    }

    /* 原地旋转: wz>0 反转(逆时针), wz<0 正转(顺时针), vx/vy 为零 */
    cmd.vx_mps   = 0.0f;
    cmd.vy_mps   = 0.0f;
    cmd.wz_radps = current_wz;
    App_ChasisVelocityControl(cmd, APP_ROT_PERIOD_MS);
    app_rot_tick++;
}

void App_RotateStart(float angular_speed_radps, uint32_t duration_ms)
{
    float max_wz_radps;
    uint32_t ticks;

    App_ChasisInit();

    /* 最大角速度 = 最大轮线速度 / (半长 + 半宽) */
    max_wz_radps = CHASSIS_MECANUM_MAX_WHEEL_RPM
                   * (6.28318530718f / 60.0f)
                   * CHASSIS_MECANUM_WHEEL_RADIUS_M
                   / (CHASSIS_MECANUM_HALF_LENGTH_M + CHASSIS_MECANUM_HALF_WIDTH_M);

    /* 限制角速度幅值，但保留符号以支持正反转:
       wz>0 → 反转(逆时针), wz<0 → 正转(顺时针) */
    if (angular_speed_radps >  max_wz_radps) { angular_speed_radps =  max_wz_radps; }
    if (angular_speed_radps < -max_wz_radps) { angular_speed_radps = -max_wz_radps; }

    ticks = duration_ms / APP_ROT_PERIOD_MS;
    if (ticks == 0u) { ticks = 1u; }

    app_rot_angular_speed_radps = angular_speed_radps;
    app_rot_total_ticks = ticks;
    app_rot_tick        = 0u;
    app_rot_phase       = 0u;

    BSP_Timer100Hz_Init();
    BSP_Timer100Hz_RegisterCallback(App_RotateCallback);
}

void App_RotateStop(void)
{
    app_rot_phase = 1u;
    App_ChasisStop();
}

uint8_t App_RotateIsDone(void)
{
    return (app_rot_phase == 1u) ? 1u : 0u;
}
