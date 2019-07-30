
#ifndef USBASPLOADER_H_5f27a7e9840141b1aa57eef07c1d939f
#define USBASPLOADER_H_5f27a7e9840141b1aa57eef07c1d939f 1

#include "../misc/iofixes.h"

#include <stdint.h>
#include <avr/pgmspace.h>
#include "../firmware/spminterface.h"

#ifndef SIZEOF_new_firmware
  #ifdef BOOTLOADER_ADDRESS
    #define SIZEOF_new_firmware (((FLASHEND)+1)-(BOOTLOADER_ADDRESS))
  #else
    #error unable to determine binary size of firmware
  #endif
#endif


extern const const uint16_t usbasploader[SIZEOF_new_firmware>>1] PROGMEM;
const uint8_t *new_firmware	=	(void*)&usbasploader;

#endif

