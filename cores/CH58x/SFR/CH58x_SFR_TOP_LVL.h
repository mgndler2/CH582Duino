#pragma once

#include "stdlib.h"
#include "stdint.h"

#ifdef CLK_OSC32K
#if ( CLK_OSC32K == 1 )
#define CAB_LSIFQ       32000
#else
#define CAB_LSIFQ       32768
#endif
#else
#define CAB_LSIFQ       32000
#endif

#define RAM_Base 0x20000000

#ifndef _nop
#define _nop() __asm__ volatile ("nop")
#endif

#ifndef __PACKED
#ifdef _MSC_VER
#define __PACKED
#else
#define __PACKED __attribute__((packed))
#endif
#endif

#ifndef __IAP_FLAG 
#ifdef _MSC_VER
#define __IAP_FLAG
#else
#define __IAP_FLAG __attribute__((section(".iapflag"),aligned(4)))
#endif // _MSC_VER
#endif // !__IAP_FLAG 


#ifndef __HIGH_CODE
#ifdef _MSC_VER
#define __HIGH_CODE
#else
#define __HIGH_CODE	__attribute__((section(".highcode")))
#endif // _MSC_VER
#endif

#ifndef __Allways_INLINE
#ifdef _MSC_VER
#define __Allways_INLINE
#else
#define __Allways_INLINE __attribute__((always_inline))
#endif // _MSC_VER
#endif // !__Allways_INLINE


#ifndef __INTERRUPT
#ifdef _MSC_VER
#define __INTERRUPT
#else
#define __INTERRUPT	__attribute__((interrupt)) __attribute__((section(".highcode")))
#endif
#endif

#define ISR(handler)											\
void handler(void) __attribute__((interrupt,	\
	 section(".highcode"),											\
	 aligned(4)));													\
void handler(void)

#ifdef _MSC_VER
#ifndef __aligned_four__
#define __aligned_four__
#endif // __aligned_four__
#else
#ifndef __aligned_four__
#define __aligned_four__	__aligned(4)
#endif // __aligned_four__
#endif // _MSC_VER

using Callback = void (*)(void*);