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
#ifndef _SPU_MUTEX_H_
#define _SPU_MUTEX_H_           1

#include "sync_utils.h"
#include "sync_irq.h"
#include <spu_mfcio.h>

typedef eaddr_t mutex_ea_t;

#define DECL_MUTEX_VARS()                               \
    char _tmp[256];                                     \
    char *tmp = (char *) ALIGN(_tmp, 128);              \
    volatile s32 *buf = (volatile s32 *) &tmp[0];       \
    u32 size = 128, tagid = 0;                          \
    u32 offset;                                         \
    addr64 ea64

/* RAW_TEST_AND_SET
 *    Macro implementing the test and set primitive.
 *
 *    RAW_TEST_AND_SET(==, 1)	used by spin_try_lock()
 *    RAW_TEST_AND_SET(>=, 1)	used by read_try_lock()
 *    RAW_TEST_AND_SET(==, -1)	used by write_try_lock()
 */
#define RAW_TEST_AND_SET(_RELOP_, _val)				\
    ea64.ull = ALIGN128_EA(ea);					\
    offset = OFFSET128_EA_U32(ea);				\
    do {							\
        MFC_DMA(buf, ea64, size, tagid, MFC_GETLLAR_CMD);	\
        spu_readch(MFC_RdAtomicStat);				\
        SET_HIT;						\
        if (likely(buf[offset] _RELOP_ 0)) {			\
            buf[offset] += _val;				\
	    MFC_DMA(buf, ea64, size, tagid, MFC_PUTLLC_CMD);	\
            status = spu_readch(MFC_RdAtomicStat);		\
            ret_val = 1;					\
        } else {						\
	    MFC_DMA(buf, ea64, size, tagid, MFC_PUTLLC_CMD);	\
            spu_readch(MFC_RdAtomicStat);			\
            status = ret_val = 0;				\
            break;						\
        }							\
    } while (status != 0)

/* RAW_SPINLOCK
 *    Macro implementing the spinlock primitive.
 *
 *    RAW_SPINLOCK(==, 1)	used by spin_lock()
 *    RAW_SPINLOCK(>=, 1)	used by read_lock()
 *    RAW_SPINLOCK(==, -1)	used by write_lock()
 */


#define RAW_SPINLOCK(_RELOP_, _val) 				\
    ea64.ull = ALIGN128_EA(ea);					\
    offset = OFFSET128_EA_U32(ea);				\
    do {							\
        MFC_DMA(buf, ea64, size, tagid, MFC_GETLLAR_CMD);	\
        spu_readch(MFC_RdAtomicStat);				\
        SET_HIT;						\
        status = 1;						\
        if (likely(buf[offset] _RELOP_ 0)) {			\
            buf[offset] += _val;				\
	    MFC_DMA(buf, ea64, size, tagid, MFC_PUTLLC_CMD);	\
            status = spu_readch(MFC_RdAtomicStat);		\
        }							\
    } while (status != 0);


static inline void _lock_init(eaddr_t ea)
{
    DECL_MUTEX_VARS();
    u32 oldtmask, tagmask;
  
    tagid = mfc_tag_reserve(); 
   
    tagmask = 1 << (tagid & 31);

    /* __lock_init
     *    Internal routine to initialize a spinlock or
     *    reader/writer lock.
     */
    ea64.ull = ALIGN128_EA(ea);
    offset = OFFSET128_EA_U32(ea);
    MFC_DMA(buf, ea64, size, tagid & 31, MFC_GET_CMD);
    oldtmask = spu_readch(MFC_RdTagMask);
    spu_writech(MFC_WrTagMask, tagmask);
    spu_writech(MFC_WrTagUpdate, MFC_TAG_UPDATE_ANY);
    spu_readch(MFC_RdTagStat);

    buf[offset] = 0;
    MFC_DMA(buf, ea64, size, tagid & 31, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, tagmask);
    spu_writech(MFC_WrTagUpdate, MFC_TAG_UPDATE_ANY);
    spu_readch(MFC_RdTagStat);
    spu_writech(MFC_WrTagMask, oldtmask);
    mfc_tag_release(tagid);
}

static inline void _spin_lock(eaddr_t ea)
{
    DECL_MUTEX_VARS();
    s32 status;

    /* _spin_lock
     *    Internal routine to acquire spinlock.
     */
/* non trace - no hit/miss indicator */
#define SET_HIT
    RAW_SPINLOCK(==, 1);
}

#ifdef LIBSYNC_TRACE
static inline s32 _spin_lock_trace(eaddr_t ea)
{
    DECL_MUTEX_VARS();
    s32 status = 0;
/* trace - need hit/miss indicator */
#undef SET_HIT
#define SET_HIT	if (status == 0) hit = buf[offset]
    s32 hit;

    hit = 0;
    /* _spin_lock_trace for trace
     *    Internal routine to acquire spinlock.
     */
    RAW_SPINLOCK(==, 1);

    return hit;

#undef SET_HIT
#define SET_HIT
}

#endif /* LIBSYNC_TRACE */


#endif /* SPU_MUTEX_H */
