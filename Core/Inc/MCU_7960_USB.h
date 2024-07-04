#ifndef MCU_7960_USB0_H_
#define MCU_7960_USB_H_

#include <stdbool.h>

void MCU_7960_USB_Initialise(void);
void MCU_7960_USB_Main(void);
void MCU_7960_USB_Timer_Interrupt(void);

#endif