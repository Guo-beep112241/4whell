// 头文件保护（防止重复包含）

#ifndef __DRV_TB6612_H__
#define __DRV_TB6612_H__

//  包含main.h中的头文件，就不用再一个个包含了
#include "main.h"

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
#define MOTOR_OUTA2_Pin GPIO_PIN_0  RR_OUT1_Pin
#define MOTOR_OUTA2_GPIO_Port GPIOB RR_OUT1_GPIO_Port
#define MOTOR_OUTA1_Pin GPIO_PIN_1  RR_OUT2_Pin
#define MOTOR_OUTA1_GPIO_Port GPIOB RR_OUT2_GPIO_Port

// #define PWMB_Pin GPIO_PIN_1
// #define PWMB_GPIO_Port GPIOA
#define MOTOR_OUTB2_Pin GPIO_PIN_2  FR_OUT1_Pin
#define MOTOR_OUTB2_GPIO_Port GPIOB FR_OUT1_GPIO_Port
#define MOTOR_OUTB1_Pin GPIO_PIN_10 FR_OUT2_Pin
#define MOTOR_OUTB1_GPIO_Port GPIOB FR_OUT2_GPIO_Port

// #define PWMC_Pin GPIO_PIN_2
// #define PWMC_GPIO_Port GPIOA
#define MOTOR_OUTC1_Pin GPIO_PIN_12 FL_OUT1_Pin
#define MOTOR_OUTC1_GPIO_Port GPIOB FL_OUT1_GPIO_Port
#define MOTOR_OUTC2_Pin GPIO_PIN_13 FL_OUT2_Pin
#define MOTOR_OUTC2_GPIO_Port GPIOB FL_OUT2_GPIO_Port

// #define PWMD_Pin GPIO_PIN_3
// #define PWMD_GPIO_Port GPIOA
#define MOTOR_OUTD2_Pin GPIO_PIN_14 RL_OUT1_Pin
#define MOTOR_OUTD2_GPIO_Port GPIOB RL_OUT1_GPIO_Port
#define MOTOR_OUTD1_Pin GPIO_PIN_15 RL_OUT2_Pin
#define MOTOR_OUTD1_GPIO_Port GPIOB RL_OUT2_GPIO_Port

// 声明对外接口函数，这些函数还没有被定义

int TB6612_Init(void);
void TB6612_SetMotorSpeed(TB6612_MotorId motor_id, int16_t speed);
void TB6612_SetMotorDir(TB6612_MotorId motor_id, TB6612_Direction dir);
void TB6612_EnableMotor(TB6612_MotorId motor_id);
void TB6612_DisableMotor(TB6612_MotorId motor_id);

#endif