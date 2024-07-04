/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

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

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_WWDG_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define R_IS_ADC0_Pin GPIO_PIN_0
#define R_IS_ADC0_GPIO_Port GPIOA
#define L_IS_ADC1_Pin GPIO_PIN_1
#define L_IS_ADC1_GPIO_Port GPIOA
#define L_EN_PWM_Pin GPIO_PIN_4
#define L_EN_PWM_GPIO_Port GPIOA
#define LED_HEARTBEAT_Pin GPIO_PIN_5
#define LED_HEARTBEAT_GPIO_Port GPIOA
#define R_EN_PWM_Pin GPIO_PIN_6
#define R_EN_PWM_GPIO_Port GPIOA
#define L_PWM_Pin GPIO_PIN_7
#define L_PWM_GPIO_Port GPIOA
#define R_PWM_Pin GPIO_PIN_1
#define R_PWM_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
