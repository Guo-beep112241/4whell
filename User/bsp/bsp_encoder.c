// #include "bsp_encoder.h"
// #include "tim.h"

// // 编码器线数（每圈脉冲数*4倍频），每圈脉冲数由电机的编码器决定，需要调整！！
// #define ENCODER_PPR (100 * 4)   // FIXME: 这里的100是编码器的脉冲数，需要根据实际编码器调整

// /* ========== 静态变量：上次采样时的计数值 ========== */
// static int32_t encoder_last_count[4];   // 上一次读到的原始计数
// static int32_t encoder_current_count[4]; // 当前累积计数（带溢出处理）
// static float   encoder_speed_rpm[4];    // 计算出的转速

// /* ========== 更新间隔（秒），在 Encoder_Init 中设置 ========== */
// static float encoder_update_period = 0.01f;  // 默认 10ms


// // 初始化函数

// void Encoder_Init(void)
// {
//     // 启动 4 路编码器
//     HAL_TIM_Encoder_Start(&htim1, TIM_CHANNEL_ALL);  // 编码器 A
//     HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);  // 编码器 B
//     HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);  // 编码器 C
//     HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);  // 编码器 D

//     // 初始化静态变量
//     for (int i = 0; i < 4; i++)
//     {
//         encoder_last_count[i] = 0;
//         encoder_current_count[i] = 0;
//         encoder_speed_rpm[i] = 0.0f;
//     }
// }

// // /* ==========读取累计脉冲，处理16位溢出 ========== */

// // void Encoder_GetCount(Encoder_Id encoder_id)
// // {
// //     if(encoder_id < Encoder_A || encoder_id > Encoder_D)
// //         return 0;

// //     // 获取对应的定时器句柄
// //     TIM_HandleTypeDef *htim = NULL;
// //     switch(encoder_id) {
// //         case ENCODER_A: htim = &htim1; break;
// //         case ENCODER_B: htim = &htim2; break;
// //         case ENCODER_C: htim = &htim3; break;
// //         case ENCODER_D: htim = &htim4; break;
// //         default: return 0;
// //     }
    
// //     // 读取当前硬件计数器的值
// //     uint32_t current_raw = __HAL_TIM_GET_COUNTER(htim);
    
// //     // 判断定时器类型并计算总脉冲
// //     int32_t total_pulse;
    
// //     if (encoder_id == ENCODER_B) {
// //         // ===== TIM2: 32位定时器 =====
// //         // 直接读取，不需要处理溢出
// //         total_pulse = (int32_t)current_raw;
// //     } else {
// //         // ===== TIM1/3/4: 16位定时器 =====
// //         // 需要软件处理溢出：溢出次数 × 65536 + 当前值
// //         // 注意：encoder_current_count 需要在中断中更新
// //         total_pulse = encoder_current_count[encoder_id] + (int32_t)current_raw;
    
// //         return total_pulse;
// // }



// // /* ==========溢出中断 ========== */

// // /* ========== HAL 回调函数：处理定时器溢出中断 ========== */
// // void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
// // {
// //     // 判断是哪个定时器发生了溢出
// //     if (htim->Instance == TIM1) {
// //         // TIM1 溢出（16位）
// //         if (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) {
// //             encoder_current_count[ENCODER_A] -= 65536;  // 反转，向下溢出
// //         } else {
// //             encoder_current_count[ENCODER_A] += 65536;  // 正转，向上溢出
// //         }
// //     }
// //     else if (htim->Instance == TIM2) {
// //         // TIM2 是 32 位，理论上不会溢出，但为了保险也处理一下
// //         if (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) {
// //             encoder_current_count[ENCODER_B] -= 0x100000000ULL;  // 32位向下溢出
// //         } else {
// //             encoder_current_count[ENCODER_B] += 0x100000000ULL;  // 32位向上溢出
// //         }
// //     }
// //     else if (htim->Instance == TIM3) {
// //         // TIM3 溢出（16位）
// //         if (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) {
// //             encoder_current_count[ENCODER_C] -= 65536;
// //         } else {
// //             encoder_current_count[ENCODER_C] += 65536;
// //         }
// //     }
// //     else if (htim->Instance == TIM4) {
// //         // TIM4 溢出（16位）
// //         if (__HAL_TIM_IS_TIM_COUNTING_DOWN(htim)) {
// //             encoder_current_count[ENCODER_D] -= 65536;
// //         } else {
// //             encoder_current_count[ENCODER_D] += 65536;
// //         }
// //     }
// // }



