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
#ifndef _SPU_ATOMIC_DEC_IF_POSITIVE_H_
#define _SPU_ATOMIC_DEC_IF_POSITIVE_H_

#include <sync_utils.h>
#include <atomic.h>

/**
 * atomic_dec_if_positive - atomically decrement if counter is positive.
 * @v: handle to effective address of counter.
 * 
 * Atomically decrement a counter if its value is positive.
 * The only restriction is that @v must be word aligned.
 *
*
 * Returns the old value of the counter, minus 1.
 */
static __inline int _atomic_dec_if_positive(atomic_ea_t v)
{
    DECL_ATOMIC_VARS();
    s32 status;

    ea64.ull = ALIGN128_EA(v);
    offset = OFFSET128_EA_U32(v);
    do {
	MFC_DMA(buf, ea64, size, tagid, MFC_GETLLAR_CMD);
	spu_readch(MFC_RdAtomicStat);

	ret_val = buf[offset] - 1;
	if (likely(ret_val >= 0)) {
	    buf[offset] = ret_val;
	    MFC_DMA(buf, ea64, size, tagid, MFC_PUTLLC_CMD);
	    status = spu_readch(MFC_RdAtomicStat);
	} else {
	    MFC_DMA(buf, ea64, size, tagid, MFC_PUTLLC_CMD);
	    spu_readch(MFC_RdAtomicStat);
	    status = 0;
	    break;
	}
    } while (status != 0);

    return ret_val;
}



#endif /* _SPU_ATOMIC_DEC_IF_POSITIVE_H_ */
