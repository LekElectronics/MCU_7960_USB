/** @file      Comms_Controller.h
 * @brief      Brief for Comms_Controller.h
 * @details    Details for Comms_Controller.h
 */
#ifndef COMMS_CONTROLLER_H_
#define COMMS_CONTROLLER_H_

#include <stdint.h>

void Comms_Controller_Bytes_Received(uint8_t *Buf, uint32_t Num_Bytes);
void Comms_Controller_Initialise(void);
void Comms_Controller_Reset_USB(void);
void Comms_Controller_Timer_Interrupt(void);

#endif
