/** @file      Comms_Defs.h
 * @brief      Brief for Comms_Defs.h
 * @details    Details for Comms_Defs.h
 */

#ifndef COMMS_DEFS_H_
#define COMMS_DEFS_H_

#include <stdint.h>
#include <stdio.h>

#define SOP_BYTE '{'            // Start of packet identifier
#define EOP_BYTE '}'            // End of packet identifier
#define PAYLOAD_BUF_SIZE 30     // How many bytes of storage do we allocate for transmit and receive payloads. This is dependent on the amount of data we will pass.  
#define BYTE_TIMEOUT_MS 500     // How many ms do we wait between bytes before assuming the other end of the comms has died

/**
  * @brief  Enum for available commands that we can process.  
  *         When adding or removing commands to the Comms_Command enum, make sure to also add/remove from the Active_Commands array
  *         Active_Commands is an array of all commands that are currently being used, and handles gaps in the enum values. 
  *
  */
typedef enum __attribute__((__packed__))
{
    COMMAND_FW_VER = 'F',         /// Read the current firmware version
    COMMAND_STATUS = 'S',         /// Read the current status of motors
    COMMAND_SET_OUTPUTS = 'O',    /// set PWM outputs
    COMMAND_REBOOT = 'R'          /// Reboot the device (turns outputs off) 
}Comms_Commands;

/**
  * @brief  Enum for response codes returned to the host after receiving a command 
  *
  */
typedef enum __attribute__((__packed__))
{
    RESP_ACK = 'A',             // Ack when we've processed a command 
    RESP_INV_PAYLOAD = 'P',     // The last payload that we were expecting was not correct
    RESP_INV_COMMAND = 'C'      // We don't support the command number we were givem
}Comms_Response_Codes;

/**
  * @brief  Enum for state machine to we know what byte we are currently expecting to receive 
  *
  */
typedef enum
{
    EXPECT_UNDEFINED,    // undefined value - use this until we get initialised properly
    EXPECT_SOP,          // Start of Packet 
    EXPECT_COMMAND,      // Commmand to be processed
    EXPECT_PAYLOAD,      // payload bytes
    EXPECT_EOP,          // End of Packet 
}Comms_RX_Expect;

/**
  * @brief  Payload struct containing buffer and length of valid data 
  *
  */
typedef struct __attribute__((__packed__))
{
    uint8_t Len;     // Length of the payload being received
    uint8_t Buf[PAYLOAD_BUF_SIZE];  // buffer to store the payload 
}Comms_Payload;

/**
  * @brief  Struct containing the packet that we want to send or have received. 
  *         FOr transmitting, bytes should be packed together since the memory address can be passed directly to the transmission function  
  *
  */
typedef struct __attribute__((__packed__))
{
    char SOP;                // The start of packet byte. Should always be SOP_BYTE   
    Comms_Commands Command;  // Command being received/transmitted
    Comms_Payload Payload;   // The payload being received.
    char EOP;                // end of packet. should always be the EOP_BYTE
}Comms_Packet;

/**
  * @brief  Object containing all data that we need to receive a packet and report its reception  
  *
  */
typedef struct
{
    Comms_RX_Expect Expect;      // What is the next byte we are expecting to receive
    uint16_t Running_Len;        // Temporary counter to count the received payload bytes
    void (*Packet_Ready)(Comms_Packet *Pkt);  // Callback gets called when a successfully received packet is ready for processing
    Comms_Packet Packet;         // Packet data being received
    uint8_t Byte_Timer;          // Timer to measure time between received bytes so we can drop incompletely received packets
}Comms_RX_Typedef;

#endif
