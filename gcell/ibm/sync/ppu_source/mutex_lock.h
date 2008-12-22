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
#ifndef _PPU_MUTEX_LOCK_H_
#define _PPU_MUTEX_LOCK_H_

#include <ppu_intrinsics.h>
#include "sync_utils.h"
#include "mutex.h"
#include "trace_libsync.h"

/* function:	_mutex_lock
 *
 * Aquire a lock at a location in system memory by waiting for the
 * value to become zero, then atomically storing 1. 
 */
static __inline void _mutex_lock (mutex_ea_t lock)
{
  unsigned int done = 0;
  void *p;
#ifdef LIBSYNC_TRACE
  unsigned int miss = 0;
#endif /* LIBSYNC_TRACE */

  TRACE_MUTEX_LOCK_ENTRY(interval);

  SYNC_ULL_TO_PTR(lock, p);

  do {
    if (__lwarx(p) == 0) done = __stwcx(p, (unsigned int) 1);
#ifdef LIBSYNC_TRACE
    /* if we missed the lock, note it.. */
    if (done == 0) miss = 1;
#endif
  } while (done == 0);
  __isync();

  TRACE_MUTEX_LOCK_EXIT(interval, lock, miss);
}

#endif /* _PPU_MUTEX_LOCK_H_ */
