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
#ifndef _SPU_WAIT_FOR_COMPLETION_H_
#define _SPU_WAIT_FOR_COMPLETION_H_

#include "sync_utils.h"
#include <spu_mfcio.h>
#include "sync_irq.h"
#include "completion.h"

/* _wait_for_completion 
 *    Internal routine to wait for completion to
 *    become true (!0).  When completion is false,
 *    uses lock line reservation lost event to 
 *    sleep until the variable has been changed.
 */
static __inline void _wait_for_completion(completion_ea_t completion)
{
    DECL_COMPLETION_VARS();
    s32 status;

    status = 1;

    ea64.ull = ALIGN128_EA(completion);
    offset = OFFSET128_EA_U32(completion);
    do {
        MFC_DMA(buf, ea64, size, tagid, MFC_GETLLAR_CMD);
        spu_readch(MFC_RdAtomicStat);
        /* if the completion variable has been set elsewhere
         * (a signal or broadcast function has been called 
         * then we get out of the loop, and reset the variable */
        if (likely(buf[offset] != 0)) {
            /* decrement the variable */
            buf[offset]--;
            MFC_DMA(buf, ea64, size, tagid, MFC_PUTLLC_CMD);
            spu_readch(MFC_RdAtomicStat);
            status = 0;
        }
    } while (status != 0);
}



#endif /* _SPU_WAIT_FOR_COMPLETION_H_ */
