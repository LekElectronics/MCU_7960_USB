/**
  @file IO.c
  @brief Interface to low lever input and output pins and signals.
  @details IO pins and hardware blocks are defined and inigtialised using STM32CubeMX.
           Tnis module will then allow control (read/write) of IO pins.

  
 */
#include <IO.h>
#include <stdbool.h>
#include "main.h"

/**
  @brief  Definition of the digital IO Pins. These only have a basic on or off state, without any additional features.
          Control these with calls to IO_OP_High() or IO_OP_Low().
  @param  .Port: The port name that the pin exists on, eg GPIOA, GPIOB, etc
  @param  .Pin: The pin mask on the port where this pin is located. eg bit0 is Pin 1 << 0 = 1, bit3 is Pin 1 << 3 = 8.   
*/
typedef struct {
	GPIO_TypeDef *Port;
	uint16_t Pin;
}Output_Pin_Type;

/**
  @brief  Port and pin definitions for each IO pin. 
  @details An array containing the GPIO port and GPIO pin, used to easily call control functions
		   Names are as they are set in STM32CubeMX
           Make sure that the number of elements in the array matches the number of valid elements in the OUTPUT_Pin_Type enum.  
*/
Output_Pin_Type Output_Pins[NUM_OUTPUT_PINS] = {
	{.Port = LED_HEARTBEAT_GPIO_Port, .Pin = LED_HEARTBEAT_Pin}
};

/**
  * @brief  Definition of the PWM-controlled output Pins. The timer and channel
  *         Control these with calls to IO_OP_High() or IO_OP_Low().  
  * @param  .Timer: the extern defined handle to the timer. eg htim1, htim2, etc
  * @param  .Channel: The channel of the timer. eg TIM_CHANNEL_1, TIM_CHANNEL_2
  */
typedef struct {
	TIM_HandleTypeDef *Timer;
	uint32_t Channel;
}PWM_Pin_Type;

extern TIM_HandleTypeDef htim3;		/// timer3 is used to generate PWM signals. This is set up in STM32CubeMX
extern TIM_HandleTypeDef htim14;	/// tim14 is used to generate PWM signals. This is set up in STM32CubeMX  

/**
  @brief Timer and chanel definitions for each PWM controlled output pin. Make sure that the number of elements in the array matches the number of valid elements in the PWM_Pin_Type enum.  
         Timers and channels are set up in STM32CubeMX. 
*/
const PWM_Pin_Type PWM_Pins[NUM_PWM_PINS] = {
	{&htim14, TIM_CHANNEL_1},
	{&htim3,  TIM_CHANNEL_1},
	{&htim3,  TIM_CHANNEL_2},
	{&htim3,  TIM_CHANNEL_4}
};

/**
  * @brief  Set the output pin to logic high state. If pin is not in OUTPUT_PIN enum then no action is performed  
  * @param  pin: the pin to be set high
  * @retval none
  */
void IO_Set_OP_High(OUTPUT_PIN pin)
{
	if(pin < NUM_OUTPUT_PINS)
	{
		HAL_GPIO_WritePin(Output_Pins[pin].Port, Output_Pins[pin].Pin, GPIO_PIN_SET);
	}
}
/**
  * @brief  Set the output pin to logic low state. If pin is not in OUTPUT_PIN enum then no action is performed  
  * @param  pin: the pin to be set low
  * @retval none
  */
void IO_Set_OP_Low(OUTPUT_PIN pin)
{
	if(pin < NUM_OUTPUT_PINS)
	{
		HAL_GPIO_WritePin(Output_Pins[pin].Port, Output_Pins[pin].Pin, GPIO_PIN_RESET);
	}
}

/**
  @brief  Not yet implemented
  		  Initialise the ADC by performing calibration. 
          This should be done at least once at power-on.
  @param  none   
  @retval none
*/
void ADC_Initialise(void)
{


}

/**
  @brief  Initialise the IO components that are not covered by the STM32 Cube MX. 
		  Call this during system initialisation.
  @param  none   
  @retval none
*/
void IO_Initialise(void)
{
	ADC_Initialise();


}

/**
  * @brief  Set the output pin pwm percent to the value specified. If pin is not in PWM_PIN enum then no action is performed.
  *         Any Value greater than 100 is set to 100%.
  *         PWM pins are controlled by the timer blocks (configured in PWM mode) attached to the physical pins. 
  * @param  Value_Percent: A whole value between 0 and 100 (inclusive). 
  *                        0% is equivalent to the the output being off (always low) 
  *                        100% is equivalent to the the output being on (always high) 
  * @param  pwm: the pwm capable pin to apply the signal to.   
  * @retval none
  */
void IO_Set_PWM_Percent(uint8_t Value_Percent, PWM_PIN pwm)
{
	static bool Already_Initialised[NUM_PWM_PINS] = {false, false, false, false};

	TIM_HandleTypeDef *This_Timer;
	uint32_t Timer_Channel = -1;
	TIM_OC_InitTypeDef sConfigOC;

	if(Value_Percent > 100)
	{
		Value_Percent = 100;
	}
	if(Value_Percent < 0)
	{
		Value_Percent = 0;
	}

	if(pwm < NUM_PWM_PINS)
	{
		sConfigOC.OCMode = TIM_OCMODE_PWM1;
		sConfigOC.Pulse = 0;
		sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
		sConfigOC.OCNPolarity = TIM_OCNPOLARITY_LOW;
		sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
		sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
		sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;

		This_Timer = PWM_Pins[pwm].Timer;
		Timer_Channel = PWM_Pins[pwm].Channel;

		float Reg_Val = This_Timer->Instance->ARR+1;
		Reg_Val *= Value_Percent;
		Reg_Val /= 100;

		if(Already_Initialised[pwm] == false)
		{
			HAL_TIM_PWM_Stop(This_Timer, Timer_Channel);
			sConfigOC.Pulse = (uint32_t)Reg_Val;
			if (HAL_TIM_PWM_ConfigChannel(This_Timer, &sConfigOC, Timer_Channel) != HAL_OK)
			{
				Error_Handler();
			}
			HAL_TIM_PWM_Start(This_Timer, Timer_Channel);	
			Already_Initialised[pwm] = true;
		}
		else
		{
			__HAL_TIM_SET_COMPARE(This_Timer, Timer_Channel, Reg_Val);
			HAL_TIM_PWM_Start(This_Timer, Timer_Channel);	
		}
	}
}

/**
  * @brief  Get the current pwm percent being applied to the specified pin. If pin is not in PWM_PIN enum then no action is performed and 0 is returned.
  *         PWM value is calculated by reading the current timer pulse value and ARR value, and calculating the duty from these.
  * @param  Value_Percent: A value between 0 and 100 (inclusive). 
  *                        0% is equivalent to the the output being off (always low) 
  *                        100% is equivalent to the the output being on (always high) 
  * @retval none
  */
uint8_t IO_Get_PWM_Percent(PWM_PIN pwm)
{

	if(pwm < NUM_PWM_PINS)
	{
		TIM_HandleTypeDef *This_Timer = PWM_Pins[pwm].Timer;
		uint32_t Timer_Channel = PWM_Pins[pwm].Channel;

		uint32_t Compare = __HAL_TIM_GET_COMPARE(This_Timer, Timer_Channel);
		uint32_t ARR = __HAL_TIM_GET_AUTORELOAD(This_Timer);

		return (uint8_t)(( ((float)Compare)/ARR) * 100);		
	}

	return 0;

}

/**
  * @brief  Not yet implemented
  * 		Get the current ADC value of the specified pin. 
  * @param  pin: the ADC_PIN to read. Any pin outside of the ADC_PIN enum will return 0  
  * @retval none
  */
uint16_t IO_Get_ADC(ADC_PIN pin)
{
	return 0;
}
