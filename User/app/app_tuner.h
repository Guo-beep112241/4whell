// 宏定义
// 调试时执行正转指令，若轮子正转则不变，若轮子反转则改为1u
#define MOTOR_FL_OUTPUT_REVERSE 0u
#define MOTOR_FR_OUTPUT_REVERSE 0u
#define MOTOR_RL_OUTPUT_REVERSE 0u
#define MOTOR_RR_OUTPUT_REVERSE 0u

// 调试时观察正转时编码器读数，若为正，则保持0u不变，若为负，改为1u
#define MOTOR_FL_ENCODER_REVERSE 0u
#define MOTOR_FR_ENCODER_REVERSE 0u
#define MOTOR_RL_ENCODER_REVERSE 0u
#define MOTOR_RR_ENCODER_REVERSE 0u