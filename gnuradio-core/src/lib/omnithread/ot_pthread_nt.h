/*				Package : omnithread
   omnithread/pthread_nt.h	Created : Steven Brenneis <brennes1@rjrt.com>
  
      Copyright (C) 1998 Steven Brennes
  
      This file is part of the omnithread library
  
      The omnithread library is free software; you can redistribute it and/or
      modify it under the terms of the GNU Library General Public
      License as published by the Free Software Foundation; either
      version 2 of the License, or (at your option) any later version.
  
      This library is distributed in the hope that it will be useful,
      but WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
      Library General Public License for more details.
  
      You should have received a copy of the GNU Library General Public
      License along with this library; if not, write to the Free
      Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
      02111-1307, USA
  
      Posix Threads implementation for Windows NT, version 4.0
*/

#ifndef PTHREAD_NT_H_INCLUDED
#define PTHREAD_NT_H_INCLUDED

#include <errno.h>

#ifndef ETIMEDOUT
//	May have to be changed if NT starts supporting more errno values
#define ETIMEDOUT 60
#endif

#undef PthreadDraftVersion
#define PthreadDraftVersion 10

#define NoNanoSleep

#define PthreadSupportThreadPriority

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _TIMERS_T_
#define _TIMERS_T_
	typedef struct timespec {
		unsigned long tv_sec;
		long tv_nsec;
	} timespec_t;
#endif

typedef char* __pthreadLongString_t;
typedef void* __pthreadLongAddr_t;
typedef __pthreadLongAddr_t* __pthreadLongAddr_p;
typedef long __pthreadLongInt_t;
typedef unsigned long __pthreadLongUint_t;
typedef __pthreadLongAddr_p __pthreadTsd_t;

typedef struct __pthread_mutex_t {
	unsigned int lock;           /* LOCK, SLOW, TYPE, RECURSIVE  */
	unsigned int valid;          /* Validation info */
	__pthreadLongString_t name;   /* Name of mutex */
	unsigned int arg;            /* printf argument for  name */
	unsigned int depth;          /* Recursive lock depth */
	unsigned long sequence;       /* Mutex sequence number */
	unsigned long owner;          /* Current owner (if known */
	__pthreadLongAddr_t block;          /* Pointer to blocking struct */
} pthread_mutex_t;

typedef struct __pthread_mutexattr_t {
	long valid;
	__pthreadLongUint_t reserved[15];
} pthread_mutexattr_t;

typedef struct __pthread_cond_t {
	unsigned int state;          /* EVENT, SLOW, REFCNT */
	unsigned int valid;          /* Validation info */
	__pthreadLongString_t name;   /* Name of condition variable */
	unsigned int arg;            /* printf argument for name */
	unsigned long sequence;       /* Condition variable seq # */
	__pthreadLongAddr_t block;          /* Pointer to blocking struct */
} pthread_cond_t ;

typedef struct __pthread_condattr_t {
	long valid;
	__pthreadLongUint_t reserved[13];
} pthread_condattr_t ;

typedef struct __pthread_transp_t {
	__pthreadLongAddr_t reserved1;      /* Reserved to posix_nt */
	__pthreadLongAddr_t reserved2;      /* Reserved to posix_nt */
	unsigned short size;           /* Size of data structure */
	unsigned char reserved3[2];   /* Reserved to posix_nt */
	__pthreadLongAddr_t reserved4;   /* Reserved to posix_nt */
	__pthreadLongUint_t sequence;       /* Thread sequence number */
	__pthreadLongUint_t reserved5[2];   /* Reserved to posix_nt */
	__pthreadLongAddr_t per_kt_area;    /* Pointer to kernel context */
	__pthreadLongAddr_t stack_base;     /* Current stack base */
	__pthreadLongAddr_t stack_reserve; /* Current stack reserve zone */
	__pthreadLongAddr_t stack_yellow;   /* Current stack yellow zone */
	__pthreadLongAddr_t stack_guard;    /* Current stack guard zone */
	__pthreadLongUint_t stack_size;     /* Size of stack */
	__pthreadTsd_t tsd_values;     /* TSD array (indexed by key) */
	unsigned long tsd_count;      /* Number of TSD cells */
	__pthreadLongAddr_t reserved6;      /* Reserved to posix_nt */
	__pthreadLongAddr_t reserved7;      /* Reserved to posix_nt */
	unsigned int thread_flags;   /* Dynamic external state */
} pthread_transp_t, *pthread_transp_p;

typedef pthread_transp_p pthread_t;

typedef struct __pthread_attr_t {
	long valid;
	__pthreadLongString_t name;
	__pthreadLongUint_t arg;
	__pthreadLongUint_t reserved[19];
} pthread_attr_t ;

typedef unsigned int pthread_key_t;

typedef struct sched_param {
	int sched_priority;
} sched_param_t;
	 
/*	Function Prototypes */

int pthread_create(pthread_t *thread, const pthread_attr_t *attr,
								void *(*start_routine)(void*), void *arg);
int pthread_detach(pthread_t thread);
int pthread_join(pthread_t thread, void **value_ptr);
void pthread_exit(void *value_ptr);
int pthread_attr_init(pthread_attr_t *attr);
int pthread_attr_destroy(pthread_attr_t *attr);
int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
int pthread_attr_getstacksize(const pthread_attr_t *attr, 
											size_t *stacksize);
int pthread_cond_init(pthread_cond_t *cond,
								const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_timedwait(pthread_cond_t *cond, 
											pthread_mutex_t *mutex,
											const struct timespec *abstime);
int pthread_cond_signal(pthread_cond_t *cond);
int pthread_cond_broadcast(pthread_cond_t *cond);
int pthread_key_create(pthread_key_t *key, void (*destructor)(void*));
int pthread_key_delete(pthread_key_t key);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_init(pthread_mutex_t *mutex, 
									const pthread_mutexattr_t *attr);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_trylock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);
pthread_t pthread_self();
int pthread_setspecific(pthread_key_t key, const void *value);
void *pthread_getspecific(pthread_key_t key);
int pthread_getschedparam(pthread_t thread, int *policy,
											struct sched_param *param);
int pthread_setschedparam(pthread_t thread, int policy,
										const struct sched_param *param);
int pthread_attr_setschedparam(pthread_attr_t *attr, 
												const struct sched_param *param);
int pthread_attr_getschedparam(const pthread_attr_t *attr, 
												struct sched_param *param);

int pthread_delay_np(const struct timespec *interval);
int pthread_get_expiration_np(const struct timespec *delta,
												struct timespec *abstime);

# define SCHED_FIFO 1
# define SCHED_RR 2
# define SCHED_OTHER 3

int sched_yield();
int sched_get_priority_max(int policy);
int sched_get_priority_min(int policy);


#ifdef __cplusplus
}
#endif

#endif	//	PTHREAD_NT_H_INCLUDED
