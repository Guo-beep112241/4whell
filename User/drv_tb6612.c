// 包含头文件，所有需要声明的函数都需要先包含

#include "drv_tb6612.h"
#include "tim.h"
#include <assert.h>
#include <assert.h>

/* ========== 静态状态变量 ========== */
static TB6612_Direction g_motor_dir[4];   // 4 个电机当前方向
static int16_t           g_motor_speed[4]; // 4 个电机当前速度

/* ========== 输入合法性检查（内部用） ========== */
static inline int TB6612_IsMotorIdValid(TB6612_MotorId motor_id)
{
    return (motor_id >= TB6612_MOTOR_A && motor_id <= TB6612_MOTOR_D);
}

int TB6612_Init(void)
{   
    static uint8_t is_initialized = 0;
    if (is_initialized) {
        return 0;  // 已经初始化过了，直接返回
    }
    

    /* 初始化状态变量 */
    for (int i = 0; i < 4; i++)
    {
        g_motor_dir[i]   = TB6612_DIR_FORWARD;
        g_motor_speed[i] = 0;
    }


    // 加入错误检查，确保 PWM 启动成功
    if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1) != HAL_OK)
        return -1;   // 失败返回 -1
    if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2) != HAL_OK)
        return -1;
    if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3) != HAL_OK)
        return -1;   // 失败返回 -1
    if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4) != HAL_OK)
        return -1;
    

    /* 先把所有电机速度置 0 */
    TB6612_SetMotorSpeed(TB6612_MOTOR_A, 0);
    TB6612_SetMotorSpeed(TB6612_MOTOR_B, 0);
    TB6612_SetMotorSpeed(TB6612_MOTOR_C, 0);
    TB6612_SetMotorSpeed(TB6612_MOTOR_D, 0);

    /* 先把所有方向引脚置为停止状态 */
    HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_OUTB2_GPIO_Port, MOTOR_OUTB2_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MOTOR_OUTC1_GPIO_Port, MOTOR_OUTC1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_OUTC2_GPIO_Port, MOTOR_OUTC2_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin, GPIO_PIN_RESET);

    is_initialized = 1;
    return 0;         // 成功返回 0
}


// 定义设置电机方向的函数，第一个参数是电机ID（TB6612_MOTOR_X）,第二个参数是正反方向，方向由GPIO引脚的电平高低决定，后续如果转反了，就更改一下电平。
void TB6612_SetMotorDir(TB6612_MotorId motor_id, TB6612_Direction dir)
{
    if (!TB6612_IsMotorIdValid(motor_id))
        return;

    g_motor_dir[motor_id] = dir;

    switch (motor_id)
    {
    case TB6612_MOTOR_A:
        if (dir == TB6612_DIR_FORWARD)
        {
            HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_SET);
        }
        break;

    case TB6612_MOTOR_B:
        if (dir == TB6612_DIR_FORWARD)
        {
            HAL_GPIO_WritePin(MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_OUTB2_GPIO_Port, MOTOR_OUTB2_Pin, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_OUTB2_GPIO_Port, MOTOR_OUTB2_Pin, GPIO_PIN_SET);
        }
        break;

    case TB6612_MOTOR_C:
        if (dir == TB6612_DIR_FORWARD)
        {
            HAL_GPIO_WritePin(MOTOR_OUTC1_GPIO_Port, MOTOR_OUTC1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_OUTC2_GPIO_Port, MOTOR_OUTC2_Pin, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(MOTOR_OUTC1_GPIO_Port, MOTOR_OUTC1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_OUTC2_GPIO_Port, MOTOR_OUTC2_Pin, GPIO_PIN_SET);
        }
        break;

    case TB6612_MOTOR_D:
        if (dir == TB6612_DIR_FORWARD)
        {
            HAL_GPIO_WritePin(MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin, GPIO_PIN_RESET);
        }
        else
        {
            HAL_GPIO_WritePin(MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin, GPIO_PIN_SET);
        }
        break;

    default:
        break;
    }
}

// 控制电机速度的函数定义

void TB6612_SetMotorSpeed(TB6612_MotorId motor_id, int16_t speed)
{
    if (!TB6612_IsMotorIdValid(motor_id))
        return;

    if (speed < 0)
        speed = 0;
    if (speed > 1000)
        speed = 1000;

    g_motor_speed[motor_id] = speed;

    // 把速度值转换成占空比，htim5.Init.Period是ARR（也就是总周期长度），pulse指脉冲宽度
    uint32_t period = htim5.Init.Period + 1;  
    uint32_t pulse = (uint32_t)speed * period / 1000U;

    switch (motor_id)
    {
    case TB6612_MOTOR_A:
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, pulse);
        break;
    case TB6612_MOTOR_B:
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, pulse);
        break;
    case TB6612_MOTOR_C:
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, pulse);
        break;
    case TB6612_MOTOR_D:
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, pulse);
        break;
    default:
        break;
    }
}

// 电机使能函数定义，先默认设置成300的初速度。
void TB6612_EnableMotor(TB6612_MotorId motor_id)
{
    if (!TB6612_IsMotorIdValid(motor_id))
        return;

    TB6612_SetMotorSpeed(motor_id, 300);
}

// 电机失能函数定义，直接把速度设置为0，并且把方向引脚拉低。
void TB6612_DisableMotor(TB6612_MotorId motor_id)
{
    if (!TB6612_IsMotorIdValid(motor_id))
        return;

    g_motor_speed[motor_id] = 0;

    switch (motor_id)
    {
    case TB6612_MOTOR_A:
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, 0);
        HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_RESET);
        break;

    case TB6612_MOTOR_B:
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, 0);
        HAL_GPIO_WritePin(MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_OUTB2_GPIO_Port, MOTOR_OUTB2_Pin, GPIO_PIN_RESET);
        break;

    case TB6612_MOTOR_C:
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, 0);
        HAL_GPIO_WritePin(MOTOR_OUTC1_GPIO_Port, MOTOR_OUTC1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_OUTC2_GPIO_Port, MOTOR_OUTC2_Pin, GPIO_PIN_RESET);
        break;

    case TB6612_MOTOR_D:
        __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, 0);
        HAL_GPIO_WritePin(MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin, GPIO_PIN_RESET);
        break;

    default:
        break;
    }
}

