/**
 @file Reboot.c
 @brief Used to initiate a reboot of the MCU
        Long term plan is to allow a reboot into DFU mode also - this is not yet implemented. 
*/
#include <stdbool.h>

#include "main.h"
#include "Reboot.h"

Reboot_Request_Type R = REBOOT_REQUEST_NONE;

/**
  * @brief  Reboot the processor
  *
  * @param  R: Type of request
  *            REBOOT_REQUEST_NORMAL = just reset and restart the application firmware, same like a normal power-on
  *            REBOOT_REQUEST_DFU = reboot into the DFU bootloader (not yet implemented, setting this has no effect).
  * @retval None
  */
void Reboot_Request(Reboot_Request_Type req)
{
    R = req;
}

/**
  * @brief  Main loop call - checks the value of the reboot request and will reboot when requested
  *         This initiates the watchdog timer, which will then time out and reset the processor  
  *
  * @param  None   
  * @retval None
  */
void Reboot_Main(void)
{
    if(R == REBOOT_REQUEST_NORMAL)
    {
        MX_WWDG_Init();	
        R = REBOOT_REQUEST_NONE;
    }
}
