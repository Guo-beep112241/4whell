// 头文件保护（防止重复包含）

#ifndef __DRV_TB6612_H__
#define __DRV_TB6612_H__

#ifdef __cplusplus
extern "C" {
#endif



#include <stddef.h>
//  包含main.h中的头文件，就不用再一个个包含了
#include "main.h"
#include "app_tuner.h"
 /*=====自己的version=====*/
// 定义电机ID枚举

// typedef enum
// {
//     TB6612_MOTOR_A = 0,
//     TB6612_MOTOR_B,
//     TB6612_MOTOR_C,
//     TB6612_MOTOR_D
// } TB6612_MotorId;


// // 定义电机旋转方向
// typedef enum
// {
//     TB6612_DIR_FORWARD = 0,
//     TB6612_DIR_BACKWARD
// } TB6612_Direction;

 /*=====参考例程改进后的version=====*/
//  给每一个电机根据在小车上的对应位置重新命名。

// #define PWMA_Pin GPIO_PIN_0
// #define PWMA_GPIO_Port GPIOA
#define RR_OUT1_Pin MOTOR_OUTA2_Pin 
#define RR_OUT1_GPIO_Port MOTOR_OUTA2_GPIO_Port 
#define RR_OUT2_Pin MOTOR_OUTA1_Pin 
#define RR_OUT2_GPIO_Port MOTOR_OUTA1_GPIO_Port 
// #define PWMB_Pin GPIO_PIN_1
// #define PWMB_GPIO_Port GPIOA
#define FR_OUT1_Pin MOTOR_OUTB2_Pin 
#define FR_OUT1_GPIO_Port MOTOR_OUTB2_GPIO_Port 
#define FR_OUT2_Pin MOTOR_OUTB1_Pin
#define FR_OUT2_GPIO_Port MOTOR_OUTB1_GPIO_Port 

// #define PWMC_Pin GPIO_PIN_2
// #define PWMC_GPIO_Port GPIOA
#define FL_OUT1_Pin MOTOR_OUTC1_Pin 
#define FL_OUT1_GPIO_Port MOTOR_OUTC1_GPIO_Port 
#define FL_OUT2_Pin MOTOR_OUTC2_Pin 
#define FL_OUT2_GPIO_Port MOTOR_OUTC2_GPIO_Port 
// #define PWMD_Pin GPIO_PIN_3
// #define PWMD_GPIO_Port GPIOA
#define RL_OUT1_Pin MOTOR_OUTD2_Pin 
#define RL_OUT1_GPIO_Port MOTOR_OUTD2_GPIO_Port 
#define RL_OUT2_Pin MOTOR_OUTD1_Pin 
#define RL_OUT2_GPIO_Port MOTOR_OUTD1_GPIO_Port 


// 对电机驱动用到的定时器通道进行统计
typedef enum
{
    DRV_TB6612_CH_FL = 0,
    DRV_TB6612_CH_FR,
    DRV_TB6612_CH_RL,
    DRV_TB6612_CH_RR,
    DRV_TB6612_CH_NUM
} DRV_TB6612Channel_t;


// 对电机驱动的方向进行统计
typedef enum
{
    DRV_TB6612_DIR_STOP = 0,
    DRV_TB6612_DIR_FORWARD,
    DRV_TB6612_DIR_REVERSE,
    DRV_TB6612_DIR_BRAKE
} DRV_TB6612Direction_t;

// // 声明对外接口函数，这些函数还没有被定义

// int TB6612_Init(void);
// void TB6612_SetMotorSpeed(TB6612_MotorId motor_id, int16_t speed);
// void TB6612_SetMotorDir(TB6612_MotorId motor_id, TB6612_Direction dir);
// void TB6612_EnableMotor(TB6612_MotorId motor_id);
// void TB6612_DisableMotor(TB6612_MotorId motor_id);

HAL_StatusTypeDef DRV_TB6612_Init(void);
HAL_StatusTypeDef DRV_TB6612_SetDirection(DRV_TB6612Channel_t channel, DRV_TB6612Direction_t direction);
HAL_StatusTypeDef DRV_TB6612_SetDuty(DRV_TB6612Channel_t channel, uint16_t duty);
HAL_StatusTypeDef DRV_TB6612_StopAll(void);
uint16_t DRV_TB6612_GetMaxDuty(void);

#ifdef __cplusplus
}
#endif

#endif
