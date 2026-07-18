#include "app_straight.h"

#include "app_chasis.h"
#include "app_tuner.h"
#include "alg_mecanum.h"
#include "bsp_timer.h"

/* 控制周期: 10ms = 100Hz，与 app_chasis 内部 PID 的 dt 一致 */
#define APP_STRAIGHT_PERIOD_MS 10u

/* 默认目标速度: 0.3 m/s，约 1 km/h，适合调试起步 */
#define APP_STRAIGHT_DEFAULT_SPEED_MPS 0.3f

/* 全局运行状态 */
static float app_straight_target_speed_mps = 0.0f;
static uint8_t app_straight_running = 0u;

/* ─── 100Hz 控制回调 ─── */
static void App_StraightControlCallback(void)
{
    ChassisMecanumCmd_t cmd;

    if (app_straight_running == 0u)
    {
        return;
    }

    /* 纯前进: vx = 目标速度, vy = 0（无横移）, wz = 0（无自旋） */
    cmd.vx_mps = app_straight_target_speed_mps;
    cmd.vy_mps = 0.0f;
    cmd.wz_radps = 0.0f;

    /*
     * App_ChasisVelocityControl 内部流程:
     *   1. Motor_UpdateSpeed() —— 读取编码器，更新四轮实时转速
     *   2. ChassisMecanum_InverseKinematics() —— 将 vx/vy/wz 解算为四轮目标 RPM
     *   3. ChassisMecanum_LimitWheelRPM() —— 限制单轮不超 CHASSIS_MECANUM_MAX_WHEEL_RPM
     *   4. 对每个轮子 PID_Calc(target_rpm, feedback_rpm, dt) —— 速度闭环
     *       PID 参数取自 app_tuner: speed_kp / speed_ki / speed_kd
     *   5. Motor_SetOutputPercent() —— 输出 PWM 占空比到 TB6612 驱动
     */
    App_ChasisVelocityControl(cmd, APP_STRAIGHT_PERIOD_MS);
}

/* ─── 公开接口 ─── */

void App_StraightStart(float target_speed_mps)
{
    /* 初始化底盘: 电机GPIO/PWM/编码器 + PID对象创建 + tuner参数加载 */
    App_ChasisInit();

    /* 速度保护: 按轮径换算，限制在轮子最大转速内 */
    {
        float max_linear_mps = CHASSIS_MECANUM_MAX_WHEEL_RPM
                               * (6.28318530718f / 60.0f)
                               * CHASSIS_MECANUM_WHEEL_RADIUS_M;

        if (target_speed_mps > max_linear_mps)
        {
            target_speed_mps = max_linear_mps;
        }
        else if (target_speed_mps < -max_linear_mps)
        {
            target_speed_mps = -max_linear_mps;
        }
    }

    app_straight_target_speed_mps = target_speed_mps;
    app_straight_running = 1u;

    /* 注册 100Hz 定时回调，驱动控制循环 */
    BSP_Timer100Hz_Init();
    BSP_Timer100Hz_RegisterCallback(App_StraightControlCallback);
}

void App_StraightStop(void)
{
    app_straight_running = 0u;
    App_ChasisStop();
}

float App_StraightGetTargetSpeed(void)
{
    return app_straight_target_speed_mps;
}

void App_StraightSetTargetSpeed(float target_speed_mps)
{
    float max_linear_mps = CHASSIS_MECANUM_MAX_WHEEL_RPM
                           * (6.28318530718f / 60.0f)
                           * CHASSIS_MECANUM_WHEEL_RADIUS_M;

    if (target_speed_mps > max_linear_mps)
    {
        target_speed_mps = max_linear_mps;
    }
    else if (target_speed_mps < -max_linear_mps)
    {
        target_speed_mps = -max_linear_mps;
    }

    app_straight_target_speed_mps = target_speed_mps;
}
