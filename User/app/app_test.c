#include "app_test.h"
#include "drv_tb6612.h"

/* ========== 测试用占空比（50% = 4200/8399） ========== */
#define TEST_DUTY  4200U
#define TEST_DELAY_ON   1000U   /* 电机转动时长 ms */
#define TEST_DELAY_OFF   500U   /* 电机停止间隔 ms */

/**
 * @brief 电机测试任务：轮流测试4个电机，再同时转动
 */
void AppTest_Task(void *argument)
{
    (void)argument;  /* 未使用 */

    /* ---------- 初始化 TB6612 驱动 ---------- */
    // if (DRV_TB6612_Init() != HAL_OK)
    // {
    //     /* 初始化失败，闪烁 LED 报警后死循环 */
    //     for (;;)
    //     {
    //         HAL_GPIO_TogglePin(LED_signalOUT_GPIO_Port, LED_signalOUT_Pin);
    //         osDelay(200);
    //     }
    // }
    DRV_TB6612_Init();
    /* 确认一下最大占空比值（调试用，实际不输出） */
    (void)DRV_TB6612_GetMaxDuty();

    for (;;)
    {
        /* ===== 第1轮：逐个电机测试 ===== */

        /* --- FL（前左）正转 --- */
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_FL, DRV_TB6612_DIR_FORWARD);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_FL, TEST_DUTY);
        // //HAL_Delay(TEST_DELAY_ON);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_FL, 0);
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_FL, DRV_TB6612_DIR_STOP);
        // //HAL_Delay(TEST_DELAY_OFF);

        // /* --- FR（前右）正转 --- */
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_FR, DRV_TB6612_DIR_FORWARD);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_FR, TEST_DUTY);
        // //HAL_Delay(TEST_DELAY_ON);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_FR, 0);
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_FR, DRV_TB6612_DIR_STOP);
        // //HAL_Delay(TEST_DELAY_OFF);

        // /* --- RL（后左）正转 --- */
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_RL, DRV_TB6612_DIR_FORWARD);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_RL, TEST_DUTY);
        // //HAL_Delay(TEST_DELAY_ON);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_RL, 0);
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_RL, DRV_TB6612_DIR_STOP);
        // //HAL_Delay(TEST_DELAY_OFF);

        // /* --- RR（后右）正转 --- */
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_RR, DRV_TB6612_DIR_FORWARD);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_RR, TEST_DUTY);
        // //HAL_Delay(TEST_DELAY_ON);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_RR, 0);
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_RR, DRV_TB6612_DIR_STOP);
        // //HAL_Delay(TEST_DELAY_OFF);

        // /* ===== 第2轮：逐个电机反转 ===== */

        // /* --- FL 反转 --- */
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_FL, DRV_TB6612_DIR_REVERSE);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_FL, TEST_DUTY);
        // HAL_Delay(TEST_DELAY_ON);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_FL, 0);
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_FL, DRV_TB6612_DIR_STOP);
        // HAL_Delay(TEST_DELAY_OFF);

        // /* --- FR 反转 --- */
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_FR, DRV_TB6612_DIR_REVERSE);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_FR, TEST_DUTY);
        // //HAL_Delay(TEST_DELAY_ON);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_FR, 0);
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_FR, DRV_TB6612_DIR_STOP);
        // //HAL_Delay(TEST_DELAY_OFF);

        // /* --- RL 反转 --- */
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_RL, DRV_TB6612_DIR_REVERSE);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_RL, TEST_DUTY);
        // // HAL_Delay(TEST_DELAY_ON);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_RL, 0);
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_RL, DRV_TB6612_DIR_STOP);
        // //HAL_Delay(TEST_DELAY_OFF);

        // /* --- RR 反转 --- */
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_RR, DRV_TB6612_DIR_REVERSE);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_RR, TEST_DUTY);
        // //HAL_Delay(TEST_DELAY_ON);
        // DRV_TB6612_SetDuty(DRV_TB6612_CH_RR, 0);
        // DRV_TB6612_SetDirection(DRV_TB6612_CH_RR, DRV_TB6612_DIR_STOP);
        // //HAL_Delay(TEST_DELAY_OFF);

        /* ===== 第3轮：4个电机同时正转 ===== */
        DRV_TB6612_SetDirection(DRV_TB6612_CH_FL, DRV_TB6612_DIR_FORWARD);
        DRV_TB6612_SetDirection(DRV_TB6612_CH_FR, DRV_TB6612_DIR_FORWARD);
        DRV_TB6612_SetDirection(DRV_TB6612_CH_RL, DRV_TB6612_DIR_FORWARD);
        DRV_TB6612_SetDirection(DRV_TB6612_CH_RR, DRV_TB6612_DIR_FORWARD);

        DRV_TB6612_SetDuty(DRV_TB6612_CH_FL, TEST_DUTY);
        DRV_TB6612_SetDuty(DRV_TB6612_CH_FR, TEST_DUTY);
        DRV_TB6612_SetDuty(DRV_TB6612_CH_RL, TEST_DUTY);
        DRV_TB6612_SetDuty(DRV_TB6612_CH_RR, TEST_DUTY);
        // HAL_Delay(2000);  /* 同时转2秒 */

        // /* 全部停止 */
        // DRV_TB6612_StopAll();
        // HAL_Delay(2000);  /* 停2秒后再循环 */
    }
}
