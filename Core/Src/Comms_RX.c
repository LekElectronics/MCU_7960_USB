/**
 * @file Comms_RX.c
  @brief Receive each byte from host, process it searching for a formed packet.
         Once a formed packet is received, it is passed then Packet_Ready_CB() is called with the packet details.
  @details
  How to use: 
   1. Initialise with a call to Comms_RX_Initialise()
   2. Call Comms_RX_Timer() from a periodic timer interrupt
   3. Call Comms_RX_Receive_Byte() for each byte received. 

  Process a Packet reception in the format:
  {<CMD><PAYLOAD>}

  Where:
  { is the start of packet indicator. Fixed value of 0x7B.
  <CMD> is the command letter.
  <PAYLOAD> is astring of payload bytes.
  } is the end of the packet. Fixed value of 0x7D

  Example are:
    {F} to request the firmware version
    {O...(to fill)} to set the motor outputs
    {S} to request the status 
*/

#include <stdbool.h>

#include "Clock.h" 
#include "Comms_Defs.h"
#include "main.h"


uint16_t Byte_Timeout_Ints;  // How many timer interrupts do we wait between receiving bytes of a packet before we time out. 


/**
  * @brief  Initialise the RX module ready to receive a new packet.  
  *
  * @param  RX: The main RX object for this channel reception 
  * @param  Packet_Ready_CB: Function to call when a full valid packet has been received. Pass a parameter of (Comms_Packet *Pkt)  
  * @retval None
  */
void Comms_RX_Initialise(Comms_RX_Typedef *RX, void *Packet_Ready_CB)
{
    RX->Expect = EXPECT_SOP;            // Initialise RX state machine to receive the SOP byte
    RX->Packet_Ready = Packet_Ready_CB;    // INitialise our callback. This is called when we have received an error free, valid packet.
    Byte_Timeout_Ints = ((float)BYTE_TIMEOUT_MS / Clock_Get_Timer_ms());  // If this is too low then manually typing into a terminal will time out
}

/**
  * @brief  Process a single received byte from the comms channel. This_Byte will be passed through the receive processing atate machine and processed. 
  *         Once a full packet has been received then its validity will be checked (errors, command, payload). 
  *         If it is valid then the Packet_Ready() callback  will be called.
  *
  * @param  RX: The main RX object for this channel reception 
  * @param  This_Byte: The individual byte in sequence that has been received
  * @retval None
  */
void Comms_RX_Receive_Byte(Comms_RX_Typedef *RX, uint8_t This_Byte)
{
    if(RX->Expect == EXPECT_SOP)
    {   // we are expecting the statt of packet
        if(This_Byte == SOP_BYTE)   
        {   // yes, we have received the start of packet 
            RX->Expect = EXPECT_COMMAND;    // next expected byte is the command
            RX->Byte_Timer = 0;             // a packet reception is in progress, allow us to time out if it's not fully received
            RX->Packet.SOP = This_Byte;     // save the SOP byte
        }
    }
    else if(RX->Expect == EXPECT_COMMAND)
    {   // we are expecting the comand bytye
        RX->Packet.Command = This_Byte;     // save the command byte we received. We'll check it later.  
        RX->Expect = EXPECT_PAYLOAD;        // next we expect the payload
        RX->Running_Len = 0;                // ready to count the number of payload bytes 
        RX->Byte_Timer = 0;                 // a packet reception is in progress, allow us to time out if it's not fully received
    }
    else if(RX->Expect == EXPECT_PAYLOAD)
    {   //  we are expecting a payload byte 
        if(This_Byte == EOP_BYTE)
        {   // we received the end of packet
            if(RX->Packet_Ready != 0)
            {   // packet is valid and we have a callback registered
                RX->Packet.EOP = EOP_BYTE;
                RX->Packet.Payload.Len = RX->Running_Len;
                RX->Packet_Ready(&RX->Packet);   // fire the callback, passing in the received packet
            }
            RX->Expect = EXPECT_SOP;    // next thing we expect is a SOP byte  
        }
        else if(RX->Running_Len < PAYLOAD_BUF_SIZE)   // do we have room in our buffer to save this byte 
        {   // yes there is enough room to save this byte 
            RX->Packet.Payload.Buf[RX->Running_Len++] = This_Byte;   // save it 
        }
        else
        {
            // buffer is full and we havent received the EOP byte yet - don't save anything. 
            // extra data included beyond the PAYLOAD_BUF_SIZE will be ignored
        }
        RX->Byte_Timer = 0;          // a packet reception is in progress, allow us to time out if it's not fully received
    }
}

/**
  * @brief  Call this every timner interrupt (like each 1ms)
  *
  * @param  RX: The main RX object for this channel reception 
  * @retval None
  */
void Comms_RX_Timer(Comms_RX_Typedef *RX)
{
    if((RX->Expect != EXPECT_SOP) && (RX->Expect != EXPECT_UNDEFINED))
    {   // are we in the middle of receiving a packet
        if(++RX->Byte_Timer >= Byte_Timeout_Ints)  // and has it been a long time since we received our last byte?
        {
            RX->Expect = EXPECT_SOP;   // drop any current reception and wait for the next start of packet
        }
    }
}
