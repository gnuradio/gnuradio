/* --------------------------------------------------------------  */
/* (C)Copyright 2001,2007,                                         */
/* International Business Machines Corporation,                    */
/* Sony Computer Entertainment, Incorporated,                      */
/* Toshiba Corporation,                                            */
/*                                                                 */
/* All Rights Reserved.                                            */
/*                                                                 */
/* Redistribution and use in source and binary forms, with or      */
/* without modification, are permitted provided that the           */
/* following conditions are met:                                   */
/*                                                                 */
/* - Redistributions of source code must retain the above copyright*/
/*   notice, this list of conditions and the following disclaimer. */
/*                                                                 */
/* - Redistributions in binary form must reproduce the above       */
/*   copyright notice, this list of conditions and the following   */
/*   disclaimer in the documentation and/or other materials        */
/*   provided with the distribution.                               */
/*                                                                 */
/* - Neither the name of IBM Corporation nor the names of its      */
/*   contributors may be used to endorse or promote products       */
/*   derived from this software without specific prior written     */
/*   permission.                                                   */
/*                                                                 */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND          */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,     */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF        */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE        */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR            */
/* CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,    */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT    */
/* NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;    */
/* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)        */
/* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN       */
/* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR    */
/* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,  */
/* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.              */
/* --------------------------------------------------------------  */
/* PROLOG END TAG zYx                                              */
/*
 * sync_utils.h - SPU sync-library internal utilities.
 *
 * These utilities are used internally by the SPU sync library.
 */
#ifndef __SPU_SYNC_UTILS_H__
#define __SPU_SYNC_UTILS_H__

#include <spu_intrinsics.h>

typedef unsigned int u32;
typedef signed int s32;
typedef unsigned long long u64;
typedef unsigned long long eaddr_t;


typedef union {
    u64 ull;  
    u32 ui[2];
} addr64;

#ifndef likely
#define likely(_c)     \
    __builtin_expect((_c), 1)
#define unlikely(_c)    \
    __builtin_expect((_c), 0)
#endif

#define ALLOCA(_nbytes, _size)    \
    alloca((_nbytes) + (_size)-1)

#define ALIGN(_ptr, _size)       \
    ((((u32) _ptr) + (_size)-1) & ~((_size)-1))

#define ALIGN128_EA(_ull)	\
    ((_ull) & ~(127ULL))
/*
#define OFFSET128_EA(_ull, _type) \
    (((_ull) & 127ULL) / sizeof(_type))
    */

#define OFFSET128_EA_U32(_ull) \
    (((_ull) & 127ULL) >> 2)

#define OFFSET128_EA_U16(_ull) \
    (((_ull) & 127ULL) >> 1)

#define MFC_DMA(_ls, _ea, _sz, _tg, _cmd) \
    spu_mfcdma64(_ls, _ea.ui[0], _ea.ui[1], _sz, _tg, _cmd)

#define MFC_SYNC() {				\
    u32 _tagid = 0;				\
    spu_writech(mfc_tag_id, _tagid);		\
    spu_writech(mfc_cmd_queue, MFC_SYNC_CMD);	\
}

#define MFC_EIEIO() {				\
    u32 _tagid = 0;				\
    spu_writech(mfc_tag_id, _tagid);		\
    spu_writech(mfc_cmd_queue, MFC_EIEIO_CMD);	\
}

#endif /* __SPU_SYNC_UTILS_H__ */
