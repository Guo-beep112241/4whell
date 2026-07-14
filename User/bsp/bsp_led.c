#include "bsp_led.h"

void BSP_LED_On(void)
{
    HAL_GPIO_WritePin(LED_signalOUT_GPIO_Port, LED_signalOUT_Pin, GPIO_PIN_RESET);
}

void BSP_LED_Off(void)
{
    HAL_GPIO_WritePin(LED_signalOUT_GPIO_Port, LED_signalOUT_Pin, GPIO_PIN_SET);
}

void BSP_LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_signalOUT_GPIO_Port, LED_signalOUT_Pin);
}

void BSP_LED_Set(GPIO_PinState state)
{
    HAL_GPIO_WritePin(LED_signalOUT_GPIO_Port, LED_signalOUT_Pin, state);
}
