# Name: Makefile
# Project: USBaspLoader
# Author: Christian Starkjohann
# Creation Date: 2007-12-10
# Author: Stephan Bärwolf
# Improvement Date: 2012-07-31
# Improvement Date: 2012-09-12
# Tabsize: 4
# Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
# License: GNU GPL v2 (see License.txt)

###############################################################################
# Configure the following variables according to your AVR.
# Program the device with
#     make fuse    # to set the clock generator, boot section size etc.
#     make flash   # to load the boot loader into flash
#     make lock    # to protect the boot loader from overwriting

include ../Makefile.inc

# Remove the -fno-* options when you use gcc 3, it does not understand them
CFLAGS = -Wall -Os -g3 -ggdb -fno-move-loop-invariants -fno-tree-scev-cprop -fno-inline-small-functions -I. -mmcu=$(DEVICE) -DBOOTLOADER_ADDRESS=$(BOOTLOADER_ADDRESS) -DF_CPU=$(F_CPU) $(DEFINES)
LDFLAGS = -Wl,--relax,--gc-sections -Wl,--section-start=.text=$(BOOTLOADER_ADDRESS) -Wl,--defsym=nullVector=0

DEPENDS =  bootloaderconfig.h ../Makefile.inc

# symbolic targets:
all: main.hex $(DEPENDS)

usbdrv/usbdrvasm.o: usbdrv/usbdrvasm.S $(DEPENDS)
	$(CC) -x assembler-with-cpp -c usbdrv/usbdrvasm.S -o usbdrv/usbdrvasm.o $(CFLAGS)

usbdrv/oddebug.o: usbdrv/oddebug.c $(DEPENDS)
	$(CC) usbdrv/oddebug.c -c -o usbdrv/oddebug.o $(CFLAGS)

main.o: main.c $(DEPENDS)
	$(CC) main.c -c -o main.o $(CFLAGS)

flash:	all
	$(ECHO) "."
	$(AVRDUDE) -U flash:w:main.hex:i
	$(ECHO) "."
	$(ECHO) "."

readflash:
	$(ECHO) "."
	$(AVRDUDE) -U flash:r:read.hex:i
	$(ECHO) "."
	$(ECHO) "."


fuse:
	$(ECHO) "."
	$(AVRDUDE) $(FUSEOPT)
	$(ECHO) "."
	$(ECHO) "."


lock:
	$(ECHO) "."
	$(AVRDUDE) $(LOCKOPT)
	$(ECHO) "."
	$(ECHO) "."


read_fuses:
	$(UISP) --rd_fuses

deepclean: clean
ifeq ($(HOSTOS), Windows_NT)
else
	$(RM) *~
endif

clean: 
	$(RM) main.hex
	$(RM) main.asm
	$(RM) main.map
	$(RM) main.elf
	$(RM) main.o
	$(RM) main.s
ifeq ($(HOSTOS), Windows_NT)
	$(RM) usbdrv\usbdrvasm.o
	$(RM) usbdrv\oddebug.o
	$(RM) usbdrv\oddebug.s
	$(RM) usbdrv\usbdrv.s
else
	$(RM) usbdrv/usbdrvasm.o
	$(RM) usbdrv/oddebug.o
	$(RM) usbdrv/oddebug.s
	$(RM) usbdrv/usbdrv.s
endif

# file targets:
main.elf: usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o $(DEPENDS)
	$(CC) $(CFLAGS) -o main.elf usbdrv/usbdrvasm.o usbdrv/oddebug.o main.o -Wl,-Map,main.map $(LDFLAGS)

main.asm: main.elf $(DEPENDS)
	$(OBD) -Stdr main.elf > main.asm

main.hex: main.elf main.asm $(DEPENDS)
	$(RM) main.hex main.eep.hex
	$(OBC) -j .text -j .data -O ihex main.elf main.hex
	$(ECHO) "."
	$(ECHO) "."
	$(ECHO) "."
	$(ECHO) "!!!ATTANTION!!!"
	$(ECHO) "(data+text) MUST fit into your MCUs bootloader section"
	$(ECHO) "."
	$(SIZ) --mcu $(DEVICE) main.elf
	$(ECHO) "."
	$(ECHO) "."
	$(ECHO) "."

disasm:	main.elf $(DEPENDS)
	$(OBD) -d main.elf

cpp: $(DEPENDS)
	$(CC) $(CFLAGS) -E main.c
