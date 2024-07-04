/**
 * @file LED.c
 * @brief Control of the onboard heartbeat/status LED
 * 
 */
#include "LED.h"

#include "main.h"

/**
 * @brief Turn the status LED off
 * 
 */
void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_HEARTBEAT_GPIO_Port, LED_HEARTBEAT_Pin, 1);
}

/**
 * @brief Turn the status LED ON
 * 
 */
void LED_On(void)
{
    HAL_GPIO_WritePin(LED_HEARTBEAT_GPIO_Port, LED_HEARTBEAT_Pin, 0);
}

/**
 * @brief Toggle the state of the status LED
 * 
 */
void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_HEARTBEAT_GPIO_Port, LED_HEARTBEAT_Pin);
}
