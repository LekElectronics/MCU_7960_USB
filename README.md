# BTS7960 USB INTERFACE 
Firmware that can be run on a small micro like STM32F0, allowing a PC to control a BTS7960 motor driver PCBA.
When plugged into a PC will show itself as a virtual com port (VCP).
It converts commands received from the USB port into PWM signals that can be applied to the BTS7960.

## Getting started

Options for getting the built firmware:
1. Download the artifact from GitLab
2. Build yourself

   a. pull repo

   b. make -f STM32Make.make

OR:
Under a linux environment, run build.sh

## Loading firmware onto target PCBA

1. Use STLink-V2 or equivalent for debugging and dev.
   Use any pads/pins on the target PCBA that expose the SWCLK, SWDIO, GND pins. 
   Connect the pins to the STLink V2.    
2. Use DFU bootloader to load the .bin/.elf/.hex into the target
   Apply 3.3V to BOOT0 pin.
   Plug in USB cable tyo target PCBA, to power the target device. 
   CPU will start in DFU bootloader mode. 
   Use STM32Cube Programmer or equivalent to program firmware into target CPU.


## Editing

1. Use STM32CubeMX to edit the target .ioc file. This defines the hardware configuration in the CPU.
2. Edit code using VSCode (with "vscode-for-stm32 extension)
3. Build/debug using the buttons available in VS Code vs-code-for-stm32 extension.

