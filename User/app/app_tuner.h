#ifndef __APP_TUNER_H__
#define __APP_TUNER_H__

#ifdef __cplusplus
extern "C" {
#endif



// 宏定义
// 设定小车的转向pid
#define TURN_KP 0.0f
#define TURN_KI 0.0f
#define TURN_KD 0.0f
#define TURN_MAX_SPEED 3.14f //最大转向速度
#define TURN_INTEGRAL_LIMIT 1.0f   //转向速度积分限制，限制积分项防止角度环积分饱和，1为常用值


// 设定小车的速度pid
#define SPEED_KP 0.4f
#define SPEED_KI 0.0f
#define SPEED_KD 0.0f
#define SPEED_MAX_PERCENT 100.0f  //最大占空比
#define SPEED_INTEGRAL_LIMIT 1000.0f  //直线速度积分限制

// 调试时执行正转指令，若轮子正转则不变，若轮子反转则改为1u
#define MOTOR_FL_OUTPUT_REVERSE 1u
#define MOTOR_FR_OUTPUT_REVERSE 0u
#define MOTOR_RL_OUTPUT_REVERSE 1u
#define MOTOR_RR_OUTPUT_REVERSE 1u

// 调试时观察正转时编码器读数，若为正，则保持0u不变，若为负，改为1u
#define MOTOR_FL_ENCODER_REVERSE 0u
#define MOTOR_FR_ENCODER_REVERSE 0u
#define MOTOR_RL_ENCODER_REVERSE 0u
#define MOTOR_RR_ENCODER_REVERSE 0u



// 全局变量声明
// 转向pid
extern volatile float turn_kp;
extern volatile float turn_ki;
extern volatile float turn_kd;
extern volatile float turn_max_speed;
extern volatile float turn_integral_limit;

// 速度pid
extern volatile float speed_kp;
extern volatile float speed_ki;
extern volatile float speed_kd;
extern volatile float speed_max_percent;
extern volatile float speed_integral_limit;


#ifdef __cplusplus
}
#endif

#endif
