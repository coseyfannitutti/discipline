/* Name: iofixes.h
 * Project: USBaspLoader
 * Author: Stephan Baerwolf
 * Creation Date: 2013-12-16
 * Copyright: (c) 2013 by Stephan Baerwolf
 * License: GNU GPL v2 (see License.txt)
 * Version: 0.96.5-testing
 */

#ifndef __IOFIXES_H_afa5e75bcfb24ef4a91ed4e7fe26e869
#define __IOFIXES_H_afa5e75bcfb24ef4a91ed4e7fe26e869	1

/* 
 * WARNING: You should not edit this file !
 * 
 * The purpose of this file is the implementation of
 * workaround for known bugs in existing AVR toolchains.
 * 
 * Most of the time it will fix missing definitions
 * for registers and bitvalues.
 * 
 */

#include <avr/io.h>


/* ---------------------- Workarounds AVR IO BUGs ---------------------- */
#if defined (__AVR_ATmega88A__)

/* ATmega88A */

/* IVCE */
#	ifndef IVCE
#		warning IVCE not defined for ATmega88A - fixing
#		define IVCE	0
#	endif

/* IVSEL */
#	ifndef IVSEL
#		warning IVSEL not defined for ATmega88A - fixing
#		define IVSEL	1
#	endif

/* RWWSRE */
#	ifndef RWWSRE
#		warning RWWSRE not defined for ATmega88A - fixing
#		define RWWSRE	4
#	endif

/* RWWSB */
#	ifndef RWWSB
#		warning RWWSB not defined for ATmega88A - fixing
#		define RWWSB	6
#	endif







#elif defined (__AVR_ATmega168A__)

/* ATmega168A */

/* IVCE */
#	ifndef IVCE
#		warning IVCE not defined for ATmega168A - fixing
#		define IVCE	0
#	endif

/* IVSEL */
#	ifndef IVSEL
#		warning IVSEL not defined for ATmega168A - fixing
#		define IVSEL	1
#	endif

/* RWWSRE */
#	ifndef RWWSRE
#		warning RWWSRE not defined for ATmega168A - fixing
#		define RWWSRE	4
#	endif

/* RWWSB */
#	ifndef RWWSB
#		warning RWWSB not defined for ATmega168A - fixing
#		define RWWSB	6
#	endif



#endif


#endif
