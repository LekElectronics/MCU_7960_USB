/**
 * @file Firmware_Version.c
 * @brief Define and return the firmware version  
 * 
 */
#include "Firmware_Version.h"

/**
   @brief  Defines the firmware version that is currently executing. 
   @details Change this as needed for a new release when new features 
            are added or removed, and bugs fixed.
   
   @param  none
  @retval null terminated string of the currently loaded firmware 
*/
#define PROJ_NAME "MCU_7960_USB"
#define PROJ_VERSION "0.1"

#define FW_VERSION PROJ_NAME "v" PROJ_VERSION

const char FW_Ver[] = FW_VERSION;

/**
  * @brief  Read and return the current firmware version string.  
  *
  * @param  none
  * @retval null terminated string of the currently loaded firmware 
  */
const char *Firmware_Version_Get(void)
{
    return FW_Ver;

}