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
 * atomic.h - PPE atomic SHM counter operations.
 *
 * Interfaces patterned after, and hopefully compatible
 * with PowerPC64-Linux atomic counter operations.  Uses
 * 32b values for various counters.
 */
#ifndef _PPU_ATOMIC_H_
#define _PPU_ATOMIC_H_

#include <ppu_intrinsics.h>
#include "sync_utils.h"

typedef unsigned int atomic_t __attribute__ ((aligned (128))); 


/* atomic_ea_t is a 64bit effective address  that points to 
 * an atomic_t variable 
 */
typedef unsigned long long atomic_ea_t;

/**
 * ASSUMPTIONS:
 * On the PPE, the size of a reservation granule is 128 bytes
 * (a cache-line), so when a programmer puts a reservation on an
 * address, that whole cacheline is reserved. Therefore both
 * the PPE and SPE can participate in an atomic operation as long as
 * lwarx and getllar operate on the same cacheline. 
 */ 


/*
 * atomically loads and replaces the value v with val. 
 * Returns the old value at v
 */ 
static __inline int _atomic_replace(atomic_ea_t v, int val)
{
  int old;
  void *p;

  SYNC_ULL_TO_PTR(v, p);

  do {
    old = (int)__lwarx(p);
  } while (__stwcx(p, (unsigned int)val) == 0);

  return old;
}


/*
 * atomically loads the value at v, adds val, replaces the
 * value at v with the sum. Returns the old value at v
 */ 
static __inline int _atomic_modify(atomic_ea_t v, int val)
{
  int oldval, newval;
  void *p;

  SYNC_ULL_TO_PTR(v, p);

  do {
    oldval = (int)__lwarx(p);
    newval = oldval + val;
  } while (__stwcx(p, (unsigned int)newval) == 0);

  return oldval;
}


#endif /* _PPU_ATOMIC_H_ */

