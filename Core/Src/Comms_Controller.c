/**
  @file Comms_Controller.c
  @brief A manager and interface for the communication channel to the USB host. 
  @details How to use:
        1. Call Comms_Controller_Reset_USB() after MX_GPIO_Init() and before MX_USB_DEVICE_Init() USB. This will pulse the USB DP+ pin low to simulate 
           a device being plugged into the USB host which will trigger device enumeration.
        2. Call Comms_Controller_Initialise(void) during system initialisation.
        3. Call Comms_Controller_Bytes_Received(uint8_t *Buf, uint32_t Num_Bytes) when data is received from the host. This will process each byte received.
        4. During processing of bytes, once a valid command packet is detected Packet_Received(Comms_Packet *Pkt) will be called containing the packet.  
           The packet will then be processed, any commands executed, and a reply sent back to the host.
        5. Ensure Comms_Controller_Timer_Interrupt() is called periodically via a timer interrupt. This will be used to detect coms timeouts.
 
        Ensure that packets are sent with time between bytes less than BYTE_TIMEOUT_MS otherwise the packet will be dropped. 

        <B>Communications</B> 
        Packet Structure: 

        <SOP_BYTE><Command><Data Payload[0]>..<Data Payload[n]><EOP_BYTE><'\\n'><'\\r'>
        
        Where:

        SOP_BYTE = byte to indicate the start of a packet.

        Command = What command to execute. One of the values in Comms_Commands.

        Data_Payload[] = Any applicable data that may be included with this packet. This can be empty if teh command does not need any additional data.

        EOP_BYTE = byte to indicate the end of a packet

        <'\\n'><'\\r'> = optional bytes to help humans to read in a terminal. It is not necessary to include these bytes, and they should not be relied upon to be sent. 

        Commms Method: 
        
        Communications occur on a polled basis. The MCU running this code will only send a packet in reply to a command received from some host controller. 



 */

#include <stdbool.h>
#include <string.h>

#include "Command.h"
#include "Comms_Controller.h"
#include "Comms_Defs.h"
#include "Comms_RX.h"

#include "usbd_cdc_if.h"

Comms_RX_Typedef RX = {.Expect=EXPECT_UNDEFINED};       /// Local object to save the data reception. Initialised with unexted until this module is properly initialised.  
const Comms_Commands Active_Commands[] = {COMMAND_FW_VER, COMMAND_STATUS, COMMAND_SET_OUTPUTS, COMMAND_REBOOT};   /// An array of all commands, used to easily check if a received command is valid   


/**
  * @brief  Check is the command received is active and valid. 
  *         Active_Commands is an array containing a list of active commands. 
  *         Search through this array to return true if cmd exists in the Active_Commands array. 
  *
  * @param  cmd: command number being searched for
  * @retval true if cmd is active,  false if cmd is not found 
  */
bool Is_Command_Valid(Comms_Commands Cmd)
{
    for(uint8_t c = 0; c < sizeof(Active_Commands)-1; c++)
    {   
        if(Active_Commands[c] == Cmd)
        {
            return true;    // cmd has been found in the active command list
        }
    }   

    return false;   // we searched the active commands but didn't find a match 
}

/**
  * @brief  Form a packet from the provided command and data, then sent it out the comms channel  
  *
  * @param  Cmd: command number being sent
  * @param  Dat: Payload to be sent
  * @retval none 
  */ 
void Send_Packet(Comms_Commands Cmd, Comms_Payload *Dat)
{
	static uint8_t Buf[PAYLOAD_BUF_SIZE+5];     // add 3 for the SOP, CMD, EOP and CRLF bytes 
	Buf[0] = SOP_BYTE;
	Buf[1] = Cmd;
  if(Dat->Len > PAYLOAD_BUF_SIZE)
  {   // dont overflow our buffer. If Dat is greater than the payload buffer then truncate it
      Dat->Len = PAYLOAD_BUF_SIZE;
  }
	memcpy(&Buf[2], Dat->Buf, Dat->Len);
	Buf[2+Dat->Len] = EOP_BYTE;
	Buf[3+Dat->Len] = '\n';   /// CRLF is not neccessary but is more human-readable. Helps when testing using a terminal.  
	Buf[4+Dat->Len] = '\r';

	CDC_Transmit_FS(Buf, (uint16_t)Dat->Len+5);   // +5 for SOP, CMD, EOP, CR, LF;
}

/**
   @brief  Process a received packet, execute any commands, and return a reply back to the host. 
           SOP and EOP should have already been checked before calling this function. 
  
   @param  Pkt: Packet to be processed
   @retval None
  */
void Packet_Received(Comms_Packet *Pkt)
{
    Comms_Payload Reply;

    Reply = *Command_Execute(Pkt->Command, Pkt->Payload);
    if(Reply.Len > 0)
    {
        Send_Packet(Pkt->Command, &Reply);
    }
}

/**
  * @brief  Initialise the comms controller module and dependencies. Call this once during power-on init. 
  *
  * @retval None
  */
void Comms_Controller_Initialise(void)
{
    Comms_RX_Initialise(&RX, Packet_Received);
}


/**
  * @brief  Pulse the USB D+ low to trigger the host to reenumerate the USB device. 
  *         This helps a lot especially during debugging/restarting  
	*         Call this after MX_GPIO_Init() and before MX_USB_DEVICE_Init() ()
  *
  * @param  None
  * @retval None
  */
void Comms_Controller_Reset_USB(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, RESET);
	HAL_Delay(500);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12, SET);
}

/**
  * @brief  Data received by USB can be sent to this function to start processing 
  *
  * @param  Buf: Buffer of data that have been received
  * @param  Len: Number of data bytes received
  * @retval None
  */
void Comms_Controller_Bytes_Received(uint8_t *Buf, uint32_t Num_Bytes)
{
    // can we add the received bytes to the existing buffer being processed 
    for(int i=0; i<Num_Bytes; i++)
    {
        Comms_RX_Receive_Byte(&RX, Buf[i]); 
    }
}

/**
  * @brief  Periodic timer processing. Call this from the timer interrupt 
  *
  * @param  None
  * @retval None
  */
void Comms_Controller_Timer_Interrupt(void)
{
  Comms_RX_Timer(&RX);
}
