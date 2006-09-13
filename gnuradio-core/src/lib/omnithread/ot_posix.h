//				Package : omnithread
// omnithread/posix.h		Created : 7/94 tjr
//
//    Copyright (C) 2006 Free Software Foundation, Inc.
//    Copyright (C) 1994,1995,1996, 1997 Olivetti & Oracle Research Laboratory
//
//    This file is part of the omnithread library
//
//    The omnithread library is free software; you can redistribute it and/or
//    modify it under the terms of the GNU Library General Public
//    License as published by the Free Software Foundation; either
//    version 2 of the License, or (at your option) any later version.
//
//    This library is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//    Library General Public License for more details.
//
//    You should have received a copy of the GNU Library General Public
//    License along with this library; if not, write to the Free
//    Software Foundation, Inc., 51 Franklin Street, Boston, MA  
//    02110-1301, USA
//
//
// OMNI thread implementation classes for posix threads
//

#ifndef __omnithread_posix_h_
#define __omnithread_posix_h_

#if defined(__alpha__) && defined(__osf1__) || defined(__hpux__)
// stop unnecessary definitions of TRY, etc on OSF
#ifndef EXC_HANDLING
#define EXC_HANDLING
#endif
#endif

#ifndef __POSIX_NT__
#  include <pthread.h>
#else
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#    define OMNI_DEFINED_WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  include "pthread_nt.h"
#  ifdef OMNI_DEFINED_WIN32_LEAN_AND_MEAN
#    undef WIN32_LEAN_AND_MEAN
#    undef OMNI_DEFINED_WIN32_LEAN_AND_MEAN
#  endif
#endif

extern "C" void* omni_thread_wrapper(void* ptr);

#define OMNI_MUTEX_IMPLEMENTATION			\
    pthread_mutex_t posix_mutex;

#define OMNI_MUTEX_LOCK_IMPLEMENTATION                  \
    pthread_mutex_lock(&posix_mutex);

#define OMNI_MUTEX_TRYLOCK_IMPLEMENTATION               \
    (pthread_mutex_trylock(&posix_mutex)==0);

#define OMNI_MUTEX_UNLOCK_IMPLEMENTATION                \
    pthread_mutex_unlock(&posix_mutex);

#define OMNI_CONDITION_IMPLEMENTATION			\
    pthread_cond_t posix_cond;

#define OMNI_SEMAPHORE_IMPLEMENTATION			\
    omni_mutex m;					\
    omni_condition c;					\
    int value;						\
    int max_count;

#define OMNI_THREAD_IMPLEMENTATION			\
    pthread_t posix_thread;				\
    static int posix_priority(priority_t);		\
    friend void* omni_thread_wrapper(void* ptr);

#endif
