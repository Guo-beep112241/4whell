// #ifndef __BSP_ENCODER_H__
// #define __BSP_ENCODER_H__

// #include "main.h"

// /* ========== 编码器 ID 枚举 ========== */
// typedef enum {
//     ENCODER_A = 0,  // 电机A，对应 TIM1
//     ENCODER_B,      // 电机B，对应 TIM2
//     ENCODER_C,      // 电机C，对应 TIM3
//     ENCODER_D       // 电机D，对应 TIM4
// } Encoder_Id;

// /* ========== 初始化：启动 4 路编码器 ========== */
// void Encoder_Init(void);

// /* ========== 读取当前累计脉冲数（带溢出处理） ========== */
// int32_t Encoder_GetCount(Encoder_Id encoder_id);

// /* ========== 计算转速，单位：RPM（每分钟转数） ========== */
// float Encoder_GetSpeedRPM(Encoder_Id encoder_id);

// /* ========== 周期调用此函数来更新速度（放在定时任务里每10ms调一次） ========== */
// void Encoder_Update(void);

// #endif
