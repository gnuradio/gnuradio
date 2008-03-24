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
#ifndef _PPU_COND_WAIT_H_
#define _PPU_COND_WAIT_H_

#include <ppu_intrinsics.h>
#include "sync_utils.h"
#include "mutex_lock.h"
#include "mutex_unlock.h"
#include "cond.h"

static __inline void _cond_wait (cond_ea_t cond, mutex_ea_t  mutex)
{
  int delta, cur_delta;
  unsigned int val, cond_val, signaled_cnt;
  void *p;


  SYNC_ULL_TO_PTR(cond, p);

  /* Atomically signal we have entered the condition wait by incrementing
   * the waiting count.
   */
  do {
    val = __lwarx(p);
    val = (val & ~0xFFFF) | ((val+1) & 0xFFFF);
  } while (__stwcx(p, val) == 0);


  /* Release the lock
   */
  _mutex_unlock (mutex);

  /* Determine the signal count needed for this 
   * participant to be signaled.
   */

  signaled_cnt = val >> 16;
  delta = (int)(val & 0xFFFF) - (int)signaled_cnt;
  if (delta < 0) delta = -delta;

  /* Wait until the signaled count reaches the count
   * previously determined.
   */
  do {
    cond_val = __lwarx(p);

    cur_delta = (int)(cond_val >> 16) - signaled_cnt;
    if (cur_delta < 0) cur_delta = -cur_delta;
   
  } while (cur_delta < delta);

  /* Relock the mutex 
   */
  _mutex_lock (mutex);
}

#endif /* _PPU_COND_WAIT_H_ */
