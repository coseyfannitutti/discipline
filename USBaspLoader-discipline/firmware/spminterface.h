/* Name: spminterface.h
 * Project: USBaspLoader
 * Author: Stephan Baerwolf
 * Creation Date: 2012-08-01
 * Copyright: (c) 2013 by Stephan Baerwolf
 * License: GNU GPL v2 (see License.txt)
 * Version: 0.97.1
 */

#ifndef SPMINTERFACE_H_f70ba6adf7624275947e859bdbff0599
#define SPMINTERFACE_H_f70ba6adf7624275947e859bdbff0599

/*
 * spminterface.h offers a lightweight interface by inserting
 * an small machine-subroutine into the bootsection. (right 
 * after the interrupt-vector-table)
 * This subroutine can be called by normal code in order to 
 * enable it to program the flash (and depending on BLB11-lockbit
 * also the bootsection itself). Since SPM-calls from RWW-sections
 * will fail to work. The routine will be called "bootloader__do_spm".
 * Its principle assembler-code is depicted below (real code is a
 * little machinedependend).
 * Interfaces will be the 8-bit registers r10..r13, for details 
 * also see below. As also the pageaddress-registes (Z and rampZ)
 * are interfaced via different registers, it is possible to call
 * this routine via indirect call (icall).
 * Traditionally it is also possible to rcall, therefore you can
 * define "bootloader__do_spm" for your normal code via defsym at 
 * linking time.
 * Example for an atmega8: "-Wl,--defsym=transfer_point=0x1826"
 * (since BOOTLOADER_ADDRESS is 0x1800 and there are 
 * 2x19 = 38 = 0x26 byte for interrupts)
 * 

bootloader__do_spm:
;disable interrupts (if enabled) before calling!
;you may also want to disable wdt, since this routine may busy-loop
;==================================================================
;-->INPUT:
;#if HAVE_SPMINTEREFACE_MAGICVALUE
;magicvalue in                                    r23:r22:r21:r20
;#endif
;spmcr (spmcrval determines SPM action) will be register:	r18
;MCU dependend RA(MPZ should be transfered within register:	r11
;lo8(Z) should be transfered within register:			r12
;hi8(Z) should be transfered within register:			r13
;( as definition of SPM low8bit of dataword are stored within	r0 )
;( as definition of SPM hi8bit  of dataword are stored within	r1 )

;<-->USED/CHANGED:
;temp0 will be register:					r11
;temp1 will be register:					r12
;temp2 will be register:					r13
;spmcrval (r18) may also be changed due to rww reenable-phase	r18
;Z (r31:r30) wil be changed during operation

;<--OUT:
;==================================================================
; TODO: waitA and waitB could be merged to subroutine saving 2 opc
;==================================================================
;<magicvalue specific code (not depicted here)>
;load pageaddress (Z) from (r11:)r13:12 since it may was used for icall
mov	rampZ,	r11
mov	r30,	r12
mov	r31,	r13

waitA:			;check for pending SPM complete
in	temp0, SPMCR
sbrc	temp0, SPMEN
rjmp	waitA

out	SPMCR, spmcrval	;SPM timed sequence
spm

waitB:			;check for previous SPM complete
in	temp0, SPMCR
sbrc	temp0, SPMEN
rjmp	waitB

;avoid crash of userapplication
ldi	spmcrval, ((1<<RWWSRE) | (1<<SPMEN)) 
in	temp0,	  SPMCR
sbrc	temp0,	  RWWSB
rjmp	waitA

ret

*
*/ 

#include "bootloaderconfig.h"

#ifndef SREG
#	include <avr/io.h>
#endif


/*
 * This MACRO commands the linker to place correspondig
 * data on the top of the firmware. (Right after the 
 * interrupt-vector-table)
 * This is necessary to always locate the
 * "bootloader__do_spm" for example at 0x1826, even if
 * there are existing PROGMEM within the firmware...
 */
#define BOOTLIBLINK __attribute__ ((section (".vectors") ))


#ifndef funcaddr___bootloader__do_spm
  #if (defined(BOOTLOADER_ADDRESS)) && (!(defined(NEW_BOOTLOADER_ADDRESS)))
    #if HAVE_SPMINTEREFACE
      #define  funcaddr___bootloader__do_spm (&bootloader__do_spm)
    #endif  
  #else
    #if defined (__AVR_ATmega8535__)
      #define  funcaddr___bootloader__do_spm 0x182a
    #elif defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || defined (__AVR_ATmega8HVA__)
      #define  funcaddr___bootloader__do_spm 0x1826
    #elif defined (__AVR_ATmega16__)
      #define  funcaddr___bootloader__do_spm 0x3854
    #elif defined (__AVR_ATmega32__)
      #define  funcaddr___bootloader__do_spm 0x7054
    #elif defined (__AVR_ATmega88__) || defined (__AVR_ATmega88P__) || defined (__AVR_ATmega88A__) || defined (__AVR_ATmega88PA__)  
      #define  funcaddr___bootloader__do_spm 0x1834
    #elif defined (__AVR_ATmega162__)
      #define  funcaddr___bootloader__do_spm 0x3870
    #elif defined (__AVR_ATmega164A__) || defined (__AVR_ATmega164P__) || defined (__AVR_ATmega164PA__)  
      #define  funcaddr___bootloader__do_spm 0x387c
    #elif defined (__AVR_ATmega168__) || defined (__AVR_ATmega168P__) || defined (__AVR_ATmega168A__) || defined (__AVR_ATmega168PA__)  
      #define  funcaddr___bootloader__do_spm 0x3868
    #elif defined (__AVR_ATmega324A__) || defined (__AVR_ATmega324P__) || defined (__AVR_ATmega324PA__)
      #define  funcaddr___bootloader__do_spm 0x707c
    #elif defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
      #define  funcaddr___bootloader__do_spm 0x7068
    #elif defined (__AVR_ATmega640__)
      #define  funcaddr___bootloader__do_spm 0xe0e4
    #elif defined (__AVR_ATmega644__)
      #define  funcaddr___bootloader__do_spm 0xe070
    #elif defined (__AVR_ATmega644A__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__)
      #define  funcaddr___bootloader__do_spm 0xe07c
    #elif defined (__AVR_ATmega128__)
      #define  funcaddr___bootloader__do_spm 0x1e08c
    #elif defined (__AVR_ATmega1280__)
      #define  funcaddr___bootloader__do_spm 0x1e0e4
    #elif defined (__AVR_ATmega1281__)
      #define  funcaddr___bootloader__do_spm 0x1e0cc
    #elif defined (__AVR_ATmega1284__) || defined (__AVR_ATmega1284P__)
      #define  funcaddr___bootloader__do_spm 0x1e08c
    #elif defined (__AVR_ATmega2560__)
      #define  funcaddr___bootloader__do_spm 0x3e0e4
    #elif defined (__AVR_ATmega2561__)
      #define  funcaddr___bootloader__do_spm 0x3e0cc
    #else
      #error "unknown MCU - where is bootloader__do_spm located?"
    #endif

    #if ((defined(_VECTORS_SIZE)) && (defined(BOOTLOADER_ADDRESS)))
      #if (funcaddr___bootloader__do_spm != (BOOTLOADER_ADDRESS+_VECTORS_SIZE))
	#error "bootloader__do_spm is not located after interrupts - sth. is very wrong here!" 
      #endif
    #endif  

  #endif
#endif


#ifndef SPMEN
#define SPMEN SELFPRGEN
#endif

/*
 * Call the "bootloader__do_spm"-function, located within the BLS via comfortable C-interface
 * During operation code will block - disable or reset watchdog before call.
 * 
 * ATTANTION:	Since the underlying "bootloader__do_spm" will automatically reenable the
 * 		rww-section, only one way to program the flash will work.
 * 		(First erase the page, then fill temp. buffer, finally program...)
 * 		Since unblocking rww-section erases the temp. pagebuffer (which happens
 * 		after a page-erase), first programming this buffer does not help !!
 * 
 * REMEMBER: interrupts have to be disabled! (otherwise code may crash non-deterministic)
 * 
 */

#define __do_spm_Ex(arguments...)	__do_spm_GeneralEx(HAVE_SPMINTEREFACE_MAGICVALUE, ##arguments)


#if HAVE_SPMINTEREFACE_MAGICVALUE
#define __do_spm_GeneralEx	__do_spm_ExASMEx_magic
#else
#define __do_spm_GeneralEx	__do_spm_ExASMEx_
#endif


#if (defined(EIND) && ((FLASHEND)>131071))
  /*
  * Huge flash version using eicall (and EIND)
  * MV defines the magic value to be send to bootloader_do_spm
  */
  #define __do_spm_ExASMEx_(MV, flash_wordaddress, spmcrval, dataword, ___bootloader__do_spm__ptr)	\
  ({													\
      asm volatile (											\
      "push r0\n\t"  											\
      "push r1\n\t"  											\
													\
      "mov r13, %B[flashaddress]\n\t"									\
      "mov r12, %A[flashaddress]\n\t"									\
      "mov r11, %C[flashaddress]\n\t"									\
													\
      /* prepare the EIND for following eicall */							\
      "in r18, %[eind]\n\t"										\
      "push r18\n\t" 											\
      "ldi r18, %[spmfuncaddrEIND]\n\t"									\
      "out %[eind], r18\n\t"										\
													\
      /* also load the spmcrval */									\
      "mov r18, %[spmcrval]\n\t"									\
													\
													\
      "mov r1, %B[data]\n\t"										\
      "mov r0, %A[data]\n\t"										\
													\
      /* finally call the bootloader-function */							\
      "eicall\n\t"											\
      "pop r1\n\t"  											\
      "out %[eind], r1\n\t"										\
													\
      /*												\
      * bootloader__do_spm should change spmcrval (r18) to						\
      * "((1<<RWWSRE) | (1<<SPMEN))" in case of success							\
      */												\
      "cpi r18, %[spmret]\n\t"										\
      /* loop infitinte if not so, most likely we called an bootloader__do_spm				\
	* with wrong magic! To avoid calls to wrong initialized pages, better crash here...		\
	*/												\
  "loop%=: \n\t"											\
      "brne loop%= \n\t"										\
													\
      "pop  r1\n\t"  											\
      "pop  r0\n\t"  											\
													\
      :													\
      : [flashaddress]		"r" (flash_wordaddress),						\
	[spmfunctionaddress]	"z" ((uint16_t)(___bootloader__do_spm__ptr)),				\
	[spmfuncaddrEIND]	"M" ((uint8_t)(___bootloader__do_spm__ptr>>16)),			\
	[eind]			"I" (_SFR_IO_ADDR(EIND)),						\
	[spmcrval]		"r" (spmcrval),								\
	[data]			"r" (dataword),								\
	[spmret]		"M" ((1<<RWWSRE) | (1<<SPMEN))						\
      : "r0","r1","r11","r12","r13","r18"								\
      );												\
  })

  #define __do_spm_ExASMEx_magic(MV, flash_wordaddress, spmcrval, dataword, ___bootloader__do_spm__ptr)	\
  ({													\
      asm volatile (											\
      "push r0\n\t"  											\
      "push r1\n\t"  											\
													\
      "ldi r23, %[magicD] \n\t"										\
      "ldi r22, %[magicC] \n\t"										\
      "ldi r21, %[magicB] \n\t"										\
      "ldi r20, %[magicA] \n\t"										\
													\
      "mov r13, %B[flashaddress]\n\t"									\
      "mov r12, %A[flashaddress]\n\t"									\
      "mov r11, %C[flashaddress]\n\t"									\
													\
      /* prepare the EIND for following eicall */							\
      "in r18, %[eind]\n\t"										\
      "push r18\n\t"  											\
      "ldi r18, %[spmfuncaddrEIND]\n\t"									\
      "out %[eind], r18\n\t"										\
													\
      /* also load the spmcrval */									\
      "mov r18, %[spmcrval]\n\t"									\
													\
													\
      "mov r1, %B[data]\n\t"										\
      "mov r0, %A[data]\n\t"										\
													\
      /* finally call the bootloader-function */							\
      "eicall\n\t"											\
      "pop r1\n\t"  											\
      "out %[eind], r1\n\t"										\
													\
      /*												\
      * bootloader__do_spm should change spmcrval (r18) to						\
      * "((1<<RWWSRE) | (1<<SPMEN))" in case of success							\
      */												\
      "cpi r18, %[spmret]\n\t"										\
      /* loop infitinte if not so, most likely we called an bootloader__do_spm				\
	* with wrong magic! To avoid calls to wrong initialized pages, better crash here...		\
	*/												\
  "loop%=: \n\t"											\
      "brne loop%= \n\t"										\
													\
      "pop  r1\n\t"  											\
      "pop  r0\n\t"  											\
													\
      :													\
      : [flashaddress] "r" (flash_wordaddress),								\
	[spmfunctionaddress] "z" ((uint16_t)(___bootloader__do_spm__ptr)),				\
	[spmfuncaddrEIND]	"M" ((uint8_t)(___bootloader__do_spm__ptr>>16)),			\
	[eind]			"I" (_SFR_IO_ADDR(EIND)),						\
	[spmcrval] "r" (spmcrval),									\
	[data] "r" (dataword),										\
	[spmret] "M" ((1<<RWWSRE) | (1<<SPMEN)),							\
	[magicD] "M" (((MV)>>24)&0xff),									\
	[magicC] "M" (((MV)>>16)&0xff),									\
	[magicB] "M" (((MV)>> 8)&0xff),									\
	[magicA] "M" (((MV)>> 0)&0xff)									\
      : "r0","r1","r11","r12","r13","r18","r20","r21","r22","r23"					\
      );												\
  })


#else
  /*
  * Normal version for devices with <=128KiB flash (using icall)
  */
  #if ((FLASHEND)>131071)
    #error "Using inappropriate code for device with more than 128kib flash"
  #endif
  
  #define __do_spm_ExASMEx_(MV, flash_wordaddress, spmcrval, dataword, ___bootloader__do_spm__ptr)	\
  ({													\
      asm volatile (											\
      "push r0\n\t"  											\
      "push r1\n\t"  											\
													\
      "mov r13, %B[flashaddress]\n\t"									\
      "mov r12, %A[flashaddress]\n\t"									\
      "mov r11, %C[flashaddress]\n\t"									\
													\
      /* also load the spmcrval */									\
      "mov r18, %[spmcrval]\n\t"									\
													\
													\
      "mov r1, %B[data]\n\t"										\
      "mov r0, %A[data]\n\t"										\
													\
      /* finally call the bootloader-function */							\
      "icall\n\t"											\
													\
      /*												\
      * bootloader__do_spm should change spmcrval (r18) to						\
      * "((1<<RWWSRE) | (1<<SPMEN))" in case of success							\
      */												\
      "cpi r18, %[spmret]\n\t"										\
      /* loop infitinte if not so, most likely we called an bootloader__do_spm				\
	* with wrong magic! To avoid calls to wrong initialized pages, better crash here...		\
	*/												\
  "loop%=: \n\t"											\
      "brne loop%= \n\t"										\
													\
      "pop  r1\n\t"  											\
      "pop  r0\n\t"  											\
													\
      :													\
      : [flashaddress] "r" (flash_wordaddress),								\
	[spmfunctionaddress] "z" ((uint16_t)(___bootloader__do_spm__ptr)),				\
	[spmcrval] "r" (spmcrval),									\
	[data] "r" (dataword),										\
	[spmret] "M" ((1<<RWWSRE) | (1<<SPMEN))								\
      : "r0","r1","r11","r12","r13","r18"								\
      );												\
  })

  #define __do_spm_ExASMEx_magic(MV, flash_wordaddress, spmcrval, dataword, ___bootloader__do_spm__ptr)	\
  ({													\
      asm volatile (											\
      "push r0\n\t"  											\
      "push r1\n\t"  											\
													\
      "ldi r23, %[magicD] \n\t"										\
      "ldi r22, %[magicC] \n\t"										\
      "ldi r21, %[magicB] \n\t"										\
      "ldi r20, %[magicA] \n\t"										\
													\
      "mov r13, %B[flashaddress]\n\t"									\
      "mov r12, %A[flashaddress]\n\t"									\
      "mov r11, %C[flashaddress]\n\t"									\
													\
      /* also load the spmcrval */									\
      "mov r18, %[spmcrval]\n\t"									\
													\
													\
      "mov r1, %B[data]\n\t"										\
      "mov r0, %A[data]\n\t"										\
													\
      /* finally call the bootloader-function */							\
      "icall\n\t"											\
													\
      /*												\
      * bootloader__do_spm should change spmcrval (r18) to						\
      * "((1<<RWWSRE) | (1<<SPMEN))" in case of success							\
      */												\
      "cpi r18, %[spmret]\n\t"										\
      /* loop infitinte if not so, most likely we called an bootloader__do_spm				\
	* with wrong magic! To avoid calls to wrong initialized pages, better crash here...   		\
	*/												\
  "loop%=: \n\t"											\
      "brne loop%= \n\t"										\
													\
      "pop  r1\n\t"  											\
      "pop  r0\n\t"  											\
													\
      :													\
      : [flashaddress] "r" (flash_wordaddress),								\
	[spmfunctionaddress] "z" ((uint16_t)(___bootloader__do_spm__ptr)),				\
	[spmcrval] "r" (spmcrval),									\
	[data] "r" (dataword),										\
	[spmret] "M" ((1<<RWWSRE) | (1<<SPMEN)),							\
	[magicD] "M" (((MV)>>24)&0xff),									\
	[magicC] "M" (((MV)>>16)&0xff),									\
	[magicB] "M" (((MV)>> 8)&0xff),									\
	[magicA] "M" (((MV)>> 0)&0xff)									\
      : "r0","r1","r11","r12","r13","r18","r20","r21","r22","r23"					\
      );												\
  })
#endif

#if (!(defined(BOOTLOADER_ADDRESS))) || (defined(NEW_BOOTLOADER_ADDRESS))
void do_spm(const uint32_t flash_byteaddress, const uint8_t spmcrval, const uint16_t dataword) {
    __do_spm_Ex(flash_byteaddress, spmcrval, dataword, funcaddr___bootloader__do_spm >> 1);
}
#endif

#if HAVE_SPMINTEREFACE_NORETMAGIC
  #define bootloader__do_spm_magic_exitstrategy(a) (0xf7f9)
#else
  #define bootloader__do_spm_magic_exitstrategy(a) (a)
#endif

#if (HAVE_SPMINTEREFACE) && (defined(BOOTLOADER_ADDRESS)) && (!(defined(NEW_BOOTLOADER_ADDRESS)))

/*
 * insert architecture dependend "bootloader_do_spm"-code
 */
#if defined (__AVR_ATmega8535__) || defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || defined (__AVR_ATmega8HVA__) || defined (__AVR_ATmega16__) || defined (__AVR_ATmega162__) || defined (__AVR_ATmega32__)

#if defined (__AVR_ATmega8535__) || defined (__AVR_ATmega8__) || defined (__AVR_ATmega8A__) || defined (__AVR_ATmega8HVA__)
  #if (BOOTLOADER_ADDRESS != 0x1800)
    #error BOOTLOADER_ADDRESS!=0x1800, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega16__) || defined (__AVR_ATmega162__)
  #if (BOOTLOADER_ADDRESS != 0x3800)
    #error BOOTLOADER_ADDRESS!=0x3800, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega32__)
  #if (BOOTLOADER_ADDRESS != 0x7000)
    #error BOOTLOADER_ADDRESS!=0x7000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#else
  #error undefined device selection - this should not happen! 
#endif

//assume  SPMCR==0x37, SPMEN==0x0, RWWSRE=0x4, RWWSB=0x6
#if HAVE_SPMINTEREFACE_MAGICVALUE
const uint16_t bootloader__do_spm[23] BOOTLIBLINK = {
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 28) & 0xf))<<8) | (0x70 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 24) & 0xf))), // r23
  bootloader__do_spm_magic_exitstrategy(0xf4a1), // brne +20
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 20) & 0xf))<<8) | (0x60 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 16) & 0xf))), // r22
  bootloader__do_spm_magic_exitstrategy(0xf491), // brne +18
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 12) & 0xf))<<8) | (0x50 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  8) & 0xf))), // r21
  bootloader__do_spm_magic_exitstrategy(0xf481), // brne +16
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  4) & 0xf))<<8) | (0x40 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  0) & 0xf))), // r20
  bootloader__do_spm_magic_exitstrategy(0xf471), // brne +14
#else
const uint16_t bootloader__do_spm[15] BOOTLIBLINK = {
#endif
  0x2dec, 0x2dfd, 0xb6b7, 0xfcb0, 0xcffd, 0xbf27, 0x95e8, 0xb6b7,
  0xfcb0, 0xcffd, 0xe121, 0xb6b7, 0xfcb6, 0xcff4, 0x9508
};

/*
00001826 <bootloader__do_spm>:
    1828:	ec 2d       	mov	r30, r12
    182a:	fd 2d       	mov	r31, r13

0000182c <waitA>:
    182c:	b7 b6       	in	r11, 0x37	; 55
    182e:	b0 fc       	sbrc	r11, 0
    1830:	fd cf       	rjmp	.-6      	; 0x182c <waitA>
    1832:	27 bf       	out	0x37, r18	; 55
    1834:	e8 95       	spm

00001836 <waitB>:
    1836:	b7 b6       	in	r11, 0x37	; 55
    1838:	b0 fc       	sbrc	r11, 0
    183a:	fd cf       	rjmp	.-6      	; 0x1836 <waitB>
    183c:	21 e1       	ldi	r18, 0x11	; 17
    183e:	b7 b6       	in	r11, 0x37	; 55
    1840:	b6 fc       	sbrc	r11, 6
    1842:	f4 cf       	rjmp	.-24     	; 0x182c <waitA>
    1844:	08 95       	ret
*/





#elif defined (__AVR_ATmega48__) || defined (__AVR_ATmega48P__) || defined (__AVR_ATmega88__) || defined (__AVR_ATmega88P__) || defined (__AVR_ATmega168__) || defined (__AVR_ATmega168P__)

#if defined (__AVR_ATmega88__) || defined (__AVR_ATmega88P__)
  #if (BOOTLOADER_ADDRESS != 0x1800)
    #error BOOTLOADER_ADDRESS!=0x1800, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega168__) || defined (__AVR_ATmega168P__)
  #if (BOOTLOADER_ADDRESS != 0x3800)
    #error BOOTLOADER_ADDRESS!=0x3800, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#else
  #error undefined device selection - this should not happen! 
#endif

//assume  SPMCR:=SPMCSR==0x37, SPMEN:=SELFPRGEN==0x0, RWWSRE=0x4, RWWSB=0x6
#if HAVE_SPMINTEREFACE_MAGICVALUE
const uint16_t bootloader__do_spm[23] BOOTLIBLINK = {
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 28) & 0xf))<<8) | (0x70 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 24) & 0xf))), // r23
  bootloader__do_spm_magic_exitstrategy(0xf4a1), // brne +20
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 20) & 0xf))<<8) | (0x60 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 16) & 0xf))), // r22
  bootloader__do_spm_magic_exitstrategy(0xf491), // brne +18
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 12) & 0xf))<<8) | (0x50 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  8) & 0xf))), // r21
  bootloader__do_spm_magic_exitstrategy(0xf481), // brne +16
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  4) & 0xf))<<8) | (0x40 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  0) & 0xf))), // r20
  bootloader__do_spm_magic_exitstrategy(0xf471), // brne +14
#else
const uint16_t bootloader__do_spm[15] BOOTLIBLINK = {
#endif
  0x2dec, 0x2dfd, 0xb6b7, 0xfcb0, 0xcffd, 0xbf27, 0x95e8, 0xb6b7,
  0xfcb0, 0xcffd, 0xe121, 0xb6b7, 0xfcb6, 0xcff4, 0x9508
};
/*
00001826 <bootloader__do_spm>:
    1828:	ec 2d       	mov	r30, r12
    182a:	fd 2d       	mov	r31, r13

0000182c <waitA>:
    182c:	b7 b6       	in	r11, 0x37	; 55
    182e:	b0 fc       	sbrc	r11, 0
    1830:	fd cf       	rjmp	.-6      	; 0x182c <waitA>
    1832:	27 bf       	out	0x37, r18	; 55
    1834:	e8 95       	spm

00001836 <waitB>:
    1836:	b7 b6       	in	r11, 0x37	; 55
    1838:	b0 fc       	sbrc	r11, 0
    183a:	fd cf       	rjmp	.-6      	; 0x1836 <waitB>
    183c:	21 e1       	ldi	r18, 0x11	; 17
    183e:	b7 b6       	in	r11, 0x37	; 55
    1840:	b6 fc       	sbrc	r11, 6
    1842:	f4 cf       	rjmp	.-24     	; 0x182c <waitA>
    1844:	08 95       	ret
*/





#elif defined (__AVR_ATmega48A__) || defined (__AVR_ATmega48PA__) || defined (__AVR_ATmega88A__) || defined (__AVR_ATmega88PA__) || defined (__AVR_ATmega168A__) || defined (__AVR_ATmega168PA__) || defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)

#if defined (__AVR_ATmega88A__) || defined (__AVR_ATmega88PA__)
  #if (BOOTLOADER_ADDRESS != 0x1800)
    #error BOOTLOADER_ADDRESS!=0x1800, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega168A__) || defined (__AVR_ATmega168PA__)
  #if (BOOTLOADER_ADDRESS != 0x3800)
    #error BOOTLOADER_ADDRESS!=0x3800, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega328__) || defined (__AVR_ATmega328P__)
  #if (BOOTLOADER_ADDRESS != 0x7000)
    #error BOOTLOADER_ADDRESS!=0x7000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#else
  #error undefined device selection - this should not happen! 
#endif

//assume  SPMCR:=SPMCSR==0x37, SPMEN:=SELFPRGEN==0x0, RWWSRE=0x4, RWWSB=0x6
#if HAVE_SPMINTEREFACE_MAGICVALUE
const uint16_t bootloader__do_spm[23] BOOTLIBLINK = {
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 28) & 0xf))<<8) | (0x70 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 24) & 0xf))), // r23
  bootloader__do_spm_magic_exitstrategy(0xf4a1), // brne +20
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 20) & 0xf))<<8) | (0x60 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 16) & 0xf))), // r22
  bootloader__do_spm_magic_exitstrategy(0xf491), // brne +18
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 12) & 0xf))<<8) | (0x50 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  8) & 0xf))), // r21
  bootloader__do_spm_magic_exitstrategy(0xf481), // brne +16
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  4) & 0xf))<<8) | (0x40 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  0) & 0xf))), // r20
  bootloader__do_spm_magic_exitstrategy(0xf471), // brne +14
#else
const uint16_t bootloader__do_spm[15] BOOTLIBLINK = {
#endif
  0x2dec, 0x2dfd, 0xb6b7, 0xfcb0, 0xcffd, 0xbf27, 0x95e8, 0xb6b7,
  0xfcb0, 0xcffd, 0xe121, 0xb6b7, 0xfcb6, 0xcff4, 0x9508
};
/*
00001826 <bootloader__do_spm>:
    1828:	ec 2d       	mov	r30, r12
    182a:	fd 2d       	mov	r31, r13

0000182c <waitA>:
    182c:	b7 b6       	in	r11, 0x37	; 55
    182e:	b0 fc       	sbrc	r11, 0
    1830:	fd cf       	rjmp	.-6      	; 0x182c <waitA>
    1832:	27 bf       	out	0x37, r18	; 55
    1834:	e8 95       	spm

00001836 <waitB>:
    1836:	b7 b6       	in	r11, 0x37	; 55
    1838:	b0 fc       	sbrc	r11, 0
    183a:	fd cf       	rjmp	.-6      	; 0x1836 <waitB>
    183c:	21 e1       	ldi	r18, 0x11	; 17
    183e:	b7 b6       	in	r11, 0x37	; 55
    1840:	b6 fc       	sbrc	r11, 6
    1842:	f4 cf       	rjmp	.-24     	; 0x182c <waitA>
    1844:	08 95       	ret
*/





#elif defined (__AVR_ATmega128__)

#if defined (__AVR_ATmega128__)
  #if (BOOTLOADER_ADDRESS != 0x1E000)
    #error BOOTLOADER_ADDRESS!=0x1E000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#else
  #error undefined device selection - this should not happen! 
#endif

//assume  SPMCR:=SPMCSR==0x68, SPMEN==0x0, RWWSRE=0x4, RWWSB=0x6 and rampZ=0x3b
#if HAVE_SPMINTEREFACE_MAGICVALUE
const uint16_t bootloader__do_spm[28] BOOTLIBLINK = {
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 28) & 0xf))<<8) | (0x70 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 24) & 0xf))), // r23
  bootloader__do_spm_magic_exitstrategy(0xf4c9), // brne +21+4
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 20) & 0xf))<<8) | (0x60 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 16) & 0xf))), // r22
  bootloader__do_spm_magic_exitstrategy(0xf4b9), // brne +19+4
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 12) & 0xf))<<8) | (0x50 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  8) & 0xf))), // r21
  bootloader__do_spm_magic_exitstrategy(0xf4a9), // brne +17+4
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  4) & 0xf))<<8) | (0x40 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  0) & 0xf))), // r20
  bootloader__do_spm_magic_exitstrategy(0xf499), // brne +15+4
#else
const uint16_t bootloader__do_spm[20] BOOTLIBLINK = {
#endif
  0xbebb, 0x2dec, 0x2dfd, 0x90b0, 0x0068, 0xfcb0, 0xcffc, 0x9320, 0x0068,
  0x95e8, 0x90b0, 0x0068, 0xfcb0, 0xcffc, 0xe121, 0x90b0, 0x0068, 0xfcb6,
  0xcff0, 0x9508
};
/*
0001e08c <bootloader__do_spm>:
   1e08c:       bb be           out     0x3b, r11       ; 59
   1e08e:       ec 2d           mov     r30, r12
   1e090:       fd 2d           mov     r31, r13

0001e092 <waitA>:
   1e092:       b0 90 68 00     lds     r11, 0x0068
   1e096:       b0 fc           sbrc    r11, 0
   1e098:       fc cf           rjmp    .-8             ; 0x1e092 <waitA>
   1e09a:       20 93 68 00     sts     0x0068, r18
   1e09e:       e8 95           spm

0001e0a0 <waitB>:
   1e0a0:       b0 90 68 00     lds     r11, 0x0068
   1e0a4:       b0 fc           sbrc    r11, 0
   1e0a6:       fc cf           rjmp    .-8             ; 0x1e0a0 <waitB>
   1e0a8:       21 e1           ldi     r18, 0x11       ; 17
   1e0aa:       b0 90 68 00     lds     r11, 0x0068
   1e0ae:       b6 fc           sbrc    r11, 6
   1e0b0:       f0 cf           rjmp    .-32            ; 0x1e092 <waitA>
   1e0b2:       08 95           ret
*/





#elif defined (__AVR_ATmega164A__) || defined (__AVR_ATmega164P__) || defined (__AVR_ATmega164PA__) || defined (__AVR_ATmega324A__) || defined (__AVR_ATmega324P__) || defined (__AVR_ATmega324PA__) || defined (__AVR_ATmega640__) || defined (__AVR_ATmega644__) || defined (__AVR_ATmega644A__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__) || defined (__AVR_ATmega1280__) || defined (__AVR_ATmega1281__) || defined (__AVR_ATmega1284__) || defined (__AVR_ATmega1284P__) || defined (__AVR_ATmega2560__) || defined (__AVR_ATmega2561__)

#if defined (__AVR_ATmega164A__) || defined (__AVR_ATmega164P__) || defined (__AVR_ATmega164PA__)
  #if (BOOTLOADER_ADDRESS != 0x3800)
    #error BOOTLOADER_ADDRESS!=0x3800, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega324A__) || defined (__AVR_ATmega324P__) || defined (__AVR_ATmega324PA__)
  #if (BOOTLOADER_ADDRESS != 0x7000)
    #error BOOTLOADER_ADDRESS!=0x7000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega640__)
  #if (BOOTLOADER_ADDRESS != 0xE000)
    #error BOOTLOADER_ADDRESS!=0xE000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega644__) || defined (__AVR_ATmega644A__) || defined (__AVR_ATmega644P__) || defined (__AVR_ATmega644PA__)
  #if (BOOTLOADER_ADDRESS != 0xE000)
    #error BOOTLOADER_ADDRESS!=0xE000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega1280__)
  #if (BOOTLOADER_ADDRESS != 0x1E000)
    #error BOOTLOADER_ADDRESS!=0x1E000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega1281__)
  #if (BOOTLOADER_ADDRESS != 0x1E000)
    #error BOOTLOADER_ADDRESS!=0x1E000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega1284__) || defined (__AVR_ATmega1284P__)
  #if (BOOTLOADER_ADDRESS != 0x1E000)
    #error BOOTLOADER_ADDRESS!=0x1E000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega2560__)
  #if (BOOTLOADER_ADDRESS != 0x3E000)
    #error BOOTLOADER_ADDRESS!=0x3E000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#elif defined (__AVR_ATmega2561__)
  #if (BOOTLOADER_ADDRESS != 0x3E000)
    #error BOOTLOADER_ADDRESS!=0x3E000, on current MCU "funcaddr___bootloader__do_spm" might be currupted - please edit spminterface.h for nonstandard use
  #endif
#else
  #error undefined device selection - this should not happen! 
#endif

//assume  SPMCR:=SPCSR==0x37, SPMEN==0x0, RWWSRE=0x4, RWWSB=0x6 and rampZ=0x3b
#if HAVE_SPMINTEREFACE_MAGICVALUE
const uint16_t bootloader__do_spm[24] BOOTLIBLINK = {
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 28) & 0xf))<<8) | (0x70 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 24) & 0xf))), // r23
  bootloader__do_spm_magic_exitstrategy(0xf4a9), // brne +21
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 20) & 0xf))<<8) | (0x60 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 16) & 0xf))), // r22
  bootloader__do_spm_magic_exitstrategy(0xf499), // brne +19
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >> 12) & 0xf))<<8) | (0x50 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  8) & 0xf))), // r21
  bootloader__do_spm_magic_exitstrategy(0xf489), // brne +17
  (((0x30 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  4) & 0xf))<<8) | (0x40 | ((HAVE_SPMINTEREFACE_MAGICVALUE >>  0) & 0xf))), // r20
  bootloader__do_spm_magic_exitstrategy(0xf479), // brne +15
#else
const uint16_t bootloader__do_spm[16] BOOTLIBLINK = {
#endif
  0xbebb,
  0x2dec, 0x2dfd, 0xb6b7, 0xfcb0, 0xcffd, 0xbf27, 0x95e8, 0xb6b7,
  0xfcb0, 0xcffd, 0xe121, 0xb6b7, 0xfcb6, 0xcff4, 0x9508
};
/*
00001826 <bootloader__do_spm>:
    1826:	bb be       	out	0x3b,r11	; rampZ=r11; (rampZ is at IO 0x3b)
    1828:	ec 2d       	mov	r30, r12
    182a:	fd 2d       	mov	r31, r13

0000182c <waitA>:
    182c:	b7 b6       	in	r11, 0x37	; 55
    182e:	b0 fc       	sbrc	r11, 0
    1830:	fd cf       	rjmp	.-6      	; 0x182c <waitA>
    1832:	27 bf       	out	0x37, r18	; 55
    1834:	e8 95       	spm

00001836 <waitB>:
    1836:	b7 b6       	in	r11, 0x37	; 55
    1838:	b0 fc       	sbrc	r11, 0
    183a:	fd cf       	rjmp	.-6      	; 0x1836 <waitB>
    183c:	21 e1       	ldi	r18, 0x11	; 17
    183e:	b7 b6       	in	r11, 0x37	; 55
    1840:	b6 fc       	sbrc	r11, 6
    1842:	f4 cf       	rjmp	.-24     	; 0x182c <waitA>
    1844:	08 95       	ret
*/





#else
  #error "bootloader__do_spm has to be adapted, since there is no architecture code, yet"
#endif  


#endif

#endif
						 
