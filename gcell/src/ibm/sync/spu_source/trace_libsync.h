/* --------------------------------------------------------------  */
/* (C)Copyright 2007                                               */
/* International Business Machines Corporation                     */
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
#ifndef __LIBSYNC_SPU_TRACEHOOKS_H__
#define __LIBSYNC_SPU_TRACEHOOKS_H__

#ifdef LIBSYNC_TRACE

#include <trace_events.h>

/*
 *  last parameter in the trace_even and trace_interval_entry call is
 *   the stack level to report the PC in the trace record. so 0 means
 *   the PC of the current frame, 1 means the PC of the caller, etc.
 *   there's a dilemma here - these macros are included in the inline
 *   _mutex calls which get called from the (created during build)
 *   mutex functions in the mutex .c files. in that case, the level
 *   should be 1 - we don't want the PC of where in the mutex function
 *   the trace call is happening, we want the PC of whomever is calling
 *   the mutex function.
 *
 *   but.. an app can call the inline functions - it can #include the
 *   appropriate libsync mutex .h files, and call the inlined _mutex
 *   function in their code. in that case, the appropriate level for the
 *   trace calls would be 0 - the user would want to know where in their
 *   code the call to the _mutex function is.
 *
 *   so, we'll assume _LEVEL of 0 which is what the inline funtions need.
 *   when we build the files for libsync, we'll do a -D_LEVEL=1 
 */
#ifndef _LEVEL
#define _LEVEL 0
#endif


#define TRACE_EVENT_MUTEX_INIT 0x0403

#define TRACE_MUTEX_INIT(lock) { \
  trace_payload_t payload; \
  payload.dword[0]=(unsigned long)lock; \
  trace_event(TRACE_EVENT_MUTEX_INIT, 1, &payload, "Event=%d, lock=0x%x", _LEVEL); \
}

#define TRACE_EVENT_MUTEX_LOCK 0x0503

#define TRACE_MUTEX_LOCK_ENTRY(_INTERVAL) \
trace_interval_p _INTERVAL = trace_interval_entry(TRACE_EVENT_MUTEX_LOCK, _LEVEL)

#define TRACE_MUTEX_LOCK_EXIT(_INTERVAL,lock,miss) { \
  trace_payload_t payload; \
  payload.dword[0]=(unsigned long)lock; \
  payload.word[2]=(unsigned int)miss; \
  trace_interval_exit(_INTERVAL, 2,  &payload, "Event=%d, lock=0x%x, miss=0x%x"); \
}

#define TRACE_EVENT_MUTEX_TRYLOCK 0x0603

#define TRACE_MUTEX_TRYLOCK(lock,ret_val) { \
  trace_payload_t payload; \
  payload.dword[0]=(unsigned long)lock; \
  payload.word[2]=(unsigned int)ret_val; \
  trace_event(TRACE_EVENT_MUTEX_TRYLOCK, 2, &payload, "Event=%d, lock=0x%x, ret_val=0x%x", _LEVEL); \
}

#define TRACE_EVENT_MUTEX_UNLOCK 0x0703

#define TRACE_MUTEX_UNLOCK(lock) { \
  trace_payload_t payload; \
  payload.dword[0]=(unsigned long)lock; \
  trace_event(TRACE_EVENT_MUTEX_UNLOCK, 1, &payload, "Event=%d, lock=0x%x", _LEVEL); \
}

#else /* LIBSYNC_TRACE */

#define TRACE_MUTEX_INIT(lock) 
#define TRACE_MUTEX_LOCK_ENTRY(_INTERVAL) 
#define TRACE_MUTEX_LOCK_EXIT(_INTERVAL,lock,miss)
#define TRACE_MUTEX_TRYLOCK(lock,ret_val)
#define TRACE_MUTEX_UNLOCK(lock) 

#endif /* LIBSYNC_TRACE */

#endif  /* __LIBSYNC_SPU_TRACEHOOKS_H__ */
