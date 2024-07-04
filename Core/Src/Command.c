/**
  @file Command.c
  @brief Executes commands received from the USB host
  @details After a packet has been received from the host, it can be processed by passing the command and payload 
           into this module via a call to Command_Execute(). This will execute the command and return a payload that can 
           be returned to the host. The payload contains the execution result and any relevent data being requested    


 */

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "Command.h"
#include "Firmware_Version.h"
#include "IO.h"
#include "Reboot.h"

/**
  * @brief  Try extract 4 PWM values from the payload. Expected format is aaa,bbb,ccc,ddd. where aaa/bbb/ccc/ddd is text between 0 and 100 (between 1 and 3 chars).
  *         Comma seperates each PWM value.
  *         Any values greater than 100 will cause a fail. 
  *         If there are less than 4 values then this will cause a fail. 
  *         Only the first 4 values will be read, any more will be ignored   
  *
  * @param  PWMs: An array to store each of the numeric PWM values. 
  * @param  Payload: The payload to be searched
  * @retval true if the 4 PWM values were stored in the array successfully, false otherwise. 
  */
bool Get_PWMs_From_Payload(uint8_t PWMs[4], Comms_Payload Payload)
{
    if((Payload.Len < 7) || (Payload.Len > 15))
    {   // the minimum possible payload is a,b,c,d (7 chars) 
        return false;   // any payload less than the minimum is invalid
    }

    char StrPayload[PAYLOAD_BUF_SIZE+1];
    strncpy(StrPayload, (char*)Payload.Buf, Payload.Len);
    StrPayload[Payload.Len] = 0;    // hammer a final null terminator in case payload buf is full

    char *Token = strtok(StrPayload, ",");
    for(uint8_t PWMs_Idx = 0; PWMs_Idx < 4; PWMs_Idx++)
    {
        // for each of the 4 PWM values
        if(strlen(Token) <= 3)
        {
            for(uint8_t c = 0; c < strlen(Token); c++)
            {
                if(Token[c] < '0' || Token[c] > '9' )
                {
                    return false;
                }
            }
            // if code gets here then all chars in Token are numeric 
            int num = atoi(Token);
            if((num>=0) && (num <= 100))
            {
                PWMs[PWMs_Idx] = num;
            }
            else
            {
                return false;
            }
        }
        else
        {
            // length is too long - fail
            return false; 
        }
        Token = strtok(NULL, ",");
    }

    return true;

} 

/**
    @brief  Read currently applied PWM values and fill them into the payload for returning to the comms channel.
   
    @param  P: The payload/parameters to be loaded. Each PWM will be a uint8_t between 0 and 100
        Response to Host Controller is:
         P->Buf[0] = RESP_ACK;
         p->Buf[1]... = aaa,bbb,ccc,ddd,eeee,ffff
        where 
         aaa is the PWM percentage for ENA_L
         bbb is the PWM percentage for ENA_R
         ccc is the PWM percentage for PWM_L
         ddd is the PWM percentage for PWM_R
    @retval none 
  */
void Load_Buf_With_Status(Comms_Payload *P)
{
    P->Buf[0] = RESP_ACK;
    sprintf((char*)&P->Buf[1], "%i,", IO_Get_PWM_Percent(ENA_L));
    uint8_t val = IO_Get_PWM_Percent(ENA_R);
    sprintf((char*)&P->Buf[1+strlen((char*)&P->Buf[1])], "%i,", val); 
    val = IO_Get_PWM_Percent(PWM_L);
    sprintf((char*)&P->Buf[1+strlen((char*)&P->Buf[1])], "%i,", val);
    val =  IO_Get_PWM_Percent(PWM_R);
    sprintf((char*)&P->Buf[1+strlen((char*)&P->Buf[1])], "%i,", val); 
    
    P->Len = 1 + strlen((char*)&P->Buf[1]);
}

/**
    @brief  Process the command and any included payload.     
    @param  Cmd: Command to be executed
    @param  Payload: The payload/parameters associated with the command
    @retval Payload of the reply to send back in response to processing this command 
*/
Comms_Payload *Command_Execute(Comms_Commands Cmd, Comms_Payload Payload)
{
    static Comms_Payload p;

    switch(Cmd)
    {
        case COMMAND_FW_VER:
            const char *str = Firmware_Version_Get();
            p.Buf[0] = RESP_ACK; 
            strncpy((char*)&p.Buf[1], str, PAYLOAD_BUF_SIZE-2);    // copy string to buffer, don't overflow. -1 byte for the resp code, -1 byte for the string null terminator 
            p.Len = strlen((char*)&p.Buf[1]) + 1;
            break;
        case COMMAND_SET_OUTPUTS:
            // extract PWM outputs ena-l,ena-r,pwm-l, pwm-r
            uint8_t PWMs[4];
            if(Get_PWMs_From_Payload(PWMs, Payload))
            {
                IO_Set_PWM_Percent(PWMs[0], ENA_L);
                IO_Set_PWM_Percent(PWMs[1], ENA_R);
                IO_Set_PWM_Percent(PWMs[2], PWM_L);
                IO_Set_PWM_Percent(PWMs[3], PWM_R);
                p.Buf[0] = RESP_ACK;
            }
            else
            {
                p.Buf[0] = RESP_INV_PAYLOAD;
            }
            p.Len = 1;
            break;
        case COMMAND_STATUS:
            Load_Buf_With_Status(&p);
            break;
        case COMMAND_REBOOT:
            if(Payload.Buf[0] == 'N')
            {
                p.Buf[0] = RESP_ACK;
                Reboot_Request(REBOOT_REQUEST_NORMAL);
            }
            else
            {
                p.Buf[0] = RESP_INV_PAYLOAD;
            }
            p.Len = 1;
            break;
        default:
            p.Buf[0] = RESP_INV_COMMAND; 
            p.Len = 1;
            break;
    }

    return &p;
}

