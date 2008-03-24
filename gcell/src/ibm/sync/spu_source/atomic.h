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
 * atomic.h - SPU atomic SHM counter operations.
 *
 * Interfaces patterned after, and hopefully compatible
 * with PowerPC64-Linux atomic counter operations.  Uses
 * 32b values for various counters.
 */
#ifndef _SPU_ATOMIC_H_
#define _SPU_ATOMIC_H_

#include <sync_utils.h>
#include <spu_mfcio.h>


/* atomic_eaddr_t is a 64bit effective address 
 * that points to an atomic_t variable */
typedef unsigned long long atomic_ea_t;

#define DECL_ATOMIC_VARS()                              \
    char _tmp[256];                                 	\
    char *tmp = (char *) ALIGN(_tmp, 128);		\
    volatile s32 *buf = (volatile s32 *) &tmp[0];       \
    u32 size = 128, tagid = 0;                          \
    s32 ret_val;                                	\
    u32 offset;                                         \
    addr64 ea64

/* __atomic_op                                              
*    Internal routine to acquire lock line reservation
*    then conditionally modify the counter variable 
*    pointed to by 'v'.  The 'replace' flag indicates 
*    whether or not this is to be an arithmetic R-M-W
*    or a simple replace operation.
*/
#define ATOMIC_OP(__v, __val, __replace, __final_val)           \
{                                                               \
    char __tmp[256];                                            \
    char *_tmp = (char *) ALIGN(__tmp, 128);		        \
    volatile s32 *_buf = (volatile s32 *) &_tmp[0];             \
    u32 _size = 128, _tagid = 0;                                \
    s32 _status, _ret_val;                                      \
    u32 _offset;                                                \
    addr64 _ea64;                                               \
                                                                \
    _ea64.ull = ALIGN128_EA(__v);                               \
    _offset = OFFSET128_EA_U32(__v);                            \
    do {                                                        \
	MFC_DMA(_buf, _ea64, _size, _tagid, MFC_GETLLAR_CMD);   \
        spu_readch (MFC_RdAtomicStat);                          \
                                                                \
	_ret_val = _buf[_offset];                                \
	_buf[_offset] = (__replace) ? __val : _ret_val + __val;    \
	MFC_DMA(_buf, _ea64, _size, _tagid, MFC_PUTLLC_CMD);    \
	_status = spu_readch(MFC_RdAtomicStat);                  \
    } while (_status != 0);                                     \
                                                                \
    __final_val = _ret_val;                                     \
}                                                               

#endif /* _SPU_ATOMIC_H_ */

