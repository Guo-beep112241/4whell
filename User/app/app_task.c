// #include "app_task.h"
// #include "drv_tb6612.h"

// /* ========== 测试任务句柄 ========== */
// static osThreadId_t testMotorTaskHandle;

// /* ========== 测试任务：轮流测试 4 个电机 ========== */
// static void TestMotorTask(void *argument)
// {
//     TB6612_Init();
//     // Encoder_Init();
//         /* --- 测试电机 A：正转 1 秒，停 0.5 秒 --- */
//         TB6612_SetMotorDir(TB6612_MOTOR_A, TB6612_DIR_FORWARD);
//         TB6612_SetMotorSpeed(TB6612_MOTOR_A, 500);
//         osDelay(1000);
//         TB6612_DisableMotor(TB6612_MOTOR_A);
//         osDelay(500);

//         /* --- 测试电机 B --- */
//         TB6612_SetMotorDir(TB6612_MOTOR_B, TB6612_DIR_FORWARD);
//         TB6612_SetMotorSpeed(TB6612_MOTOR_B, 500);
//         osDelay(1000);
//         TB6612_DisableMotor(TB6612_MOTOR_B);
//         osDelay(500);

//         /* --- 测试电机 C --- */
//         TB6612_SetMotorDir(TB6612_MOTOR_C, TB6612_DIR_FORWARD);
//         TB6612_SetMotorSpeed(TB6612_MOTOR_C, 500);
//         osDelay(1000);
//         TB6612_DisableMotor(TB6612_MOTOR_C);
//         osDelay(500);

//         /* --- 测试电机 D --- */
//         TB6612_SetMotorDir(TB6612_MOTOR_D, TB6612_DIR_FORWARD);
//         TB6612_SetMotorSpeed(TB6612_MOTOR_D, 500);
//         osDelay(1000);
//         TB6612_DisableMotor(TB6612_MOTOR_D);
//         osDelay(500);

//         /* --- 4 个电机同时正转 1 秒 --- */
//         TB6612_SetMotorDir(TB6612_MOTOR_A, TB6612_DIR_FORWARD);
//         TB6612_SetMotorDir(TB6612_MOTOR_B, TB6612_DIR_FORWARD);
//         TB6612_SetMotorDir(TB6612_MOTOR_C, TB6612_DIR_FORWARD);
//         TB6612_SetMotorDir(TB6612_MOTOR_D, TB6612_DIR_FORWARD);

//         TB6612_SetMotorSpeed(TB6612_MOTOR_A, 400);
//         TB6612_SetMotorSpeed(TB6612_MOTOR_B, 400);
//         TB6612_SetMotorSpeed(TB6612_MOTOR_C, 400);
//         TB6612_SetMotorSpeed(TB6612_MOTOR_D, 400);
//         osDelay(1000);

//         /* 全部停下 */
//         TB6612_DisableMotor(TB6612_MOTOR_A);
//         TB6612_DisableMotor(TB6612_MOTOR_B);
//         TB6612_DisableMotor(TB6612_MOTOR_C);
//         TB6612_DisableMotor(TB6612_MOTOR_D);
//         osDelay(1000);

//     for (;;)
//     {



//         //     /* ===== 第2步：只测电机A，方向引脚交替翻转 ===== */
//         // /* A1=高, A2=低 → 正向 */
//         // HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_SET);
//         // HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_RESET);

//         // /* 给最大 PWM */
//         // TB6612_SetMotorSpeed(TB6612_MOTOR_A, 999);

//         // osDelay(2000);  /* 2 秒 */

//         // /* ===== 第3步：翻转方向 ===== */
//         // HAL_GPIO_WritePin(MOTOR_OUTA1_GPIO_Port, MOTOR_OUTA1_Pin, GPIO_PIN_RESET);
//         // HAL_GPIO_WritePin(MOTOR_OUTA2_GPIO_Port, MOTOR_OUTA2_Pin, GPIO_PIN_SET);

//         // osDelay(2000);  /* 2 秒 */


//     }
// }

// /* ========== 创建测试任务 ========== */
// void AppTask_Init(void *argument)
// {
//     const osThreadAttr_t testTask_attr = {
//         .name       = "testMotor",
//         .stack_size = 256 * 4,
//         .priority   = osPriorityNormal,
//     };

//     testMotorTaskHandle = osThreadNew(TestMotorTask, NULL, &testTask_attr);
// }





// // /* ========== 电机正反转定义测试 ========== */
// //    写在TestMotorTask中，不用TB662_Init(),直接初始化编码器就可以
// //         Encoder_Init();
        
// //         while(1)
// //         {
// //             int32_t enc_a = Encoder_GetCount(ENCODER_A);
// //             int32_t enc_b = Encoder_GetCount(ENCODER_B);
// //             int32_t enc_c = Encoder_GetCount(ENCODER_C);  
// //             int32_t enc_d = Encoder_GetCount(ENCODER_D);
            
// //             printf("A:%6ld  B:%6ld  C:%6ld  D:%6ld\r\n", enc_a, enc_b, enc_c, enc_d);
// //             // 手动顺时针转动电机，如果print出来的数值是增加的，说明正转的定义ok，如果反了的话————————咋整（？）
// //             osDelay(500);  // 500ms 打印一次
// //         }
