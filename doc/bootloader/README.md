# Bootloader

## If you buy a kit from cftkb.com your microcontroller already has the bootloader flashed and you do not need this.

This project uses USBaspLoader as the usb bootloader on the atmega32a.
- My custom bootloader:   
https://github.com/coseyfannitutti/USBaspLoader/tree/atmega32a
  - Please ensure you are in the atmega32a branch of my fork of the USBaspLoader repository when downloading.
  - Follow the directions in the readme for directions on setting up the build environment for your operating system as well as flash instructions.
  - Makefile.inc sets flashing device as usbtiny, as I use a sparkfun pocket avr programmer. If you need to use another programmer please edit line 41 of Makefile.inc accordingly. Only edit Makefile.inc, and NEVER directly edit the actual Makefile.
  - Commands for flashing once build environment is set up:
  
	```make flash``` (flashes makefile)
  
  	```make fuse``` (sets fuses for microcontroller)
  
## Enter bootloader mode
1. Press and hold ```BOOT``` switch
2. Tap ```RESET``` switch
3. Release ```BOOT``` switch

Alternatively, you can hold ```BOOT``` switch while inserting the USB cable.

If you have successfully entered bootloader mode you should see USBaspLoader in device manager or as a connected device in QMK Toolbox.
