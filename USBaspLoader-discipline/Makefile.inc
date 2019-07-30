# Name: Makefile.inc
# Project: USBaspLoader (updater)
# Author: Stephan Bärwolf
# Creation Date: 2012-09-01
# Tabsize: 4
# License: GNU GPL v2 (see License.txt)

F_CPU = 16000000
DEVICE = atmega32

# where the updating firmware should be located (starting address)
FLASHADDRESS = 0x0000

# use any fix (ATxmega compatible) crc32, uncomment to disable feature or set automatically with value "0"
;UPDATECRC32  = 0

# some MCU independent defines...
#...will be extended within MCU dependend configuration below...
DEFINES += -DCONFIG_NO__CHIP_ERASE -DCONFIG_NO__ONDEMAND_PAGEERASE -DCONFIG_NO__PRESERVE_WATCHDOG

# have the bootloader always enter the programming mode
;DEFINES += -DCONFIG_HAVE__BOOTLOADER_ALWAYSENTERPROGRAMMODE

# have the bootloader exit itself after around 3 seconds (@16MHz) inactivity
;DEFINES += -DCONFIG_BOOTLOADER_LOOPCYCLES_TIMEOUT=16 -DCONFIG_HAVE__BOOTLOADER_ABORTTIMEOUTONACT

# save the PROG button on the layout (CAREFUL - read feature description first)
;DEFINES += -DCONFIG_HAVE__BOOTLOADER_IGNOREPROGBUTTON



# some MCUs with small BLS (bootloader section) need to deactivate some
# features in order to fit into BLS.
# If this option is enabled, also essential features may become deactivated
# Esp. you MUST ensure not to have watchdog initially enabled when using this!
DANGEROUS=0

# PROGRAMMER contains AVRDUDE options to address your programmer
# PROGRAMMER = -c pony-stk200
# PROGRAMMER = -c usbasp
PROGRAMMER = -c usbtiny

#  since USBaspLoader supports HAVE_BLB11_SOFTW_LOCKBIT...
LOCKOPT = -U lock:w:0x3f:m





#untested devices (WARNING: may destroy ISP ability!)
#device always should select maximum bootloader-section size
#please mail tests-reports to matrixstorm@gmx.de
#
#some features may be deactivated to fit into bootloader section
#ATTANTION: CONFIG_NO... feature deactivation may not be optimal!

FUSEOPT_8535            = -U lfuse:w:0x1f:m -U hfuse:w:0xc0:m
BOOTLOADER_ADDRESS_8535 = 0x1800
DEFINES_8535            = -DCONFIG_USE__EXCESSIVE_ASSEMBLER -DCONFIG_NO__NEED_WATCHDOG -DCONFIG_NO__PRECISESLEEP



FUSEOPT_16              = -U lfuse:w:0x1f:m -U hfuse:w:0xc0:m
BOOTLOADER_ADDRESS_16   = 0x3800
DEFINES_16              = -DCONFIG_USE__EXCESSIVE_ASSEMBLER -DCONFIG_NO__NEED_WATCHDOG -DCONFIG_NO__PRECISESLEEP
ifeq ($(F_CPU), 12000000)
DEFINES_16             += -DCONFIG_NO__HAVE_READ_LOCK_FUSE
endif



FUSEOPT_88              = -U lfuse:w:0xd7:m -U hfuse:w:0xd4:m -U efuse:w:0xf8:m
BOOTLOADER_ADDRESS_88   = 0x1800
DEFINES_88              = -DCONFIG_USE__EXCESSIVE_ASSEMBLER -DCONFIG_NO__NEED_WATCHDOG -DCONFIG_NO__PRECISESLEEP -DCONFIG_NO__HAVE_READ_LOCK_FUSE



FUSEOPT_164             = -U lfuse:w:0xd7:m -U hfuse:w:0xd0:m -U efuse:w:0xfc:m
BOOTLOADER_ADDRESS_164  = 0x3800
DEFINES_164             = -DCONFIG_USE__EXCESSIVE_ASSEMBLER -DCONFIG_NO__NEED_WATCHDOG  -DCONFIG_NO__PRECISESLEEP -DCONFIG_NO__FLASH_BYTE_READACCESS -DCONFIG_NO__HAVE_READ_LOCK_FUSE
ifeq ($(F_CPU), 12000000)
DEFINES_164            += -DCONFIG_NO__BOOTLOADER_CAN_EXIT 
endif



FUSEOPT_324             = $(FUSEOPT_164)
BOOTLOADER_ADDRESS_324  = 0x7000
DEFINES_324             = #-DEMPTY



FUSEOPT_128             = -U lfuse:w:0x1f:m -U hfuse:w:0xc0:m -U efuse:w:0xfd:m
BOOTLOADER_ADDRESS_128  = 0x1E000
DEFINES_128             = -D__AVR_LIBC_DEPRECATED_ENABLE__



FUSEOPT_640             = -U lfuse:w:0xd7:m -U hfuse:w:0xd0:m -U efuse:w:0xfc:m
BOOTLOADER_ADDRESS_640  = 0xE000
DEFINES_640             = #-DEMPTY



FUSEOPT_1280             = $(FUSEOPT_640)
BOOTLOADER_ADDRESS_1280  = 0x1E000
DEFINES_1280             = #-DEMPTY



FUSEOPT_1281             = $(FUSEOPT_1280)
BOOTLOADER_ADDRESS_1281  = 0x1E000
DEFINES_1281             = #-DEMPTY



FUSEOPT_2561             = $(FUSEOPT_1281)
BOOTLOADER_ADDRESS_2561  = 0x3E000
DEFINES_2561             = #-DEMPTY




#tested devices

# standard atmega8 needs BODLEVEL to be programed, since it is a 5V device
# you may also want to UNprogram  SUT1 to get a SLOWER bootup (lfuse then would be 0x3f)
FUSEOPT_8            = -U lfuse:w:0x1f:m -U hfuse:w:0xc0:m
BOOTLOADER_ADDRESS_8 = 0x1800
DEFINES_8            = -DCONFIG_USE__EXCESSIVE_ASSEMBLER -DCONFIG_NO__NEED_WATCHDOG -DCONFIG_NO__PRECISESLEEP



FUSEOPT_32              = $(FUSEOPT_16)
BOOTLOADER_ADDRESS_32   = 0x7000
DEFINES_32              = #-DEMPTY



FUSEOPT_162            = -U lfuse:w:0xff:m -U hfuse:w:0x90:m  -U efuse:w:0xf9:m
BOOTLOADER_ADDRESS_162 = 0x3800
DEFINES_162            = -DCONFIG_USE__EXCESSIVE_ASSEMBLER -DCONFIG_NO__NEED_WATCHDOG -DCONFIG_NO__PRECISESLEEP -DCONFIG_NO__HAVE_READ_LOCK_FUSE
ifeq ($(F_CPU), 12000000)
DEFINES_162             += -DCONFIG_NO__BOOTLOADER_CAN_EXIT
endif



FUSEOPT_168             = $(FUSEOPT_88)
BOOTLOADER_ADDRESS_168  = 0x3800
DEFINES_168             = -DCONFIG_USE__EXCESSIVE_ASSEMBLER -DCONFIG_NO__NEED_WATCHDOG -DCONFIG_NO__PRECISESLEEP -DCONFIG_NO__FLASH_BYTE_READACCESS -DCONFIG_NO__HAVE_READ_LOCK_FUSE 



FUSEOPT_328             = -U lfuse:w:0xd7:m -U hfuse:w:0xd0:m -U efuse:w:0x04:m
BOOTLOADER_ADDRESS_328  = 0x7000
DEFINES_328             = #-DEMPTY



FUSEOPT_644             = $(FUSEOPT_324)
BOOTLOADER_ADDRESS_644  = 0xE000
DEFINES_644             = #-DEMPTY



FUSEOPT_1284            = -U lfuse:w:0xd7:m -U hfuse:w:0xd0:m -U efuse:w:0xfc:m
BOOTLOADER_ADDRESS_1284 = 0x1E000
DEFINES_1284            = #-DEMPTY



FUSEOPT_2560             = $(FUSEOPT_1280)
BOOTLOADER_ADDRESS_2560  = 0x3E000
DEFINES_2560             = #-DEMPTY






#http://www.nongnu.org/avr-libc/user-manual/using_tools.html
#autoselect logic
ifeq ($(DEVICE), atmega8535)
  FUSEOPT            = $(FUSEOPT_8535)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_8535) 
  MCUARCH            = avr4
  DEFINES	     += $(DEFINES_8535)
else ifeq ($(DEVICE), atmega8)
  FUSEOPT            = $(FUSEOPT_8)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_8) 
  MCUARCH            = avr4
  DEFINES	     += $(DEFINES_8)
else ifeq ($(DEVICE), atmega16)
  FUSEOPT            = $(FUSEOPT_16)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_16)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_16)
else ifeq ($(DEVICE), atmega32)
  FUSEOPT            = $(FUSEOPT_32)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_32)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_32)
else ifeq ($(DEVICE), atmega88)
  FUSEOPT            = $(FUSEOPT_88)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_88)
  MCUARCH            = avr4
  DEFINES	     += $(DEFINES_88)
else ifeq ($(DEVICE), atmega88a)
  FUSEOPT            = $(FUSEOPT_88)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_88)
  MCUARCH            = avr4
  DEFINES	     += $(DEFINES_88)
else ifeq ($(DEVICE), atmega88p)
  FUSEOPT            = $(FUSEOPT_88)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_88)
  MCUARCH            = avr4
  DEFINES	     += $(DEFINES_88)
else ifeq ($(DEVICE), atmega88pa)
  FUSEOPT            = $(FUSEOPT_88)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_88)
  MCUARCH            = avr4
  DEFINES	     += $(DEFINES_88)
else ifeq ($(DEVICE), atmega162)
  FUSEOPT            = $(FUSEOPT_162)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_162)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_162)
else ifeq ($(DEVICE), atmega164a)
  FUSEOPT            = $(FUSEOPT_164)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_164)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_164)
else ifeq ($(DEVICE), atmega164p)
  FUSEOPT            = $(FUSEOPT_164)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_164)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_164)
else ifeq ($(DEVICE), atmega164pa)
  FUSEOPT            = $(FUSEOPT_164)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_164)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_164)
else ifeq ($(DEVICE), atmega168)
  FUSEOPT            = $(FUSEOPT_168)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_168)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_168)
else ifeq ($(DEVICE), atmega168a)
  FUSEOPT            = $(FUSEOPT_168)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_168)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_168)
else ifeq ($(DEVICE), atmega168p)
  FUSEOPT            = $(FUSEOPT_168)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_168)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_168)
else ifeq ($(DEVICE), atmega168pa)
  FUSEOPT            = $(FUSEOPT_168)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_168)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_168)
else ifeq ($(DEVICE), atmega324a)
  FUSEOPT            = $(FUSEOPT_324)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_324)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_324)
else ifeq ($(DEVICE), atmega324p)
  FUSEOPT            = $(FUSEOPT_324)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_324)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_324)
else ifeq ($(DEVICE), atmega324pa)
  FUSEOPT            = $(FUSEOPT_324)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_324)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_324)
else ifeq ($(DEVICE), atmega328)
  FUSEOPT            = $(FUSEOPT_328)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_328)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_328)
else ifeq ($(DEVICE), atmega328p)
  FUSEOPT            = $(FUSEOPT_328)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_328)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_328)
else ifeq ($(DEVICE), atmega640)
  FUSEOPT            = $(FUSEOPT_640)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_640)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_640)
else ifeq ($(DEVICE), atmega644)
  FUSEOPT            = $(FUSEOPT_644)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_644)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_644)
else ifeq ($(DEVICE), atmega644a)
  FUSEOPT            = $(FUSEOPT_644)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_644)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_644)
else ifeq ($(DEVICE), atmega644p)
  FUSEOPT            = $(FUSEOPT_644)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_644)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_644)
else ifeq ($(DEVICE), atmega644pa)
  FUSEOPT            = $(FUSEOPT_644)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_644)
  MCUARCH            = avr5
  DEFINES	     += $(DEFINES_644)
else ifeq ($(DEVICE), atmega128)
  FUSEOPT            = $(FUSEOPT_128)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_128)
  MCUARCH            = avr51
  DEFINES	     += $(DEFINES_128)
else ifeq ($(DEVICE), atmega1280)
  FUSEOPT            = $(FUSEOPT_1280)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_1280)
  MCUARCH            = avr51
  DEFINES	     += $(DEFINES_1280)
else ifeq ($(DEVICE), atmega1281)
  FUSEOPT            = $(FUSEOPT_1281)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_1281)
  MCUARCH            = avr51
  DEFINES	     += $(DEFINES_1281)
else ifeq ($(DEVICE), atmega1284)
  FUSEOPT            = $(FUSEOPT_1284)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_1284)
  MCUARCH            = avr51
  DEFINES	     += $(DEFINES_1284)
else ifeq ($(DEVICE), atmega1284p)
  FUSEOPT            = $(FUSEOPT_1284)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_1284)
  MCUARCH            = avr51
  DEFINES	     += $(DEFINES_1284)
else ifeq ($(DEVICE), atmega2560)
  FUSEOPT            = $(FUSEOPT_2560)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_2560)
  MCUARCH            = avr6
  DEFINES	     += $(DEFINES_2560)
else ifeq ($(DEVICE), atmega2561)
  FUSEOPT            = $(FUSEOPT_2561)
  BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS_2561)
  MCUARCH            = avr6
  DEFINES	     += $(DEFINES_2561)
else
  FUSEOPT = unknown
endif

NEW_BOOTLOADER_ADDRESS = $(BOOTLOADER_ADDRESS)

# Tools:
AVRPATH = $(AVR8TOOLCHAINBINDIR)
AVRDUDE = $(AVRDUDEBINDIR)avrdude $(PROGRAMMER) -p $(DEVICE)

# Platform
# Check operative system.
# Check if we are on Windows.
ifeq ($(OS), Windows_NT)
  HOSTOS := Windows_NT
  # Check if although we are on a windows machine it's a linux-like
  # environment (Cygwin/Mingw or similar).
  ifeq (marks,$(shell echo "marks"))
    HOSTOS := Linux
  endif
else
  UNAME := $(shell uname -s)
  # Check if it's a OS X system.
  ifeq ($(UNAME), Darwin)
    HOSTOS := Darwin
  # Default as Linux.
  else
    HOSTOS := Linux
  endif
endif

ECHO=@echo
GCC=@gcc
MAKE=@make
ifeq ($(HOSTOS), Windows_NT)
  RM=@del /F /Q
else
  RM=@rm -f
endif

CC=@$(AVRPATH)avr-gcc
OBC=@$(AVRPATH)avr-objcopy
OBD=@$(AVRPATH)avr-objdump
SIZ=@$(AVRPATH)avr-size

