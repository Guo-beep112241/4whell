
#ifndef __APP_CHASIS_H__
#define __APP_CHASIS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "app_tuner.h"
#include "alg_mecanum.h"


typedef struct
{
    float vx_mps;
    float vy_mps;
    float target_yaw_rad;
    float gyro_yaw_rad;
} AppChasisHeadingCmd_t;


typedef struct
{
    float fl_percent;
    float fr_percent;
    float rl_percent;
    float rr_percent;
} AppChasisWheelOutput_t;


typedef struct
{
    ChassisMecanumCmd_t cmd;
    ChassisMecanumWheelRPM_t target_rpm;
    ChassisMecanumWheelRPM_t feedback_rpm;
    AppChasisWheelOutput_t output_percent;
    uint32_t update_cnt;
} AppChasisControlState_t;


extern volatile AppChasisControlState_t app_chasis_control_state;


void App_ChasisInit(void);


void App_ChasisTurnPIDReset(void);


void App_ChasisTurnPIDSetParam(float kp, float ki, float kd);


void App_ChasisSpeedPIDReset(void);


void App_ChasisSpeedPIDSetParam(float kp, float ki, float kd);


float App_ChasisTurnPIDCalc(float target_yaw_rad, float gyro_yaw_rad, float dt_s);


ChassisMecanumWheelRPM_t App_ChasisHeadingSolve(AppChasisHeadingCmd_t cmd, float dt_s);


void App_ChasisVelocityControl(ChassisMecanumCmd_t cmd, uint32_t dt_ms);


void App_ChasisHeadingControl(AppChasisHeadingCmd_t cmd, uint32_t dt_ms);


void App_ChasisStop(void);


void App_ChasisBrake(void);


AppChasisControlState_t App_ChasisGetControlState(void);

#ifdef __cplusplus
}
#endif

#endif


