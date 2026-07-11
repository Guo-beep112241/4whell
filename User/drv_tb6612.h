// 头文件保护（防止重复包含）

#ifndef __DRV_TB6612_H__
#define __DRV_TB6612_H__

//  包含main.h中的头文件，就不用再一个个包含了
#include "main.h"


// 定义电机ID枚举
typedef enum
{
    TB6612_MOTOR_A = 0,
    TB6612_MOTOR_B,
    TB6612_MOTOR_C,
    TB6612_MOTOR_D
} TB6612_MotorId;


// 定义电机旋转方向
typedef enum
{
    TB6612_DIR_FORWARD = 0,
    TB6612_DIR_BACKWARD
} TB6612_Direction;


// 声明对外接口函数，这些函数还没有被定义

void TB6612_Init(void);
void TB6612_SetMotorSpeed(TB6612_MotorId motor_id, int16_t speed);
void TB6612_SetMotorDir(TB6612_MotorId motor_id, TB6612_Direction dir);
void TB6612_EnableMotor(TB6612_MotorId motor_id);
void TB6612_DisableMotor(TB6612_MotorId motor_id);

#endif