This is the README file for USBaspLoader.

USBaspLoader is a USB boot loader for AVR microcontrollers. It can be used on
most AVRs with at least 2 kB of boot loader section, e.g. the popular ATMega8.
The firmware is flashed into the bootloader-section of the flash memory and
takes control immediately after reset. If a certain hardware condition is met
(this condition can be configured, e.g. a jumper), the boot loader waits for
data on the USB interface and loads it into the remaining part of the flash
memory. If the condition is not met, control is passed to the loaded firmware.

This boot loader is similar to Thomas Fischl's avrusbboot and our own
bootloadHID, but it requires no separate command line tool to upload the data.
USBaspLoader emulates Thomas Fischl's USBasp programmer instead. You can thus
use AVRDUDE to upload flash memory data (and if the option is enabled) EEPROM
data.

Since USBaspLoader cooperates with AVRDUDE, it can be used in conjunction with
the Arduino software to upload flash memory data.


FILES IN THE DISTRIBUTION
=========================
Readme.txt ........ The file you are currently reading.
firmware .......... Source code of the controller firmware.
firmware/usbdrv ... USB driver -- See Readme.txt in that directory for info
updater ........... Source code of an updater-firmware exchanging bootloaders
License.txt ....... Public license (GPL2) for all contents of this project.
Schematics.txt .... File giving infos about default and recommended hw-layout.

*_usbasploader.hex  precompiled USBaspLoader for default layout and settings


BUILDING AND INSTALLING
=======================
This project can be built on Unix (Linux, FreeBSD or Mac OS X) or Windows.

For all platforms, you must first describe your hardware and layout 
specific parameters (PINs to use, etc...) in "firmware/bootloaderconfig.h".
Some USB tuning is possible by modifying "firmware/usbconfig.h".
All files provide working default settings, which can be used as an example.
Then edit "Makefile.inc" (NEVER any Makefile directly!) to reflect
the target device, and some feature set departing from the default one.
(You also can edit "firmware/bootloaderconfig.h" to change feature set
and therefore memory space requirements).
Since now "Makefile.inc" has an automatic selection logic based on
the type of the microcontroller, there is NO need for setting bootloader-
addresses, lock- or fusebits anymore...

Building on Windows:
You need WinAVR for the firmware, see http://winavr.sourceforge.net/.
To build the firmware with WinAVR, change into the "USBaspLoader" directory,
check whether you need to edit the files mentioned above (e.g. change device
settings, programmer hardware, clock rate etc.) and type "make" to compile
the complete source code.
After you upload the code to the device with "make flash", you should set
the fuses with "make fuse".

If you already have some working USBaspLoader on your controller, you can
use "make update" to flash the "updater"-firmware to it.
After starting this firmware on the microcontroller it will replace the
old bootloader with the new one just compiled.
The update-feature can be used on boards, where HVPP or ISP is not possible
after production anymore. (Or you just want to save trouble laying out ISP.)
Therefore you also could implement the "HAVE_SPMINTEREFACE_MAGICVALUE"-
feature, protecting your board from wrong updates for other boards.

At default configuration the bootloader protects itself from overwriting
itself. In order to sustain the new update-capability, no lock bits 
("make lock") should be programmed after uploading the firmware and
programming the fuse bits.


Building on Unix (Linux, FreeBSD and Mac):
You need the GNU toolchain and avr-libc for the firmware. See
    http://www.nongnu.org/avr-libc/user-manual/install_tools.html
for a good description on how to install the GNU compiler toolchain and
avr-libc on Unix. For Mac OS X, we provide a read-made package, see
    http://www.obdev.at/avrmacpack/

To build the firmware, change to the "USBaspLoader" directory, edit the
files mentioned above if you need to change settings (as also described in
the Windows paragraph above) and type "make" to compile the source code.
After you upload the code to the device with "make flash", you should
set the fuses with "make fuse".
As described within the Windows paragraph, "make update" can also
be used instead.

WORKING WITH THE BOOT LOADER
============================
The boot loader is quite easy to use. Set the jumper (or whatever condition
you have configured) for boot loading on the target hardware, connect it to
the host computer and (if not bus powered) issue a Reset on the AVR.

You can now flash the device with AVRDUDE through a "virtual" USBasp
programmer.


ABOUT THE LICENSE
=================
It is our intention to make our USB driver and this demo application
available to everyone. Moreover, we want to make a broad range of USB
projects and ideas for USB devices available to the general public. We
therefore want that all projects built with our USB driver are published
under an Open Source license. Our license for the USB driver and demo code is
the GNU General Public License Version 2 (GPL2). See the file "License.txt"
for details.

If you don't want to publish your source code under the GPL2, you can simply
pay money for AVR-USB. As an additional benefit you get USB PIDs for free,
licensed exclusively to you. See the file "CommercialLicense.txt" in the usbdrv
directory for details.


MORE INFORMATION
================
For questions, reports, suggestions or just for fun please contact
Stephan Baerwolf (matrixstorm@gmx.de) and/or visit demonstration-board
for USBaspLoader at

    http://matrixstorm.com/avr/tinyusbboard/


For more information about Objective Development's firmware-only USB driver
for Atmel's AVR microcontrollers please visit the URL

    http://www.obdev.at/products/avrusb/

A technical documentation of the driver's interface can be found in the
file "firmware/usbdrv/usbdrv.h".


--
recent version:
(c) 2013 by Stephan Baerwolf
matrixstorm@gmx.de

initial version:
(c) 2008 by OBJECTIVE DEVELOPMENT Software GmbH.
http://www.obdev.at/
