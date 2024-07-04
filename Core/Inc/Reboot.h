#ifndef REBOOT_H_
#define REBOOT_H_

/**
 * @brief Type of reboot being requested
 * 
 */
typedef enum 
{
    REBOOT_REQUEST_NONE = 0,    // Has no effect on system. No reboot will occur
    REBOOT_REQUEST_NORMAL,      // A normal reboot will occur. Application will restart as though a power-on reset occurred.
    REBOOT_REQUEST_DFU          // (Not yet implemented) A reboot into the DFU bootloader will occur. This is equivalent to booting with the BOOT0 ppin held high.
}Reboot_Request_Type;

void Reboot_Request(Reboot_Request_Type R);
void Reboot_Main(void);

#endif
