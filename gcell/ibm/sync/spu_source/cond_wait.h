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
#ifndef _SPU_COND_WAIT_H_
#define _SPU_COND_WAIT_H_
#include <stdio.h>
#include "sync_irq.h"
#include "sync_utils.h"
#include "mutex_lock.h"
#include "mutex_unlock.h"
#include "cond.h"
#include <spu_mfcio.h>

static __inline void _cond_wait (cond_ea_t cond, mutex_ea_t  mutex)
{
    char _tmp[256];                                     
    char *tmp = (char *) ALIGN(_tmp, 128);              
    volatile signed short *buf = (volatile signed short *) &tmp[0];       
    unsigned int size = 128, tagid = 0;                          
    int status;                                
    unsigned int offset;                                         
    addr64 ea64;  
    signed short delta, cur_delta, signaled_cnt;

    ea64.ull = ALIGN128_EA(cond);
    offset = OFFSET128_EA_U16(cond);

    /* increment the waiting halfword of the condition variable.
     */
    do {
      MFC_DMA(buf, ea64, size, tagid, MFC_GETLLAR_CMD);
      (void)spu_readch(MFC_RdAtomicStat);

      buf[offset+1]++;
      
      MFC_DMA(buf, ea64, size, tagid, MFC_PUTLLC_CMD);
      status = spu_readch(MFC_RdAtomicStat); 
    } while (status);

    _mutex_unlock(mutex);

    /* keep track of the change in count needed to be signaled. This 
     * is delta.
     */
    signaled_cnt = buf[offset];
    delta = buf[offset+1] - signaled_cnt;
    if (delta < 0) delta = -delta;

    while (1) {
      MFC_DMA(buf, ea64, size, tagid, MFC_GETLLAR_CMD);
      (void)spu_readch(MFC_RdAtomicStat);

      cur_delta = buf[offset] - signaled_cnt;
      if (cur_delta < 0) cur_delta = -cur_delta;


      if (cur_delta >= delta) {
	/* the counts indicate that this thread has been signaled.
	 */
	break;
      } 
    } 
    _mutex_lock (mutex);
}

#endif /* _SPU_COND_WAIT_H_ */
