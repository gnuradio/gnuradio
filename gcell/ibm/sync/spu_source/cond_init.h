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
#ifndef _SPU_COND_INIT_H_
#define _SPU_COND_INIT_H_

#include "sync_utils.h"
#include "cond.h"
#include <spu_mfcio.h>


/**
 * cond_init - initialize condition variable.
 * @cond: handle to effective address of condition variable.
 *
 *
 * Conditional Variable - is a synchronization device that allows 
 * SPE and PPE threads to suspend execution and relinquish the 
 * processors until some predicate on shared data is satisfied. 
 * The basic operations on conditions are: signal the condition
 * (when the predicate becomes true), and wait for the condition,
 * suspending the thread execution until anoter thread signals the
 * condition
 *
 * A condition variable must always be associated with a mutex, to
 * avoid the race condition where a thread prepares to wait on a 
 * condition variable and another thread signals the condition just
 * before the first thread actually waits on it.
 *
 * cond_init initializes the condition variable cond. 
 *
 * cond_signal restarts one of the threads that are waiting on the
 * condition variable cond. If no threads are waiting on cond, nothing
 * happens. If several threads are waiting on cond, exactly one
 * is restarted, but it is not specified which
 *
 * cond_broadcast restarts all the threads that are waiting on the 
 * condition variable cond. Nothing happens if no threads are waiting
 * on cond
 *
 * cond_wait atomically unlocks the mutex and waits for the condition
 * variable cond to be signaled. The mutex must be lock locked by 
 * the calling thread on the entrance to cond_wait. Before returning
 * to the calling thread, cond_wait re-acquires mutex. 
 *
 * Only one thread initializes a condition variable. Usually, the 
 * PPE thread initializes a condidtion variable, however, a cond_init
 * function is provided here for completeness
 *
 * Description: Initialize a cond variable to false.
 */
static __inline void _cond_init(cond_ea_t cond )
{
    char _tmp[256];                                     
    char *tmp = (char *) ALIGN(_tmp, 128);              
    volatile unsigned short *buf = (volatile unsigned short *) &tmp[0];       
    unsigned int size = 128, tagid;                          
    unsigned int offset;           
    addr64 ea64;
    unsigned int oldtmask;
    unsigned int tagmask;    

    tagid = mfc_tag_reserve(); 

    tagmask = 1 << (tagid & 31);

    ea64.ull = ALIGN128_EA(cond);
    offset = OFFSET128_EA_U16(cond);

    MFC_DMA(buf, ea64, size, tagid & 31, MFC_GET_CMD);
    oldtmask = spu_readch(MFC_RdTagMask);
    spu_writech(MFC_WrTagMask, tagmask);
    spu_writech(MFC_WrTagUpdate, MFC_TAG_UPDATE_ANY);
    spu_readch(MFC_RdTagStat);

    /* this is still just one word. since buf is of type
     * short, we fit both counts into one word. */
    buf[offset] = 0;
    buf[offset+1] = 0;
    MFC_DMA(buf, ea64, size, (tagid & 31), MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, tagmask);
    spu_writech(MFC_WrTagUpdate, MFC_TAG_UPDATE_ANY);
    spu_readch(MFC_RdTagStat);
    spu_writech(MFC_WrTagMask, oldtmask);
    mfc_tag_release (tagid);
}



#endif /* _SPU_COND_INIT_H_ */
