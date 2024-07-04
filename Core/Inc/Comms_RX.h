/** @file      Comms_RX.h
 * @brief      Brief for Comms_RX.h
 * @details    Details for Comms_RX.h
 */

#ifndef COMMS_RX_H_
#define COMMS_RX_H_

#include <stdint.h>

#include "Comms_Defs.h"

void Comms_RX_Initialise(Comms_RX_Typedef *RX, void *PacketReadyCB);
void Comms_RX_Receive_Byte(Comms_RX_Typedef *RX, uint8_t This_Byte);
void Comms_RX_Timer(Comms_RX_Typedef *RX);


#endif
