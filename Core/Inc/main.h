/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_signalOUT_Pin GPIO_PIN_13
#define LED_signalOUT_GPIO_Port GPIOC
#define PWMA_Pin GPIO_PIN_0
#define PWMA_GPIO_Port GPIOA
#define PWMB_Pin GPIO_PIN_1
#define PWMB_GPIO_Port GPIOA
#define PWMC_Pin GPIO_PIN_2
#define PWMC_GPIO_Port GPIOA
#define PWMD_Pin GPIO_PIN_3
#define PWMD_GPIO_Port GPIOA
#define ENCODER_B2_Pin GPIO_PIN_5
#define ENCODER_B2_GPIO_Port GPIOA
#define ENCODER_C2_Pin GPIO_PIN_6
#define ENCODER_C2_GPIO_Port GPIOA
#define ENCODER_C1_Pin GPIO_PIN_7
#define ENCODER_C1_GPIO_Port GPIOA
#define MOTOR_OUTA2_Pin GPIO_PIN_0
#define MOTOR_OUTA2_GPIO_Port GPIOB
#define MOTOR_OUTA1_Pin GPIO_PIN_1
#define MOTOR_OUTA1_GPIO_Port GPIOB
#define MOTOR_OUTB2_Pin GPIO_PIN_2
#define MOTOR_OUTB2_GPIO_Port GPIOB
#define MOTOR_OUTB1_Pin GPIO_PIN_10
#define MOTOR_OUTB1_GPIO_Port GPIOB
#define MOTOR_OUTC1_Pin GPIO_PIN_12
#define MOTOR_OUTC1_GPIO_Port GPIOB
#define MOTOR_OUTC2_Pin GPIO_PIN_13
#define MOTOR_OUTC2_GPIO_Port GPIOB
#define MOTOR_OUTD2_Pin GPIO_PIN_14
#define MOTOR_OUTD2_GPIO_Port GPIOB
#define MOTOR_OUTD1_Pin GPIO_PIN_15
#define MOTOR_OUTD1_GPIO_Port GPIOB
#define ENCODER_A1_Pin GPIO_PIN_8
#define ENCODER_A1_GPIO_Port GPIOA
#define ENCODER_A2_Pin GPIO_PIN_9
#define ENCODER_A2_GPIO_Port GPIOA
#define TX_Pin GPIO_PIN_11
#define TX_GPIO_Port GPIOA
#define RX_Pin GPIO_PIN_12
#define RX_GPIO_Port GPIOA
#define ENCODER_B1_Pin GPIO_PIN_3
#define ENCODER_B1_GPIO_Port GPIOB
#define ENCODER_D1_Pin GPIO_PIN_6
#define ENCODER_D1_GPIO_Port GPIOB
#define ENCODER_D1B7_Pin GPIO_PIN_7
#define ENCODER_D1B7_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
