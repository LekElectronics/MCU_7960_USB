/** @file      Command.h
 * @brief      Brief for Command.h
 * @details    Details for Command.h
 */
#ifndef COMMAND_H_
#define COMMAND_H_

#include "Comms_Defs.h"

Comms_Payload *Command_Execute(Comms_Commands Cmd, Comms_Payload Payload);

#endif
