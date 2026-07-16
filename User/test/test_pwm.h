#ifndef __TEST_PWM_H__
#define __TEST_PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"



// TestMotorSequenceStart用到的
extern volatile uint32_t test_motor_speed_update_cnt;

extern volatile uint8_t test_motor_current;
extern volatile uint8_t test_motor_phase;
extern volatile uint16_t test_motor_phase_tick;
extern volatile uint8_t test_motor_sequence_done;

// TestClearOutputPercentDebug用到的
extern volatile float test_motor_fl_output_percent;
extern volatile float test_motor_fr_output_percent;
extern volatile float test_motor_rl_output_percent;
extern volatile float test_motor_rr_output_percent;

// TestClearTargetRPMDebug用到的
extern volatile float test_motor_fl_target_rpm;
extern volatile float test_motor_fr_target_rpm;
extern volatile float test_motor_rl_target_rpm;
extern volatile float test_motor_rr_target_rpm;

extern volatile float test_chassis_target_vx_mps;
extern volatile float test_chassis_target_vy_mps;

// TestUpdateSpeedDebug中用到
extern volatile float test_motor_fl_rpm;
extern volatile float test_motor_fr_rpm;
extern volatile float test_motor_rl_rpm;
extern volatile float test_motor_rr_rpm;
extern volatile int32_t test_motor_fl_cnt_per_s;
extern volatile int32_t test_motor_fr_cnt_per_s;
extern volatile int32_t test_motor_rl_cnt_per_s;
extern volatile int32_t test_motor_rr_cnt_per_s;

// TestUpdateChassisSpeedMPSDebug中用到
extern volatile float test_motor_fl_mps;
extern volatile float test_motor_fr_mps;
extern volatile float test_motor_rl_mps;
extern volatile float test_motor_rr_mps;

// TestUpdateChassisSpeedMPSDebug中用于
extern volatile float test_chassis_vx_mps;
extern volatile float test_chassis_vy_mps;
extern volatile float test_chassis_wz_radps;


void TestMotorSequenceStart(void);

#ifdef __cplusplus
}
#endif

#endif