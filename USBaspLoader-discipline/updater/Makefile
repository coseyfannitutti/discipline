# Name: Makefile
# Project: USBaspLoader (updater)
# Author: Stephan Bärwolf
# Creation Date: 2012-09-01
# Tabsize: 4
# License: GNU GPL v2 (see License.txt)

include ../Makefile.inc

ifeq ($(HOSTOS), Windows_NT)
	# If windows we will use an script for calculating filesize.
	FILESIZE = filesize.bat
else ifeq ($(HOSTOS), Darwin)
	# Variable declaration for Mac OS X (BSD stat).
	FILESIZE = stat -f %z
else
	# Default settings for remaining operating systems (GNU stat).
	FILESIZE = stat -c %s
endif

# elsewise gcc would complain unnecessary
CFLAGS = -Wall -Wno-pointer-to-int-cast -Os -g3 -ggdb -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -I. -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -DBOOTLOADER_ADDRESS=$(BOOTLOADER_ADDRESS) -DNEW_BOOTLOADER_ADDRESS=$(NEW_BOOTLOADER_ADDRESS) -DFLASHADDRESS=$(FLASHADDRESS) $(DEFINES)
LDFLAGS = -Wl,--relax,--gc-sections

DEPENDS =  ../firmware/bootloaderconfig.h ../Makefile.inc

ifneq ($(FLASHADDRESS), 0)
ifneq ($(FLASHADDRESS), 00)
ifneq ($(FLASHADDRESS), 000)
ifneq ($(FLASHADDRESS), 0000)
ifneq ($(FLASHADDRESS), 00000)
ifneq ($(FLASHADDRESS), 0x0)
ifneq ($(FLASHADDRESS), 0x00)
ifneq ($(FLASHADDRESS), 0x000)
ifneq ($(FLASHADDRESS), 0x0000)
ifneq ($(FLASHADDRESS), 0x00000)
FLASHPREAMBLE = 0x0000
LDFLAGS += -Wl,--section-start=.text=$(FLASHADDRESS)
endif
endif
endif
endif
endif
endif
endif
endif
endif
endif

all:  updater.hex $(DEPENDS)

flash:	all
	$(ECHO) "."
	$(AVRDUDE) -D -U flash:w:updater.hex:i
	$(ECHO) "."
	$(ECHO) "."


../firmware/main.elf: $(DEPENDS)
	$(MAKE) -C ../firmware main.hex


usbasploader.raw: ../firmware/main.elf $(DEPENDS)
	$(OBC) -j .text -j .data -O binary ../firmware/main.elf usbasploader.raw

usbasploader.o: usbasploader.raw $(DEPENDS)
	$(OBC) -B $(MCUARCH) -I binary -O elf32-avr --rename-section .data=.text --redefine-sym _binary_usbasploader_raw_start=usbasploader  usbasploader.raw usbasploader.o


updater.o: updater.c usbasploader.h usbasploader.raw usbasploader.o $(DEPENDS)
ifndef UPDATECRC32
	$(CC) updater.c -c -o updater.o -DSIZEOF_new_firmware=$(shell $(FILESIZE) usbasploader.raw) $(CFLAGS)
else
ifeq ($(UPDATECRC32), 0)
	$(CC) updater.c -c -o updater.o -DSIZEOF_new_firmware=$(shell $(FILESIZE) usbasploader.raw) -DUPDATECRC32=0x$(shell crc32 usbasploader.raw) $(CFLAGS)
else
	$(CC) updater.c -c -o updater.o -DSIZEOF_new_firmware=$(shell $(FILESIZE) usbasploader.raw) -DUPDATECRC32=$(UPDATECRC32) $(CFLAGS)
endif
endif
# 	$(CC) updater.c -c -o updater.o $(CFLAGS)

updater.elf: updater.o usbasploader.o $(DEPENDS)
	$(CC) updater.o usbasploader.o -o updater.elf -Wl,-Map,updater.map $(CFLAGS) $(LDFLAGS)

updater.asm: updater.elf $(DEPENDS)
	$(OBD) -Stdr updater.elf > updater.asm

updater.hex: updater.elf updater.asm $(DEPENDS)
	$(OBC) -j .text -j .data -O ihex updater.elf updater.hex
	$(ECHO) "."
	$(ECHO) "."
	$(SIZ) updater.elf
	$(ECHO) "."

deepclean: clean
ifeq ($(HOSTOS), Windows_NT)
else
	$(RM) *~
endif

clean:
	$(RM) usbasploader.o
	$(RM) updater.o
	$(RM) usbasploader.raw
	$(RM) updater.hex
	$(RM) updater.asm
	$(RM) updater.elf
	$(RM) updater.map
	$(RM) usbasploader.raw
