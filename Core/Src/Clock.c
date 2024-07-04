/**
 @file Clock.c
 @brief Calculates the number of milliseconds per timer interrupt interval.
 @details TIMER_USED_HANDLE timer should be generating periodic interrupts, and these are used to measure elapsed time.   
        Clock_Calc_Timer_ms() should be called during initialisation. This will calculate the milliseconds per timer interrupt.
        Clock_Get_Timer_ms() can then be called afterwards to retrieve this value.
        Example: Our process wants to measure when 15ms has elapsed. It increments a counter every timer interrupt. 
        It will need to wait until the counter reaches a value of (15ms/CLock_Get_Timer_ms())      
 */

#include "main.h"
#include "stm32f0xx_ll_tim.h"

#define TIMER_USED_HANDLE htim3   /// The timer being used to measure the elapsed time

extern TIM_HandleTypeDef TIMER_USED_HANDLE;     // tim3 is used as the timer interrups

float Timer_ms = 1;


/**
  * @brief  Get the precalculated number of ms per timer interval. 
  *         This should only be called after calling Clock_Calc_Timer_ms() once during initialisation.
  * @retval Number of ms per timer interrupt
  */
float Clock_Get_Timer_ms(void)
{
    return Timer_ms;
}

/**
  * @brief  Calculate the number of ms per timer interval. 
  *         This should only be called once for efficiency, it will read registers and calculate the timer value.
  *         After this initialisation, the value can be read by calling Clock_Get_Timer_ms() 
  * @retval None
  */
void Clock_Calc_Timer_ms(void)
{
    uint32_t Sys_Freq = HAL_RCC_GetSysClockFreq();  // example 48,000,000
    float f = (float)Sys_Freq / (LL_TIM_GetPrescaler(TIMER_USED_HANDLE.Instance) +1); // example 48000000 / (47+1) = 1,000,000
    f /= LL_TIM_GetAutoReload(TIMER_USED_HANDLE.Instance);  // example 1,000,000 / 1000 = 1000ms
    f /= 1000;   // example 1000 / 1000 = 1.0

    Timer_ms = f;
}
