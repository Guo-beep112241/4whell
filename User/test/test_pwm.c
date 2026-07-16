#include "test_pwm.h"
#include "drv_motor.h"
#include "bsp_timer.h"
#include "alg_mecanum.h" 

typedef enum
{
    TEST_PHASE_POS_ACCEL = 0,
    TEST_PHASE_POS_DECEL,
    TEST_PHASE_NEG_ACCEL,
    TEST_PHASE_NEG_DECEL,
    TEST_PHASE_BRAKE,
    TEST_PHASE_DONE
} TestMotorPhase_t;



// TestMotorSequenceStart用到的
volatile uint32_t test_motor_speed_update_cnt = 0u;

volatile uint8_t test_motor_current = MOTOR_FL;
volatile uint8_t test_motor_phase = TEST_PHASE_POS_ACCEL;
volatile uint16_t test_motor_phase_tick = 0u;
volatile uint8_t test_motor_sequence_done = 0u;

// TestClearOutputPercentDebug用到的
volatile float test_motor_fl_output_percent = 0.0f;
volatile float test_motor_fr_output_percent = 0.0f;
volatile float test_motor_rl_output_percent = 0.0f;
volatile float test_motor_rr_output_percent = 0.0f;

// TestClearTargetRPMDebug用到的
volatile float test_motor_fl_target_rpm = 0.0f;
volatile float test_motor_fr_target_rpm = 0.0f;
volatile float test_motor_rl_target_rpm = 0.0f;
volatile float test_motor_rr_target_rpm = 0.0f;

volatile float test_chassis_target_vx_mps = 0.0f;
volatile float test_chassis_target_vy_mps = 0.0f;

// TestUpdateSpeedDebug中用到
volatile float test_motor_fl_rpm = 0.0f;
volatile float test_motor_fr_rpm = 0.0f;
volatile float test_motor_rl_rpm = 0.0f;
volatile float test_motor_rr_rpm = 0.0f;
volatile int32_t test_motor_fl_cnt_per_s = 0;
volatile int32_t test_motor_fr_cnt_per_s = 0;
volatile int32_t test_motor_rl_cnt_per_s = 0;
volatile int32_t test_motor_rr_cnt_per_s = 0;

// TestUpdateChassisSpeedMPSDebug中用到
volatile float test_motor_fl_mps = 0.0f;
volatile float test_motor_fr_mps = 0.0f;
volatile float test_motor_rl_mps = 0.0f;
volatile float test_motor_rr_mps = 0.0f;

// TestUpdateChassisSpeedMPSDebug中用于
volatile float test_chassis_vx_mps = 0.0f;
volatile float test_chassis_vy_mps = 0.0f;
volatile float test_chassis_wz_radps = 0.0f;

// TestMotorSpeed100HzCallback用到的
#define TEST_MOTOR_SPEED_PERIOD_MS 10u

// TestMotorSpeed100HzCallback中用到
#define TEST_MOTOR_SPEED_PERIOD_MS 10u
#define TEST_MOTOR_CONTROL_PERIOD_S 0.01f
#define TEST_MOTOR_RAMP_TIME_MS 2000u
#define TEST_MOTOR_RAMP_TICKS (TEST_MOTOR_RAMP_TIME_MS / TEST_MOTOR_SPEED_PERIOD_MS)
#define TEST_MOTOR_MAX_OUTPUT_PERCENT 100.0f

// TestUpdateChassisSpeedMPSDebug中用到
#define TEST_RPM_TO_RADPS (6.28318530718f / 60.0f)



static const MotorId_t test_motor_order[MOTOR_NUM] = {
    MOTOR_FL,
    MOTOR_FR,
    MOTOR_RL,
    MOTOR_RR,
};

static uint8_t test_motor_order_index = 0u;
static uint16_t test_motor_local_phase_tick = 0u;
static TestMotorPhase_t test_motor_local_phase = TEST_PHASE_POS_ACCEL;


static void TestClearOutputPercentDebug(void)
{
    test_motor_fl_output_percent = 0.0f;
    test_motor_fr_output_percent = 0.0f;
    test_motor_rl_output_percent = 0.0f;
    test_motor_rr_output_percent = 0.0f;
}

static void TestClearTargetRPMDebug(void)
{
    test_motor_fl_target_rpm = 0.0f;
    test_motor_fr_target_rpm = 0.0f;
    test_motor_rl_target_rpm = 0.0f;
    test_motor_rr_target_rpm = 0.0f;

    test_chassis_target_vx_mps = 0.0f;
    test_chassis_target_vy_mps = 0.0f;
}

static float TestGetRampRatio(uint16_t tick)
{
    if (TEST_MOTOR_RAMP_TICKS <= 1u)
    {
        return 1.0f;
    }

    if (tick >= (TEST_MOTOR_RAMP_TICKS - 1u))
    {
        return 1.0f;
    }

    return (float)tick / (float)(TEST_MOTOR_RAMP_TICKS - 1u);
}

static void TestSetOutputPercentDebug(MotorId_t motor, float output_percent)
{
    TestClearOutputPercentDebug();

    switch (motor)
    {
        case MOTOR_FL:
            test_motor_fl_output_percent = output_percent;
            break;

        case MOTOR_FR:
            test_motor_fr_output_percent = output_percent;
            break;

        case MOTOR_RL:
            test_motor_rl_output_percent = output_percent;
            break;

        case MOTOR_RR:
            test_motor_rr_output_percent = output_percent;
            break;

        default:
            break;
    }
}

static void TestAdvancePhase(void)
{
    if (test_motor_local_phase == TEST_PHASE_BRAKE)
    {
        if ((test_motor_order_index + 1u) >= MOTOR_NUM)
        {
            test_motor_local_phase = TEST_PHASE_DONE;
            test_motor_sequence_done = 1u;
            return;
        }

        test_motor_order_index++;
        test_motor_local_phase_tick = 0u;
        test_motor_local_phase = TEST_PHASE_POS_ACCEL;
        return;
    }

    test_motor_local_phase_tick++;

    if (test_motor_local_phase_tick < TEST_MOTOR_RAMP_TICKS)
    {
        return;
    }

    test_motor_local_phase_tick = 0u;
    test_motor_local_phase = (TestMotorPhase_t)((uint8_t)test_motor_local_phase + 1u);
}


static void TestUpdateMotorSequence(void)
{
    MotorId_t motor;
    float ratio;
    float output_percent = 0.0f;

    if (test_motor_sequence_done != 0u)
    {
        return;
    }

    motor = test_motor_order[test_motor_order_index];
    ratio = TestGetRampRatio(test_motor_local_phase_tick);

    switch (test_motor_local_phase)
    {
        case TEST_PHASE_POS_ACCEL:
            output_percent = TEST_MOTOR_MAX_OUTPUT_PERCENT * ratio;
            Motor_SetOutputPercent(motor, output_percent);
            break;

        case TEST_PHASE_POS_DECEL:
            output_percent = TEST_MOTOR_MAX_OUTPUT_PERCENT * (1.0f - ratio);
            Motor_SetOutputPercent(motor, output_percent);
            break;

        case TEST_PHASE_NEG_ACCEL:
            output_percent = -TEST_MOTOR_MAX_OUTPUT_PERCENT * ratio;
            Motor_SetOutputPercent(motor, output_percent);
            break;

        case TEST_PHASE_NEG_DECEL:
            output_percent = -TEST_MOTOR_MAX_OUTPUT_PERCENT * (1.0f - ratio);
            Motor_SetOutputPercent(motor, output_percent);
            break;

        case TEST_PHASE_BRAKE:
            output_percent = 0.0f;
            Motor_Brake(motor);
            break;

        case TEST_PHASE_DONE:
        default:
            test_motor_sequence_done = 1u;
            output_percent = 0.0f;
            break;
    }

    test_motor_current = (uint8_t)motor;
    test_motor_phase = (uint8_t)test_motor_local_phase;
    test_motor_phase_tick = test_motor_local_phase_tick;
    TestSetOutputPercentDebug(motor, output_percent);

    TestAdvancePhase();
}

static float TestRPMToWheelMPS(float rpm)
{
    return rpm * TEST_RPM_TO_RADPS * CHASSIS_MECANUM_WHEEL_RADIUS_M;
}

static void TestUpdateChassisSpeedMPSDebug(float fl_rpm, float fr_rpm, float rl_rpm, float rr_rpm)
{
    float fl_radps = fl_rpm * TEST_RPM_TO_RADPS;
    float fr_radps = fr_rpm * TEST_RPM_TO_RADPS;
    float rl_radps = rl_rpm * TEST_RPM_TO_RADPS;
    float rr_radps = rr_rpm * TEST_RPM_TO_RADPS;
    float radius = CHASSIS_MECANUM_WHEEL_RADIUS_M;
    float wheel_base = CHASSIS_MECANUM_HALF_LENGTH_M + CHASSIS_MECANUM_HALF_WIDTH_M;

    test_motor_fl_mps = TestRPMToWheelMPS(fl_rpm);
    test_motor_fr_mps = TestRPMToWheelMPS(fr_rpm);
    test_motor_rl_mps = TestRPMToWheelMPS(rl_rpm);
    test_motor_rr_mps = TestRPMToWheelMPS(rr_rpm);

    test_chassis_vx_mps = radius * (fl_radps + fr_radps + rl_radps + rr_radps) * 0.25f;
    test_chassis_vy_mps = radius * (-fl_radps + fr_radps + rl_radps - rr_radps) * 0.25f;

    if (wheel_base > 0.0f)
    {
        test_chassis_wz_radps = radius * (-fl_radps + fr_radps - rl_radps + rr_radps) / (4.0f * wheel_base);
    }
    else
    {
        test_chassis_wz_radps = 0.0f;
    }
}



static void TestUpdateSpeedDebug(void)
{
    test_motor_fl_rpm = Motor_GetSpeedRPM(MOTOR_FL);
    test_motor_fr_rpm = Motor_GetSpeedRPM(MOTOR_FR);
    test_motor_rl_rpm = Motor_GetSpeedRPM(MOTOR_RL);
    test_motor_rr_rpm = Motor_GetSpeedRPM(MOTOR_RR);

    test_motor_fl_cnt_per_s = Motor_GetSpeedCntPerS(MOTOR_FL);
    test_motor_fr_cnt_per_s = Motor_GetSpeedCntPerS(MOTOR_FR);
    test_motor_rl_cnt_per_s = Motor_GetSpeedCntPerS(MOTOR_RL);
    test_motor_rr_cnt_per_s = Motor_GetSpeedCntPerS(MOTOR_RR);

    TestUpdateChassisSpeedMPSDebug(test_motor_fl_rpm,
                                       test_motor_fr_rpm,
                                       test_motor_rl_rpm,
                                       test_motor_rr_rpm);
}


static void TestMotorSpeed100HzCallback(void)
{
    Motor_UpdateSpeed(TEST_MOTOR_SPEED_PERIOD_MS);
    TestUpdateMotorSequence();
    TestUpdateSpeedDebug();

    test_motor_speed_update_cnt++;
}



void TestMotorSequenceStart(void)
{
    Motor_Init();
    Motor_StopAll();

    test_motor_speed_update_cnt = 0u;
    test_motor_order_index = 0u;
    test_motor_local_phase_tick = 0u;
    test_motor_local_phase = TEST_PHASE_POS_ACCEL;
    test_motor_current = MOTOR_FL;
    test_motor_phase = TEST_PHASE_POS_ACCEL;
    test_motor_phase_tick = 0u;
    test_motor_sequence_done = 0u;
    TestClearOutputPercentDebug();
    TestClearTargetRPMDebug();

    BSP_Timer100Hz_RegisterCallback(TestMotorSpeed100HzCallback);
    BSP_Timer100Hz_Init();
}