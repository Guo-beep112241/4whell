// 包含头文件，所有需要声明的函数都需要先包含

#include "drv_tb6612.h"
#include "tim.h"
// #include <assert.h>
// #include <assert.h>

// /* ========== 静态状态变量 ========== */
// static TB6612_Direction g_motor_dir[4];   // 4 个电机当前方向
// static int16_t           g_motor_speed[4]; // 4 个电机当前速度


// 封装同一电机的1个PWM通道和2个方向引脚的结构体（指针+整数），后续调用只用传入电机序号即可
typedef struct
{
    TIM_HandleTypeDef *htim;
    uint32_t pwm_channel;
    GPIO_TypeDef *in1_port;
    uint16_t in1_pin;
    GPIO_TypeDef *in2_port;
    uint16_t in2_pin;
} DRV_TB6612Config_t;



static const DRV_TB6612Config_t tb6612[DRV_TB6612_CH_NUM] = {
    [DRV_TB6612_CH_FL] = {&htim5, TIM_CHANNEL_3, MOTOR_OUTC1_GPIO_Port,MOTOR_OUTC1_Pin,
                          MOTOR_OUTC2_GPIO_Port,MOTOR_OUTC2_Pin}, 
    [DRV_TB6612_CH_FR] = {&htim5, TIM_CHANNEL_2,MOTOR_OUTB2_GPIO_Port,MOTOR_OUTB2_Pin,
                          MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin},
    [DRV_TB6612_CH_RL] = {&htim5, TIM_CHANNEL_4,MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin,
                          MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin},
    [DRV_TB6612_CH_RR] = {&htim5, TIM_CHANNEL_1, MOTOR_OUTA2_GPIO_Port,  MOTOR_OUTA2_Pin ,
                          MOTOR_OUTA1_GPIO_Port,MOTOR_OUTA1_Pin},
};


/* ========== 内部判断输入通道是否存在========== */
static uint8_t DRV_TB6612_IsValidChannel(DRV_TB6612Channel_t channel)
{
    return channel < DRV_TB6612_CH_NUM;
}

/* ========== 内部判断输入角度是否存在========== */
static uint8_t DRV_TB6612_HasDirectionPins(const DRV_TB6612Config_t *config)
{
    return (config->in1_port != NULL) && (config->in2_port != NULL);
}

/* ========== 封装设置引脚电平的函数，简化代码========== */
static void DRV_TB6612_WritePins(const DRV_TB6612Config_t *config, GPIO_PinState in1, GPIO_PinState in2)
{
    HAL_GPIO_WritePin(config->in1_port, config->in1_pin, in1);
    HAL_GPIO_WritePin(config->in2_port, config->in2_pin, in2);
}


/* ========== 我的牢方法设置tb6612初始状态========== */
// int TB6612_Init(void)
// {   
//     static uint8_t is_initialized = 0;
//     if (is_initialized) {
//         return 0;  // 已经初始化过了，直接返回
//     }
    

//     /* 初始化状态变量 */
//     for (int i = 0; i < 4; i++)
//     {
//         g_motor_dir[i]   = TB6612_DIR_FORWARD;
//         g_motor_speed[i] = 0;
//     }


//     // 加入错误检查，确保 PWM 启动成功
//     if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_1) != HAL_OK)
//         return -1;   // 失败返回 -1
//     if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2) != HAL_OK)
//         return -1;
//     if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_3) != HAL_OK)
//         return -1;   // 失败返回 -1
//     if (HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_4) != HAL_OK)
//         return -1;
    

//     /* 先把所有电机速度置 0 */
//     TB6612_SetMotorSpeed(TB6612_MOTOR_A, 0);
//     TB6612_SetMotorSpeed(TB6612_MOTOR_B, 0);
//     TB6612_SetMotorSpeed(TB6612_MOTOR_C, 0);
//     TB6612_SetMotorSpeed(TB6612_MOTOR_D, 0);

//     /* 先把所有方向引脚置为停止状态 */
//     HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_RESET);

//     HAL_GPIO_WritePin(MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(MOTOR_OUTB2_GPIO_Port, MOTOR_OUTB2_Pin, GPIO_PIN_RESET);

//     HAL_GPIO_WritePin(MOTOR_OUTC1_GPIO_Port, MOTOR_OUTC1_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(MOTOR_OUTC2_GPIO_Port, MOTOR_OUTC2_Pin, GPIO_PIN_RESET);

//     HAL_GPIO_WritePin(MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin, GPIO_PIN_RESET);
//     HAL_GPIO_WritePin(MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin, GPIO_PIN_RESET);

//     is_initialized = 1;
//     return 0;         // 成功返回 0
// }

/* ========== 老师的新方法设置，直接调用HAL更快和便捷========== */
HAL_StatusTypeDef DRV_TB6612_Init(void)
{
    for (uint8_t i = 0; i < DRV_TB6612_CH_NUM; i++)
    {
        if (HAL_TIM_PWM_Start(tb6612[i].htim, tb6612[i].pwm_channel) != HAL_OK)
        {
            return HAL_ERROR;
        }

        __HAL_TIM_SET_COMPARE(tb6612[i].htim, tb6612[i].pwm_channel, 0u);

        if (DRV_TB6612_HasDirectionPins(&tb6612[i]))
        {
            DRV_TB6612_WritePins(&tb6612[i], GPIO_PIN_RESET, GPIO_PIN_RESET);
        }
    }

    return HAL_OK;
}

/* ========== 获取最大占空比，直接用HAL不用自己计算了========== */
uint16_t DRV_TB6612_GetMaxDuty(void)
{
    return (uint16_t)__HAL_TIM_GET_AUTORELOAD(&htim5);
}

/* ========== 我的牢方法设置电机旋转方向========== */

// // 定义设置电机方向的函数，第一个参数是电机ID（TB6612_MOTOR_X）,第二个参数是正反方向，方向由GPIO引脚的电平高低决定，后续如果转反了，就更改一下电平。
// void TB6612_SetMotorDir(TB6612_MotorId motor_id, TB6612_Direction dir)
// {
//     if (!TB6612_IsMotorIdValid(motor_id))
//         return;

//     g_motor_dir[motor_id] = dir;

//     switch (motor_id)
//     {
//     case TB6612_MOTOR_A:
//         if (dir == TB6612_DIR_FORWARD)
//         {
//             HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_SET);
//             HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_RESET);
//         }
//         else
//         {
//             HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_RESET);
//             HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_SET);
//         }
//         break;

//     case TB6612_MOTOR_B:
//         if (dir == TB6612_DIR_FORWARD)
//         {
//             HAL_GPIO_WritePin(MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin, GPIO_PIN_SET);
//             HAL_GPIO_WritePin(MOTOR_OUTB2_GPIO_Port, MOTOR_OUTB2_Pin, GPIO_PIN_RESET);
//         }
//         else
//         {
//             HAL_GPIO_WritePin(MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin, GPIO_PIN_RESET);
//             HAL_GPIO_WritePin(MOTOR_OUTB2_GPIO_Port, MOTOR_OUTB2_Pin, GPIO_PIN_SET);
//         }
//         break;

//     case TB6612_MOTOR_C:
//         if (dir == TB6612_DIR_FORWARD)
//         {
//             HAL_GPIO_WritePin(MOTOR_OUTC1_GPIO_Port, MOTOR_OUTC1_Pin, GPIO_PIN_SET);
//             HAL_GPIO_WritePin(MOTOR_OUTC2_GPIO_Port, MOTOR_OUTC2_Pin, GPIO_PIN_RESET);
//         }
//         else
//         {
//             HAL_GPIO_WritePin(MOTOR_OUTC1_GPIO_Port, MOTOR_OUTC1_Pin, GPIO_PIN_RESET);
//             HAL_GPIO_WritePin(MOTOR_OUTC2_GPIO_Port, MOTOR_OUTC2_Pin, GPIO_PIN_SET);
//         }
//         break;

//     case TB6612_MOTOR_D:
//         if (dir == TB6612_DIR_FORWARD)
//         {
//             HAL_GPIO_WritePin(MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin, GPIO_PIN_SET);
//             HAL_GPIO_WritePin(MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin, GPIO_PIN_RESET);
//         }
//         else
//         {
//             HAL_GPIO_WritePin(MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin, GPIO_PIN_RESET);
//             HAL_GPIO_WritePin(MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin, GPIO_PIN_SET);
//         }
//         break;

//     default:
//         break;
//     }
// }

/* ========== 老师的新方法设置电机旋转方向========== */
HAL_StatusTypeDef DRV_TB6612_SetDirection(DRV_TB6612Channel_t channel, DRV_TB6612Direction_t direction)
{
    if (!DRV_TB6612_IsValidChannel(channel) || !DRV_TB6612_HasDirectionPins(&tb6612[channel]))
    {
        return HAL_ERROR;
    }

    switch (direction)
    {
        case DRV_TB6612_DIR_FORWARD:
            DRV_TB6612_WritePins(&tb6612[channel], GPIO_PIN_SET, GPIO_PIN_RESET);
            break;

        case DRV_TB6612_DIR_REVERSE:
            DRV_TB6612_WritePins(&tb6612[channel], GPIO_PIN_RESET, GPIO_PIN_SET);
            break;

        case DRV_TB6612_DIR_BRAKE:
            DRV_TB6612_WritePins(&tb6612[channel], GPIO_PIN_SET, GPIO_PIN_SET);
            break;

        case DRV_TB6612_DIR_STOP:
        default:
            DRV_TB6612_WritePins(&tb6612[channel], GPIO_PIN_RESET, GPIO_PIN_RESET);
            break;
    }

    return HAL_OK;
}

/* ========== 我的牢方法设置电机转速========== */

// // 控制电机速度的函数定义

// void TB6612_SetMotorSpeed(TB6612_MotorId motor_id, int16_t speed)
// {
//     if (!TB6612_IsMotorIdValid(motor_id))
//         return;

//     if (speed < 0)
//         speed = 0;
//     if (speed > 1000)
//         speed = 1000;

//     g_motor_speed[motor_id] = speed;

//     // 把速度值转换成占空比，htim5.Init.Period是ARR（也就是总周期长度），pulse指脉冲宽度
//     uint32_t period = htim5.Init.Period + 1;  
//     uint32_t pulse = (uint32_t)speed * period / 1000U;

//     switch (motor_id)
//     {
//     case TB6612_MOTOR_A:
//         __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, pulse);
//         break;
//     case TB6612_MOTOR_B:
//         __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, pulse);
//         break;
//     case TB6612_MOTOR_C:
//         __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, pulse);
//         break;
//     case TB6612_MOTOR_D:
//         __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, pulse);
//         break;
//     default:
//         break;
//     }
// }

/* ========== 老师的新方法设置电机转速========== */
HAL_StatusTypeDef DRV_TB6612_SetDuty(DRV_TB6612Channel_t channel, uint16_t duty)
{
    uint16_t max_duty = DRV_TB6612_GetMaxDuty();

    if (!DRV_TB6612_IsValidChannel(channel))
    {
        return HAL_ERROR;
    }

    if (duty > max_duty)
    {
        duty = max_duty;
    }

    __HAL_TIM_SET_COMPARE(tb6612[channel].htim, tb6612[channel].pwm_channel, duty);

    return HAL_OK;
}



/* ========== 我的牢方法设置使能和失能========== */
// // 电机使能函数定义，先默认设置成300的初速度。
// void TB6612_EnableMotor(TB6612_MotorId motor_id)
// {
//     if (!TB6612_IsMotorIdValid(motor_id))
//         return;

//     TB6612_SetMotorSpeed(motor_id, 300);
// }

// // 电机失能函数定义，直接把速度设置为0，并且把方向引脚拉低。
// void TB6612_DisableMotor(TB6612_MotorId motor_id)
// {
//     if (!TB6612_IsMotorIdValid(motor_id))
//         return;

//     g_motor_speed[motor_id] = 0;

//     switch (motor_id)
//     {
//     case TB6612_MOTOR_A:
//         __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_1, 0);
//         HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_RESET);
//         break;

//     case TB6612_MOTOR_B:
//         __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_2, 0);
//         HAL_GPIO_WritePin(MOTOR_OUTB1_GPIO_Port, MOTOR_OUTB1_Pin, GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MOTOR_OUTB2_GPIO_Port, MOTOR_OUTB2_Pin, GPIO_PIN_RESET);
//         break;

//     case TB6612_MOTOR_C:
//         __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_3, 0);
//         HAL_GPIO_WritePin(MOTOR_OUTC1_GPIO_Port, MOTOR_OUTC1_Pin, GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MOTOR_OUTC2_GPIO_Port, MOTOR_OUTC2_Pin, GPIO_PIN_RESET);
//         break;

//     case TB6612_MOTOR_D:
//         __HAL_TIM_SET_COMPARE(&htim5, TIM_CHANNEL_4, 0);
//         HAL_GPIO_WritePin(MOTOR_OUTD1_GPIO_Port, MOTOR_OUTD1_Pin, GPIO_PIN_RESET);
//         HAL_GPIO_WritePin(MOTOR_OUTD2_GPIO_Port, MOTOR_OUTD2_Pin, GPIO_PIN_RESET);
//         break;

//     default:
//         break;
//     }
// }

/* ========== 老师的新方法设置失能========== */
HAL_StatusTypeDef DRV_TB6612_StopAll(void)
{
    for (uint8_t i = 0; i < DRV_TB6612_CH_NUM; i++)
    {
        if (DRV_TB6612_SetDuty((DRV_TB6612Channel_t)i, 0u) != HAL_OK)
        {
            return HAL_ERROR;
        }

        if (DRV_TB6612_HasDirectionPins(&tb6612[i]))
        {
            DRV_TB6612_WritePins(&tb6612[i], GPIO_PIN_RESET, GPIO_PIN_RESET);
        }
    }

    return HAL_OK;
}

