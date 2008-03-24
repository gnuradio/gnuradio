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
#ifndef _PPU_LIBSYNC_H_
#define _PPU_LIBSYNC_H_

#include "sync_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long long atomic_ea_t;

extern void atomic_set(atomic_ea_t v, int val);
extern void atomic_add(int a, atomic_ea_t v);
extern void atomic_sub(int a, atomic_ea_t v);
extern void atomic_inc(atomic_ea_t v);
extern void atomic_dec(atomic_ea_t v);

extern int atomic_read(atomic_ea_t v);
extern int atomic_add_return(int a, atomic_ea_t v);
extern int atomic_sub_return(int a, atomic_ea_t v);
extern int atomic_inc_return(atomic_ea_t v);
extern int atomic_dec_return(atomic_ea_t v);
extern int atomic_sub_and_test(int a, atomic_ea_t v);
extern int atomic_dec_and_test(atomic_ea_t v);
extern int atomic_dec_if_positive(atomic_ea_t v);

typedef unsigned long long mutex_ea_t;
void mutex_init(mutex_ea_t lock);

void mutex_lock(mutex_ea_t lock);
int mutex_trylock(mutex_ea_t ea);
void mutex_unlock(mutex_ea_t lock);

typedef struct
{
  int num_threads_signal;       /* the number of threads that are going to be waken up.
                                 There are 3 values possible for this parameter, 0, 1, 
                                 or num_threads_waiting*/
  int num_threads_waiting;      /* the number of threads that are waiting to be awaken */ 
} condition_variable_t __attribute__ ((aligned (16)));  

typedef unsigned long long cond_ea_t;         /* a system memory 64 bit address that points to
                                 * a valid condition_variable_t */

typedef union {
    unsigned long long ull;  
    unsigned int ui[2];
} val64;


void cond_init (cond_ea_t  cond);
void cond_signal (cond_ea_t cond);
void cond_broadcast (cond_ea_t cond);
void cond_wait (cond_ea_t cond, mutex_ea_t  mutex);


#define MAX_THREADS_WAITING   32000 

typedef unsigned long long completion_ea_t;

extern void init_completion(completion_ea_t comp);
extern void wait_for_completion(completion_ea_t comp);
/*
extern void wait_for_completion_irq(completion_ea_t comp);
extern void wait_for_completion_irqsave(completion_ea_t comp);
*/
extern void complete_all(completion_ea_t comp);
extern void complete (completion_ea_t comp);

#ifdef __cplusplus
}
#endif

#endif /* _PPU_LIBSYNC_H_ */
