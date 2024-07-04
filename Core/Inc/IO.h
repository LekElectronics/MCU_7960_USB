#ifndef IO_H_
#define IO_H_

#include <stdint.h>

typedef enum 
{
    ENA_L,
    ENA_R,
    PWM_L,
    PWM_R,
    NUM_PWM_PINS
}PWM_PIN;

typedef enum
{
    LED,
    NUM_OUTPUT_PINS
}OUTPUT_PIN;

typedef enum 
{
    ISENSE_L,
    ISENSE_R,
    NUM_ADC_PINS
}ADC_PIN;

uint16_t IO_Get_ADC(ADC_PIN pin);
void IO_Initialise(void);
uint8_t IO_Get_PWM_Percent(PWM_PIN pin);
void IO_Set_OP_High(OUTPUT_PIN pin);
void IO_Set_OP_Low(OUTPUT_PIN pin);
void IO_Set_PWM_Percent(uint8_t Value_Percent, PWM_PIN pin);


#endif
