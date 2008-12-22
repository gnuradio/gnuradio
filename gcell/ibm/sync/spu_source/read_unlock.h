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
#ifndef _SPU_READ_UNLOCK_H_
#define _SPU_READ_UNLOCK_H_

#include "sync_utils.h"
#include "mutex.h"
#include <spu_mfcio.h>


/**
 * read_unlock - release reader lock.
 * @ea: handle to effective address of lock variable.
 * 
 * Description: Release a reader lock.  The only restriction 
 * here is that @ea must be word aligned.
 *
 * Context: This routine should be used when interrupts
 * do not need to be re-enabled --either because interrupts
 * are not being used, or because the application will take
 * steps to re-enable them later.
 */
static __inline void _read_unlock(eaddr_t ea)
{
    DECL_MUTEX_VARS();
    s32 status;

    /* _read_unlock
     *
     *    Reader locks must use PUTLLC when releasing, instead of
     *    the more traditional PUTLLUC because of the non-exclusive
     *    nature of the lock.  The reason for this is that other
     *    readers may have incremented the counter, and we don't
     *    want to corrupt it by blindly issuing PUTLLUC!!
     */
    ea64.ull = ALIGN128_EA(ea);
    offset = OFFSET128_EA_U32(ea);
    do {
        MFC_DMA(buf, ea64, size, tagid, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);

        buf[offset] -= 1;
        MFC_DMA(buf, ea64, size, tagid, MFC_PUTLLC_CMD);
        status = spu_readch(MFC_RdAtomicStat);
    } while (status != 0);

}


#endif /* _SPU_READ_UNLOCK_H_ */
