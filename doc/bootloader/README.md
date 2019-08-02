# Bootloader

## If you buy a kit from cftkb.com your microcontroller already has the bootloader flashed and you do not need this.

This project uses USBaspLoader as the usb bootloader on the atmega32a.
- My custom bootloader:   
https://github.com/coseyfannitutti/USBaspLoader/tree/discipline
  - Please ensure you are in the discipline branch of my fork of the USBaspLoader repository when downloading
  - Follow the directions in the readme for directions on setting up the build environment for your operating system as well as flash instructions.
  - Makefile.inc sets flashing device as usbtiny, as I use a sparkfun pocket avr programmer. If you need to use another programmer please edit line 41 of Makefile.inc accordingly. Only edit Makefile.inc, and NEVER directly edit the actual Makefile.
  - Commands for flashing once build environment is set up:
  
  	```make``` (compiles makefile)
  
  	```make flash``` (flashes makefile)
  
  	```make fuse``` (sets fuses for microcontroller)
  
 	```make lock``` (prevents bootloader from being overwritten)

## Enter bootloader mode
1. Plug in USB cable
2. Push and hold ```BOOT``` switch
3. Push and hold ```RESET``` switch
4. Release ```RESET``` switch
5. Release ```BOOT``` switch

If you have successfully entered bootloader mode you should see USBaspLoader in device manager or as a connected device in QMK Toolbox.