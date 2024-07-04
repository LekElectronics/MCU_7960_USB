/**
 @file MCU_7960_USB.c
 @brief Main entry point to the application application (after CubeMX initialisation has been done)
        This allows for cleaner seperation from the CubeMX code and may be easier to manage.
  
*/
#include "Clock.h"
#include "Comms_Controller.h"
#include "IO.h"
#include "LED.h"
#include "main.h"
#include "Reboot.h"
#include "MCU_7960_USB.h"


/**
 @brief Application initialisation.
        Call this after CubeMX initialisation has been done
 
*/
void MCU_7960_USB_Initialise(void)
{
    Clock_Calc_Timer_ms();
    IO_Initialise();
    Comms_Controller_Initialise();
}

/**
 @brief The main application loop.
        Call this from within the main() loop created by CubeMX.This will get executed every loop of main()
 
*/
void MCU_7960_USB_Main(void)
{
    LED_Toggle();
    Reboot_Main();
    HAL_Delay(250);
}

/**
 @brief Application general purpose timer interrupt.
        Call this from a general timer. It should be the same timer interrupt that is referenced in Clock.c
        Any functions that require periodic processing can be placed in this interrupt routine. 
        The timer should already be started in the main initialisation (after MCU_7960_USB_Initialise() has finished). 
*/
void MCU_7960_USB_Timer_Interrupt(void)
{
    Comms_Controller_Timer_Interrupt();
}
