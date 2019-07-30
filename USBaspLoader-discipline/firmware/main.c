/* Name: main.c
 * Project: USBaspLoader
 * Author: Christian Starkjohann
 * Author: Stephan Baerwolf
 * Creation Date: 2007-12-08
 * Modification Date: 2013-03-31
 * Tabsize: 4
 * Copyright: (c) 2007 by OBJECTIVE DEVELOPMENT Software GmbH
 * License: GNU GPL v2 (see License.txt)
 */

#include "spminterface.h"  /* must be included as first! */

#include "../misc/iofixes.h"

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/eeprom.h>
#include <util/delay.h>


#if 0
/*
 * 29.09.2012 /  30.09.2012
 * 
 * Since cpufunc.h is not needed in this context and
 * since it is not available in all toolchains, this include
 * becomes deactivated by github issue-report.
 * (In case of trouble it remains in sourcecode for reactivation.)
 * 
 * The autor would like to thank Lena-M for reporting this
 * issue (https://github.com/baerwolf/USBaspLoader/issues/1).
 */
#include <avr/cpufunc.h>
#endif

#include <avr/boot.h>

#include <string.h>



#include "bootloaderconfig.h"

#include "usbdrv/usbdrv.c"
static usbMsgLen_t usbFunctionDescriptor(struct usbRequest *rq) {
  return 0;
}

#ifndef BOOTLOADER_ADDRESS
  #error need to know the bootloaders flash address!
#endif
#define BOOTLOADER_PAGEADDR	(BOOTLOADER_ADDRESS - (BOOTLOADER_ADDRESS % SPM_PAGESIZE))

/* ------------------------------------------------------------------------ */

/* Request constants used by USBasp */
#define USBASP_FUNC_CONNECT         1
#define USBASP_FUNC_DISCONNECT      2
#define USBASP_FUNC_TRANSMIT        3
#define USBASP_FUNC_READFLASH       4
#define USBASP_FUNC_ENABLEPROG      5
#define USBASP_FUNC_WRITEFLASH      6
#define USBASP_FUNC_READEEPROM      7
#define USBASP_FUNC_WRITEEEPROM     8
#define USBASP_FUNC_SETLONGADDRESS  9

// additional USBasp Commands
#define USBASP_FUNC_SETISPSCK	     10
#define USBASP_FUNC_TPI_CONNECT      11
#define USBASP_FUNC_TPI_DISCONNECT   12
#define USBASP_FUNC_TPI_RAWREAD      13
#define USBASP_FUNC_TPI_RAWWRITE     14
#define USBASP_FUNC_TPI_READBLOCK    15
#define USBASP_FUNC_TPI_WRITEBLOCK   16
#define USBASP_FUNC_GETCAPABILITIES 127
/* ------------------------------------------------------------------------ */

#ifndef ulong
#   define ulong    unsigned long
#endif
#ifndef uint
#   define uint     unsigned int
#endif


/* allow compatibility with avrusbboot's bootloaderconfig.h: */
#ifdef BOOTLOADER_INIT
#   define bootLoaderInit()         BOOTLOADER_INIT
#   define bootLoaderExit()
#endif
#ifdef BOOTLOADER_CONDITION
#   define bootLoaderCondition()    BOOTLOADER_CONDITION
#endif

/* device compatibility: */
#ifndef GICR    /* ATMega*8 don't have GICR, use MCUCR instead */
#   define GICR     MCUCR
#endif

/* ------------------------------------------------------------------------ */

#if (FLASHEND) > 0xffff /* we need long addressing */
#   define CURRENT_ADDRESS  currentAddress.l
#   define addr_t           ulong
#else
#   define CURRENT_ADDRESS  currentAddress.w[0]
#   define addr_t           uint
#endif

typedef union longConverter{
    addr_t  l;
    uint    w[sizeof(addr_t)/2];
    uchar   b[sizeof(addr_t)];
}longConverter_t;


#define __IMPLEMENT_PRESERVE_WATCHDOG	((PRESERVE_WATCHDOG) && (!(USE_EXCESSIVE_ASSEMBLER)) && ((NEED_WATCHDOG) || (defined(__MCUCSR_COMPATMODE))))
#if (__IMPLEMENT_PRESERVE_WATCHDOG)
static uint8_t __original_WDTCR;
#endif

#if (BOOTLOADER_CAN_EXIT)
#	if (BOOTLOADER_LOOPCYCLES_TIMEOUT)
#		if (BOOTLOADER_LOOPCYCLES_TIMEOUT < 256)
#			if ((HAVE_UNPRECISEWAIT))
	 register uint8_t timeout_remaining __asm__("r2");
#			else
static volatile uint8_t timeout_remaining;
#			endif
#		else
static volatile uint16_t timeout_remaining;
#		endif
#	endif

#	define stayinloader_initialValue 0xfe
#	if ((HAVE_UNPRECISEWAIT))
/* here we have to assume we need to optimize for every byte */
	 register uint8_t stayinloader __asm__("r17");
#	else
static volatile uint8_t stayinloader;
#	endif
#endif

static longConverter_t  	currentAddress; /* in bytes */
static uchar            	bytesRemaining;
static uchar            	isLastPage;
#if HAVE_EEPROM_PAGED_ACCESS
static uchar            	currentRequest;
#else
static const uchar      	currentRequest = 0;
#endif

static const uchar  signatureBytes[4] = {
#ifdef SIGNATURE_BYTES
    SIGNATURE_BYTES
#elif defined (__AVR_ATmega8535__)
    0x1e, 0x93, 0x08, 0
#elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || defined (__AVR_ATmega8HVA__)
    0x1e, 0x93, 0x07, 0
#elif defined (__AVR_ATmega16__)
    0x1e, 0x94, 0x03, 0
#elif defined (__AVR_ATmega32__)
    0x1e, 0x95, 0x02, 0
#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48A__)
    #error ATmega48 does not support bootloaders!
    0x1e, 0x92, 0x05, 0
#elif defined (__AVR_ATmega48PA__) || defined (__AVR_ATmega48P__)
    #error ATmega48P does not support bootloaders!
    0x1e, 0x92, 0x0A, 0
#elif defined (__AVR_ATmega88__) || defined (__AVR_ATmega88A__)
    0x1e, 0x93, 0x0a, 0
#elif defined (__AVR_ATmega88PA__) || defined (__AVR_ATmega88P__)
    0x1e, 0x93, 0x0F, 0
#elif defined (__AVR_ATmega162__)
    0x1e, 0x94, 0x04, 0
#elif defined (__AVR_ATmega164A__)
    0x1e, 0x94, 0x0f, 0
#elif defined (__AVR_ATmega164P__) || defined (__AVR_ATmega164PA__)
    0x1e, 0x94, 0x0a, 0
#elif defined (__AVR_ATmega168__) || defined (__AVR_ATmega168A__)
    0x1e, 0x94, 0x06, 0
#elif defined (__AVR_ATmega168PA__) || defined (__AVR_ATmega168P__)
    0x1e, 0x94, 0x0B, 0
#elif defined (__AVR_ATmega324A__)
    0x1e, 0x95, 0x15, 0
#elif defined (__AVR_ATmega324P__)
    0x1e, 0x95, 0x08, 0
#elif defined (__AVR_ATmega324PA__)
    0x1e, 0x95, 0x11, 0
#elif defined (__AVR_ATmega328__)
    0x1e, 0x95, 0x14, 0
#elif defined (__AVR_ATmega328P__)
    0x1e, 0x95, 0x0f, 0
#elif defined (__AVR_ATmega640__)
    0x1e, 0x96, 0x08, 0
#elif defined (__AVR_ATmega644__) || defined (__AVR_ATmega644A__)
    0x1e, 0x96, 0x09, 0
#elif defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__)
    0x1e, 0x96, 0x0a, 0
#elif defined (__AVR_ATmega128__)
    0x1e, 0x97, 0x02, 0
#elif defined (__AVR_ATmega1280__)
    0x1e, 0x97, 0x03, 0
#elif defined (__AVR_ATmega1281__)
    0x1e, 0x97, 0x04, 0
#elif defined (__AVR_ATmega1284__)
    0x1e, 0x97, 0x06, 0
#elif defined (__AVR_ATmega1284P__)
    0x1e, 0x97, 0x05, 0
#elif defined (__AVR_ATmega2560__)
    0x1e, 0x98, 0x01, 0
#elif defined (__AVR_ATmega2561__)
    0x1e, 0x98, 0x02, 0
#else
#   if (defined(SIGNATURE_0) && defined(SIGNATURE_1) && defined(SIGNATURE_2))
#     warning "Device signature is not known - using AVR Libc suggestion..."
    SIGNATURE_0, SIGNATURE_1, SIGNATURE_2, 0
#   else
#     error "Device signature is not known, please edit main.c!"
#   endif
#endif
};

/* ------------------------------------------------------------------------ */

#if (HAVE_BOOTLOADERENTRY_FROMSOFTWARE)
void __attribute__ ((section(".init3"),naked,used,no_instrument_function)) __BOOTLOADERENTRY_FROMSOFTWARE__bootup_investigate_RAMEND(void);
void __BOOTLOADERENTRY_FROMSOFTWARE__bootup_investigate_RAMEND(void) {
  asm volatile (
    "in		%[mcucsrval]	,	%[mcucsrio]\n\t"
    "ldi	r29		,	%[ramendhi]\n\t"
    "ldi	r28		,	%[ramendlo]\n\t"
#if (FLASHEND>131071)
    "ld		%[result]	,	Y+\n\t"
    "cpi	%[result]	,	%[bootaddrhi]\n\t"
    "brne	__BOOTLOADERENTRY_FROMSOFTWARE__bootup_investigate_RAMEND_mismatch%=\n\t"
#endif
    "ld		%[result]	,	Y+\n\t"
    "cpi	%[result]	,	%[bootaddrme]\n\t"
    "ld		%[result]	,	Y+\n\t"
    "breq	__BOOTLOADERENTRY_FROMSOFTWARE__bootup_investigate_RAMEND_done%=\n\t"

    "__BOOTLOADERENTRY_FROMSOFTWARE__bootup_investigate_RAMEND_mismatch%=:\n\t"
    "ldi	%[result]	,	0xff\n\t"

    "__BOOTLOADERENTRY_FROMSOFTWARE__bootup_investigate_RAMEND_done%=:\n\t"
    : [result]		"=a" (__BOOTLOADERENTRY_FROMSOFTWARE__bootup_RAMEND_doesmatch),
      [mcucsrval]	"=a" (__BOOTLOADERENTRY_FROMSOFTWARE__bootup_MCUCSR)
    : [mcucsrio]	"I"  (_SFR_IO_ADDR(MCUCSR)),
#if (FLASHEND>131071)
      [ramendhi]	"M" (((RAMEND - 2) >> 8) & 0xff),
      [ramendlo]	"M" (((RAMEND - 2) >> 0) & 0xff),
      [bootaddrhi]	"M" (((__BOOTLOADERENTRY_FROMSOFTWARE__EXPECTEDADDRESS) >>16) & 0xff),
#else
      [ramendhi]	"M" (((RAMEND - 1) >> 8) & 0xff),
      [ramendlo]	"M" (((RAMEND - 1) >> 0) & 0xff),
#endif
      [bootaddrme]	"M" (((__BOOTLOADERENTRY_FROMSOFTWARE__EXPECTEDADDRESS) >> 8) & 0xff)
    
  );
}
#endif

#if (USE_BOOTUP_CLEARRAM)
/*
* Under normal circumstances, RESET will not clear contents of RAM.
* As always, if you want it done - do it yourself...
*/
void __attribute__ ((section(".init3"),naked,used,no_instrument_function)) __func_clearram(void);
void __func_clearram(void) {
  extern size_t __bss_end;
  asm volatile (
    "__clearram:\n\t"
#if (!(HAVE_BOOTLOADERENTRY_FROMSOFTWARE))
    "ldi r29, %[ramendhi]\n\t"
    "ldi r28, %[ramendlo]\n\t"
#endif
    "__clearramloop%=:\n\t"
    "st -Y , __zero_reg__\n\t"
    "cp r28, %A[bssend]\n\t"
    "cpc r29, %B[bssend]\n\t"
    "brne __clearramloop%=\n\t"
    :
    : [ramendhi] "M" (((RAMEND+1)>>8) & 0xff),
      [ramendlo] "M" (((RAMEND+1)>>0) & 0xff),
      [bssend] "r" (&__bss_end)
    : "memory"
      );
}
#endif

#if (!USE_EXCESSIVE_ASSEMBLER) || (!(defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || defined (__AVR_ATmega8HVA__)))
static void (*nullVector)(void) __attribute__((__noreturn__));
#endif

#if (USE_EXCESSIVE_ASSEMBLER) && (defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || defined (__AVR_ATmega8HVA__))
static void __attribute__((naked,__noreturn__)) leaveBootloader(void);
static void leaveBootloader(void) {
  asm  volatile  (
  "cli\n\t"
  "sbi		%[usbddr],	%[usbminus]\n\t"  
  "cbi		%[port],	%[bit]\n\t"
  "out		%[usbintrenab],	__zero_reg__\n\t"
  "out		%[usbintrcfg],	__zero_reg__\n\t"
  "ldi		r31,		%[ivce]\n\t"
  "out		%[mygicr],	r31\n\t"
  "out		%[mygicr],	__zero_reg__\n\t"  
  "rjmp		nullVector\n\t"
  :
  : [port]        "I" (_SFR_IO_ADDR(PIN_PORT(JUMPER_PORT))),
    [bit]         "I" (PIN(JUMPER_PORT, JUMPER_BIT)),
    [usbintrenab] "I" (_SFR_IO_ADDR(USB_INTR_ENABLE)),
    [usbintrcfg]  "I" (_SFR_IO_ADDR(USB_INTR_CFG)),
    [usbddr]      "I" (_SFR_IO_ADDR(USBDDR)),
    [usbminus]    "I" (USBMINUS),
    [mygicr]      "I" (_SFR_IO_ADDR(GICR)),	      
    [ivce]        "I" (1<<IVCE)
);
}
#else
static void __attribute__((__noreturn__)) leaveBootloader(void);
static void leaveBootloader(void) {
    DBG1(0x01, 0, 0);
    cli();
    usbDeviceDisconnect();
    bootLoaderExit();
    USB_INTR_ENABLE = 0;
    USB_INTR_CFG = 0;       /* also reset config bits */
    GICR = (1 << IVCE);     /* enable change of interrupt vectors */
    GICR = (0 << IVSEL);    /* move interrupts to application flash section */

/* restore the original watchdog timer if necessary */
#if (__IMPLEMENT_PRESERVE_WATCHDOG)
    if (__original_WDTCR & _BV(WDE)) {
      __original_WDTCR &= ~(_BV(WDCE));
      wdt_reset();
      WDTCR |= (_BV(WDCE)) | (_BV(WDE));
      WDTCR  = __original_WDTCR;
      wdt_reset();
    }
#endif
/*
 * There seems to be another funny compiler Bug.
 * When gcc is using "eicall" opcode it forgets to modify EIND.
 * On devices with large flash memory there are some target address bits
 * missing. In this case some zero bits...
 */
#if (defined(EIND) && ((FLASHEND)>131071))
  EIND=0;
#endif
/* We must go through a global function pointer variable instead of writing
 *  ((void (*)(void))0)();
 * because the compiler optimizes a constant 0 to "rcall 0" which is not
 * handled correctly by the assembler.
 */
    nullVector();
}
#endif

/* ------------------------------------------------------------------------ */


uchar usbFunctionSetup_USBASP_FUNC_TRANSMIT(usbRequest_t *rq) {
  uchar rval = 0;
  usbWord_t address;
  address.bytes[1] = rq->wValue.bytes[1];
  address.bytes[0] = rq->wIndex.bytes[0];

  if(rq->wValue.bytes[0] == 0x30){        /* read signature */
    rval = rq->wIndex.bytes[0] & 3;
    rval = signatureBytes[rval];
#if HAVE_READ_LOCK_FUSE
#if defined (__AVR_ATmega8535__) || 					\
    defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || 		\
    defined (__AVR_ATmega16__) || defined (__AVR_ATmega32__)
  }else if(rq->wValue.bytes[0] == 0x58 && rq->wValue.bytes[1] == 0x00){  /* read lock bits */
      rval = boot_lock_fuse_bits_get(GET_LOCK_BITS);
  }else if(rq->wValue.bytes[0] == 0x50 && rq->wValue.bytes[1] == 0x00){  /* read lfuse bits */
      rval = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
  }else if(rq->wValue.bytes[0] == 0x58 && rq->wValue.bytes[1] == 0x08){  /* read hfuse bits */
      rval = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);

#elif defined (__AVR_ATmega48__)   || defined (__AVR_ATmega48A__)   || defined (__AVR_ATmega48P__)   || defined (__AVR_ATmega48PA__)  ||  \
defined (__AVR_ATmega88__)   || defined (__AVR_ATmega88A__)   || defined (__AVR_ATmega88P__)   || defined (__AVR_ATmega88PA__)  ||  \
defined (__AVR_ATmega162__)  || 												      \
defined (__AVR_ATmega164A__) || defined (__AVR_ATmega164P__)  || 								      \
defined (__AVR_ATmega168__)  || defined (__AVR_ATmega168A__)  || defined (__AVR_ATmega168P__)  || defined (__AVR_ATmega168PA__) ||  \
defined (__AVR_ATmega324A__) || defined (__AVR_ATmega324P__)  ||								      \
defined (__AVR_ATmega328__)  || defined (__AVR_ATmega328P__)  ||								      \
defined (__AVR_ATmega640__)  ||													\
defined (__AVR_ATmega644__)  || defined (__AVR_ATmega644A__)  || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__)  ||  \
defined (__AVR_ATmega128__)  ||													      \
defined (__AVR_ATmega1280__) ||													\
defined (__AVR_ATmega1281__) ||													\
defined (__AVR_ATmega1284__) || defined (__AVR_ATmega1284P__)  ||													\
defined (__AVR_ATmega2560__) ||													\
defined (__AVR_ATmega2561__)
  }else if(rq->wValue.bytes[0] == 0x58 && rq->wValue.bytes[1] == 0x00){  /* read lock bits */
      rval = boot_lock_fuse_bits_get(GET_LOCK_BITS);
  }else if(rq->wValue.bytes[0] == 0x50 && rq->wValue.bytes[1] == 0x00){  /* read lfuse bits */
      rval = boot_lock_fuse_bits_get(GET_LOW_FUSE_BITS);
  }else if(rq->wValue.bytes[0] == 0x58 && rq->wValue.bytes[1] == 0x08){  /* read hfuse bits */
      rval = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
  }else if(rq->wValue.bytes[0] == 0x50 && rq->wValue.bytes[1] == 0x08){  /* read efuse bits */
      rval = boot_lock_fuse_bits_get(GET_EXTENDED_FUSE_BITS );
#else
  #warning "HAVE_READ_LOCK_FUSE is activated but MCU unknown -> will not support this feature"
#endif
#endif
#if HAVE_FLASH_BYTE_READACCESS
  }else if(rq->wValue.bytes[0] == 0x20){  /* read FLASH low  byte */
#if ((FLASHEND) > 65535)
      rval = pgm_read_byte_far((((addr_t)address.word)<<1)+0);
#else
      rval = pgm_read_byte((((addr_t)address.word)<<1)+0);
#endif
  }else if(rq->wValue.bytes[0] == 0x28){  /* read FLASH high byte */
#if ((FLASHEND) > 65535)
      rval = pgm_read_byte_far((((addr_t)address.word)<<1)+1);
#else
      rval = pgm_read_byte((((addr_t)address.word)<<1)+1);
#endif
#endif
#if HAVE_EEPROM_BYTE_ACCESS
  }else if(rq->wValue.bytes[0] == 0xa0){  /* read EEPROM byte */
      rval = eeprom_read_byte((void *)address.word);
  }else if(rq->wValue.bytes[0] == 0xc0){  /* write EEPROM byte */
      eeprom_write_byte((void *)address.word, rq->wIndex.bytes[1]);
#endif
#if HAVE_CHIP_ERASE
  }else if(rq->wValue.bytes[0] == 0xac && rq->wValue.bytes[1] == 0x80){  /* chip erase */
      addr_t addr;
#if HAVE_BLB11_SOFTW_LOCKBIT
      for(addr = 0; addr < (addr_t)(BOOTLOADER_PAGEADDR) ; addr += SPM_PAGESIZE) {
#else
      for(addr = 0; addr <= (addr_t)(FLASHEND) ; addr += SPM_PAGESIZE) {
#endif
	  /* wait and erase page */
	  DBG1(0x33, 0, 0);
#   ifndef NO_FLASH_WRITE
	  boot_spm_busy_wait();
	  cli();
	  boot_page_erase(addr);
	  sei();
#   endif
      }
#endif
#if ((HAVE_BOOTLOADER_HIDDENEXITCOMMAND) && (BOOTLOADER_CAN_EXIT))
#	if ((HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0xac) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x20) && (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x28) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x40) && (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x48) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x4c) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0xa0) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0xc0) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x58) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x5c) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x30) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0xac) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x50) && (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x58) && \
	    (HAVE_BOOTLOADER_HIDDENEXITCOMMAND != 0x38))
  }else if(rq->wValue.bytes[0] == (HAVE_BOOTLOADER_HIDDENEXITCOMMAND)){  /* cause a bootLoaderExit at disconnect */
      stayinloader = 0xf1;  /* we need to be connected - so assume it */
#	endif
#endif
  }else{
      /* ignore all others, return default value == 0 */
  }
        
  return rval;
}


usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;
usbMsgLen_t     len = 0;
static uchar    replyBuffer[4];

    usbMsgPtr = (usbMsgPtr_t)replyBuffer;
    if(rq->bRequest == USBASP_FUNC_TRANSMIT){   /* emulate parts of ISP protocol */
        replyBuffer[3] = usbFunctionSetup_USBASP_FUNC_TRANSMIT(rq);
        len = (usbMsgLen_t)4;
    }else if((rq->bRequest == USBASP_FUNC_ENABLEPROG) || (rq->bRequest == USBASP_FUNC_SETISPSCK)){
        /* replyBuffer[0] = 0; is never touched and thus always 0 which means success */
        len = (usbMsgLen_t)1;
    }else if(rq->bRequest >= USBASP_FUNC_READFLASH && rq->bRequest <= USBASP_FUNC_SETLONGADDRESS){
        currentAddress.w[0] = rq->wValue.word;
        if(rq->bRequest == USBASP_FUNC_SETLONGADDRESS){
#if (FLASHEND) > 0xffff
            currentAddress.w[1] = rq->wIndex.word;
#endif
        }else{
            bytesRemaining = rq->wLength.bytes[0];
            /* if(rq->bRequest == USBASP_FUNC_WRITEFLASH) only evaluated during writeFlash anyway */
            isLastPage = rq->wIndex.bytes[1] & 0x02;
#if HAVE_EEPROM_PAGED_ACCESS
            currentRequest = rq->bRequest;
#endif
            len = USB_NO_MSG; /* hand over to usbFunctionRead() / usbFunctionWrite() */
        }

    }else if(rq->bRequest == USBASP_FUNC_DISCONNECT){

#if BOOTLOADER_CAN_EXIT
#	ifdef CONFIG_HAVE__BOOTLOADER_ABORTTIMEOUTONACT
      /* let the main loop know for ever that here was activity */
      stayinloader	   &= (0xfc);
#	else
      stayinloader	   &= (0xfe);
#	endif
#endif
    }else{
        /* ignore: others, but could be USBASP_FUNC_CONNECT */
#if BOOTLOADER_CAN_EXIT
	stayinloader	   |= (0x01);
#endif
    }
    return len;
}

#if (USE_EXCESSIVE_ASSEMBLER) && ((!HAVE_CHIP_ERASE) || (HAVE_ONDEMAND_PAGEERASE)) && (SPM_PAGESIZE <= 256) && (((BOOTLOADER_PAGEADDR>>0)&0xff) == 0)
uchar usbFunctionWrite(uchar *data, uchar len)
{
uchar   isLast;

    DBG1(0x31, (void *)&currentAddress.l, 4);
    if(len > bytesRemaining)
        len = bytesRemaining;
    bytesRemaining -= len;
    isLast = bytesRemaining == 0;
    if(currentRequest >= USBASP_FUNC_READEEPROM){
        uchar i;
        for(i = 0; i < len; i++){
            eeprom_write_byte((void *)(currentAddress.w[0]++), *data++);
        }
    }else{
	asm  volatile  (
	  "sbrc		%[len], 0\n\t"
	  "inc		%[len]\n\t"
"usbFunctionWrite_flashloop:\n\t"
	  "subi		%[len], 2\n\t"
	  "brlo		usbFunctionWrite_finished\n\t"
	  
#if HAVE_BLB11_SOFTW_LOCKBIT
	  "cpi		r31, %[blsaddrhi]\n\t"			/* accelerated BLB11_SOFTW_LOCKBIT check */
	  "brsh		usbFunctionWrite_finished\n\t"
// 	  "brlo		usbFunctionWrite_addrunlock_ok\n\t"
// 	  "brne		usbFunctionWrite_finished\n\t"
// 	  "cpi		r30, %[blsaddrlo]\n\t"
// 	  "brlo		usbFunctionWrite_addrunlock_ok\n\t"
// 	  "rjmp		usbFunctionWrite_finished\n\t"
// "usbFunctionWrite_addrunlock_ok:\n\t"
#endif
	  "rcall	usbFunctionWrite_waitA\n\t"
	  "cli\n\t"						/* r0 or r1 may be __zero_reg__ and may become dangerous nonzero within interrupts */
	  "ld		r0,		X+\n\t"
	  "ld		r1,		X+\n\t"

	  "ldi		r18,		%[pagfillval]\n\t"
	  "rcall	usbFunctionWrite_saveflash\n\t"	/* page fill */

	  "mov		r18,		r30\n\t"
	  "subi		r18,		0xfe\n\t"		/* add with 2 */
	  "andi		r18,		%[pagemask]\n\t"
	  "breq		usbFunctionWrite_pageisfull\n\t"
	  "tst		%[islast]\n\t"
	  "breq		usbFunctionWrite_skippageisfull\n\t"
	  "tst		%[isLastPage]\n\t"
	  "breq		usbFunctionWrite_skippageisfull\n\t"
	  "cpi		%[len],		0\n\t"
	  "brne		usbFunctionWrite_skippageisfull\n\t"

"usbFunctionWrite_pageisfull:\n\t"				/* start writing the page */
	  "ldi		r18,		%[pageraseval]\n\t"
	  "rcall	usbFunctionWrite_saveflash\n\t"	/* page erase */
	  "rcall	usbFunctionWrite_waitA\n\t"

	  "ldi		r18,		%[pagwriteval]\n\t"
	  "rcall	usbFunctionWrite_saveflash\n\t"	/* page write */
	  "rcall	usbFunctionWrite_waitA\n\t"

	  "in		__tmp_reg__,	%[spmcr]\n\t"
	  "sbrs		__tmp_reg__,	%[rwwsbbit]\n\t"
	  "rjmp		usbFunctionWrite_skippageisfull\n\t"
	  "ldi		r18,		%[rwwenrval]\n\t"
	  "rcall	usbFunctionWrite_saveflash\n\t"	/* reenable rww*/
// 	  "rcall	usbFunctionWrite_waitA\n\t"


"usbFunctionWrite_skippageisfull:\n\t"	  
	  "adiw		r30,		0x2\n\t"
	  "rjmp		usbFunctionWrite_flashloop\n\t"

"usbFunctionWrite_saveflash:\n\t"
	  "cli\n\t"
	  "out		%[spmcr],	r18\n\t"
	  "spm\n\t"
	  "clr		__zero_reg__\n\t"			/* if r0 or r1 is __zero_reg__ it may have become inconsisten while page-fill */
	  "sei\n\t"
	  "ret\n\t"

"usbFunctionWrite_waitA:\n\t"
	  "in		__tmp_reg__,	%[spmcr]\n\t"
	  "sbrc		__tmp_reg__,	%[spmenbit]\n\t"
	  "rjmp		usbFunctionWrite_waitA\n\t"
	  "ret\n\t"

"usbFunctionWrite_finished:\n\t"
	  : [addr]	   "+z" (currentAddress.l)

	  : [spmenbit]    "I" (SPMEN),
	    [rwwsbbit]    "I" (RWWSB),
	    [spmcr]       "I" (_SFR_IO_ADDR(__SPM_REG)),
	    [pagfillval]  "M" ((1<<SPMEN)),
	    [pageraseval] "M" ((1<<PGERS) | (1<<SPMEN)),
	    [pagwriteval] "M" ((1<<PGWRT) | (1<<SPMEN)),
	    [rwwenrval]   "M" ((1<<RWWSRE) | (1<<SPMEN)),
	    [pagemask]    "M" (SPM_PAGESIZE-1),
#if HAVE_BLB11_SOFTW_LOCKBIT
	    [blsaddrhi]	   "M" ((uint8_t)((BOOTLOADER_PAGEADDR>>8)&0xff)),
// 	    [blsaddrlo]	   "M" ((uint8_t)((BOOTLOADER_PAGEADDR>>0)&0xff)),
#endif
	    [islast]      "r"  (isLast),
	    [isLastPage]  "r"  (isLastPage),
	    [len]	   "d" (len),
	    [dataptr]	   "x" (data)

	    : "r0", "r1", "r18"
	);
    }
    return isLast;
}
#else
uchar usbFunctionWrite(uchar *data, uchar len)
{
uchar   i,isLast;

    DBG1(0x31, (void *)&currentAddress.l, 4);
    if(len > bytesRemaining)
        len = bytesRemaining;
    bytesRemaining -= len;
    isLast = bytesRemaining == 0;
    for(i = 0; i < len;) {
      if(currentRequest >= USBASP_FUNC_READEEPROM){
	eeprom_write_byte((void *)(currentAddress.w[0]++), *data++);
	i++;
      } else {
#if HAVE_BLB11_SOFTW_LOCKBIT
	if (CURRENT_ADDRESS >= (addr_t)(BOOTLOADER_PAGEADDR)) {
	  return 1;
	}
#endif
	i += 2;
	DBG1(0x32, 0, 0);
	cli();
	boot_page_fill(CURRENT_ADDRESS, *(short *)data);
	sei();
	CURRENT_ADDRESS += 2;
	data += 2;
	/* write page when we cross page boundary or we have the last partial page */
	if((currentAddress.w[0] & (SPM_PAGESIZE - 1)) == 0 || (isLast && i >= len && isLastPage)){
#if (!HAVE_CHIP_ERASE) || (HAVE_ONDEMAND_PAGEERASE)
	    DBG1(0x33, 0, 0);
#   ifndef NO_FLASH_WRITE
	    cli();
	    boot_page_erase(CURRENT_ADDRESS - 2);   /* erase page */
	    sei();
	    boot_spm_busy_wait();                   /* wait until page is erased */
#   endif
#endif
	    DBG1(0x34, 0, 0);
#ifndef NO_FLASH_WRITE
	    cli();
	    boot_page_write(CURRENT_ADDRESS - 2);
	    sei();
	    boot_spm_busy_wait();
	    cli();
	    boot_rww_enable();
	    sei();
#endif
	}
        }
        DBG1(0x35, (void *)&currentAddress.l, 4);
    }
    return isLast;
}
#endif

uchar usbFunctionRead(uchar *data, uchar len)
{
uchar   i;

    if(len > bytesRemaining)
        len = bytesRemaining;
    bytesRemaining -= len;
    for(i = 0; i < len; i++){
        if(currentRequest >= USBASP_FUNC_READEEPROM){
            *data = eeprom_read_byte((void *)currentAddress.w[0]);
        }else{
#if ((FLASHEND) > 65535)
            *data = pgm_read_byte_far(CURRENT_ADDRESS);
#else
            *data = pgm_read_byte(CURRENT_ADDRESS);
#endif
        }
        data++;
        CURRENT_ADDRESS++;
    }
    return len;
}

/* ------------------------------------------------------------------------ */

#if ((NEED_WATCHDOG) || (defined(__MCUCSR_COMPATMODE)))
#	define __MYWAIT_CYCLESperLOOP 5 /* cpu cycles per loop */
#else
#	define __MYWAIT_CYCLESperLOOP 4 /* cpu cycles per loop */
#endif
#	define __MYWAIT_CPLCONST (65536*__MYWAIT_CYCLESperLOOP) /* per waitloopcnt */

#if HAVE_UNPRECISEWAIT
#define _mydelay_ms(millisecs) _mywait(1+((((F_CPU/1000)*millisecs)/__MYWAIT_CYCLESperLOOP)/65536))
static void _mywait(uint8_t waitloopcnt) {
    asm volatile (
      /*we really don't care what value Z has...
       * ...if we loop 65536/F_CPU more or less...
       * ...unimportant - just save some opcodes
       */
#else
#define _mydelay_ms(millisecs) __DO_NOT_USE_DIRECTLY_mywait(0+((((F_CPU/1000)*millisecs)/__MYWAIT_CYCLESperLOOP)/65536), (uint16_t)(((uint32_t)(((F_CPU/1000)*millisecs)/__MYWAIT_CYCLESperLOOP))%(uint32_t)65536))
static void __DO_NOT_USE_DIRECTLY_mywait(uint8_t waitloopcnt, uint16_t remainder) {
    asm volatile (
#endif
"_mywait_sleeploop%=:					\n\t"
#if ((NEED_WATCHDOG) || (defined(__MCUCSR_COMPATMODE)))
      "wdr						\n\t"
#endif
      "sbiw	r30,	1				\n\t"
      "sbci	%[wlc],	0				\n\t"
      "brne	_mywait_sleeploop%=			\n\t"
#if HAVE_UNPRECISEWAIT
      : [wlc] "+d" (waitloopcnt)
      :
      : "r30","r31"
#else
      : [wlc] "+d" (waitloopcnt),
	[rem] "+z" (remainder)
      :
#endif
    );
}


static void initForUsbConnectivity(void)
{
    usbInit();
    /* enforce USB re-enumerate: */
    usbDeviceDisconnect();  /* do this while interrupts are disabled */
    _mydelay_ms(250);	/* fake USB disconnect for > 250 ms */
    usbDeviceConnect();
    sei();
}

int __attribute__((__noreturn__)) main(void)
{
#if ((BOOTLOADER_LOOPCYCLES_TIMEOUT) && (BOOTLOADER_CAN_EXIT))
    uint16_t __loopscycles;
    timeout_remaining = BOOTLOADER_LOOPCYCLES_TIMEOUT;
#endif
    /* initialize  */
    bootLoaderInit();
    odDebugInit();
    DBG1(0x00, 0, 0);
#ifndef NO_FLASH_WRITE
    GICR = (1 << IVCE);  /* enable change of interrupt vectors */
    GICR = (1 << IVSEL); /* move interrupts to boot flash section */
#endif
#if (HAVE_BOOTLOADER_ADDITIONALMSDEVICEWAIT>0)
    _mydelay_ms(HAVE_BOOTLOADER_ADDITIONALMSDEVICEWAIT);
#endif
    if(bootLoaderCondition()){
#if (BOOTLOADER_CAN_EXIT)
#	if (USE_EXCESSIVE_ASSEMBLER)
asm  volatile  (
  "ldi		%[sil],		%[normval]\n\t"
#		if ((defined(CONFIG_HAVE__BOOTLOADER_ABORTTIMEOUTONACT)) && (!(BOOTLOADER_IGNOREPROGBUTTON)) && (BOOTLOADER_LOOPCYCLES_TIMEOUT))
  "sbis		%[pin],		%[bit]\n\t"
  "subi		%[sil],		0x02\n\t"
#		endif
  : [sil]        "=d" (stayinloader)
  : [normval]     "M" (stayinloader_initialValue)
#		if (!(BOOTLOADER_IGNOREPROGBUTTON))
                                                    ,
    [pin]         "I" (_SFR_IO_ADDR(PIN_PIN(JUMPER_PORT))),
    [bit]         "I" (PIN(JUMPER_PORT, JUMPER_BIT))
#		endif    
);
#	else
#		if ((defined(CONFIG_HAVE__BOOTLOADER_ABORTTIMEOUTONACT)) && (!(BOOTLOADER_IGNOREPROGBUTTON)) && (BOOTLOADER_LOOPCYCLES_TIMEOUT))
      if (bootLoaderConditionSimple()) {
	stayinloader = stayinloader_initialValue - 0x02;
      } else
#		endif
	      stayinloader = stayinloader_initialValue;
#	endif
#endif
#if (__IMPLEMENT_PRESERVE_WATCHDOG)
	__original_WDTCR=WDTCR;
	if (__original_WDTCR & _BV(WDE)) {
	  wdt_enable(WDTO_2S);
	}
#else
	MCUCSR = 0;       /* clear all reset flags for next time */
#	if ((NEED_WATCHDOG) || (defined(__MCUCSR_COMPATMODE)))
	wdt_disable();    /* main app may have enabled watchdog */
#	endif
#endif
        initForUsbConnectivity();
        do{
#if ((BOOTLOADER_LOOPCYCLES_TIMEOUT) && (BOOTLOADER_CAN_EXIT))
#	ifdef CONFIG_HAVE__BOOTLOADER_ABORTTIMEOUTONACT
	if (stayinloader != 0x0e) {
	  /* can be reached, since high-nibble is decreased every cycle... */
#else
	if (stayinloader & 0x01) {
#endif
	  timeout_remaining = BOOTLOADER_LOOPCYCLES_TIMEOUT;
	} else {
	  __loopscycles++;
	  if (!(__loopscycles)) {
	    if(timeout_remaining)	timeout_remaining--;
	    else			stayinloader&=0xf1;
	  }
	}
#endif
#if (__IMPLEMENT_PRESERVE_WATCHDOG)
	    wdt_reset();
#endif
            usbPoll();
#if BOOTLOADER_CAN_EXIT
#if BOOTLOADER_IGNOREPROGBUTTON
  /* 
   * remove the high nibble as it would be subtracted due to:
   * "(!bootLoaderConditionSimple())"
   */ 
#if USE_EXCESSIVE_ASSEMBLER
asm  volatile  (
  "andi		%[sil],		0x0f\n\t"
  : [sil]        "+d" (stayinloader)
  :
);
#else
  stayinloader &= 0x0f;
#endif
#else
#if USE_EXCESSIVE_ASSEMBLER
asm  volatile  (
  "cpi		%[sil],		0x10\n\t"
  "brlo		main_stayinloader_smaller\n\t"
  "sbic		%[pin],		%[bit]\n\t"
  "subi		%[sil],		0x10\n\t"
  "rjmp		main_stayinloader_finished\n\t"
  
  "main_stayinloader_smaller:\n\t"
  "cpi		%[sil],		0x2\n\t"
  "brlo		main_stayinloader_finished\n\t"
  "sbis		%[pin],		%[bit]\n\t"
  "subi		%[sil],		0x2\n\t"

  "main_stayinloader_finished:\n\t"
  : [sil]        "+d" (stayinloader)
  : [pin]         "I" (_SFR_IO_ADDR(PIN_PIN(JUMPER_PORT))),
    [bit]         "I" (PIN(JUMPER_PORT, JUMPER_BIT))
);
#else
	if (stayinloader >= 0x10) {
	  if (!bootLoaderConditionSimple()) {
	    stayinloader-=0x10;
	  } 
	} else {
	  if (bootLoaderConditionSimple()) {
	    if (stayinloader > 1) stayinloader-=2;
	  }
	}
#endif
#endif
#endif

#if BOOTLOADER_CAN_EXIT
        }while (stayinloader);	/* main event loop, if BOOTLOADER_CAN_EXIT*/
#else
        }while (1);  		/* main event loop */
#endif
    }
    leaveBootloader();
}

/* ------------------------------------------------------------------------ */
