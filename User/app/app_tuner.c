#include "app_tuner.h"

// 全局变量定义
// 转向pid
volatile float turn_kp = TURN_KP;
volatile float turn_ki = TURN_KI;
volatile float turn_kd = TURN_KD;
volatile float turn_max_speed = TURN_MAX_SPEED;
volatile float turn_integral_limit = TURN_INTEGRAL_LIMIT;

// 速度pid
volatile float speed_kp = SPEED_KP;
volatile float speed_ki = SPEED_KI;
volatile float speed_kd = SPEED_KD;
volatile float speed_max_percent = SPEED_MAX_PERCENT;
volatile float speed_integral_limit = SPEED_INTEGRAL_LIMIT;
