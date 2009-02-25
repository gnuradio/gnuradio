#ifndef __VXTHREAD_H__
#define __VXTHREAD_H__
/*
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Project:     omniORB
%% Filename:    $Filename$
%% Author:      Guillaume/Bill ARRECKX
%%              Copyright Wavetek Wandel & Goltermann, Plymouth.
%% Description: OMNI thread implementation classes for VxWorks threads
%% Notes:
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% $Log$
%% Revision 1.1  2004/04/10 18:00:52  eb
%% Initial revision
%%
%% Revision 1.1.1.1  2004/03/01 00:20:27  eb
%% initial checkin
%%
%% Revision 1.1  2003/05/25 05:29:04  eb
%% see ChangeLog
%%
%% Revision 1.1.2.1  2003/02/17 02:03:07  dgrisby
%% vxWorks port. (Thanks Michael Sturm / Acterna Eningen GmbH).
%%
%% Revision 1.1.1.1  2002/11/19 14:55:21  sokcevti
%% OmniOrb4.0.0 VxWorks port
%%
%% Revision 1.2  2002/06/14 12:45:50  engeln
%% unnecessary members in condition removed.
%% ---
%%
%% Revision 1.1.1.1  2002/04/02 10:08:49  sokcevti
%% omniORB4 initial realease
%%
%% Revision 1.1  2001/03/23 16:50:23  hartmut
%% Initial Version 2.8
%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
*/


///////////////////////////////////////////////////////////////////////////
// Includes
///////////////////////////////////////////////////////////////////////////
#include <vxWorks.h>
#include <semLib.h>
#include <taskLib.h>


///////////////////////////////////////////////////////////////////////////
// Externs prototypes
///////////////////////////////////////////////////////////////////////////
extern "C" void omni_thread_wrapper(void* ptr);


///////////////////////////////////////////////////////////////////////////
// Exported macros
// Note: These are added as private members in each class implementation.
///////////////////////////////////////////////////////////////////////////
#define OMNI_MUTEX_IMPLEMENTATION \
   SEM_ID mutexID;	\
   bool m_bConstructed;

#define OMNI_CONDITION_IMPLEMENTATION \
   long waiters_; \
   SEM_ID waiters_lock_; \
   SEM_ID sema_;

#define OMNI_SEMAPHORE_IMPLEMENTATION \
   SEM_ID semID;

#define OMNI_MUTEX_LOCK_IMPLEMENTATION                  \
	if(semTake(mutexID, WAIT_FOREVER) != OK)	\
	{	\
		throw omni_thread_fatal(errno);	\
	}

#define OMNI_MUTEX_UNLOCK_IMPLEMENTATION                \
	if(semGive(mutexID) != OK)	\
	{	\
		throw omni_thread_fatal(errno);	\
	}

#define OMNI_THREAD_IMPLEMENTATION \
   friend void omni_thread_wrapper(void* ptr); \
   static int vxworks_priority(priority_t); \
   omni_condition *running_cond; \
   void* return_val; \
   int tid; \
   public: \
   static void attach(void); \
   static void detach(void); \
   static void show(void);


///////////////////////////////////////////////////////////////////////////
// Porting macros
///////////////////////////////////////////////////////////////////////////
// This is a wrapper function for the 'main' function which does not exists
//  as such in VxWorks. The wrapper creates a launch function instead,
//  which spawns the application wrapped in a omni_thread.
// Argc will always be null.
///////////////////////////////////////////////////////////////////////////
#define main( discarded_argc, discarded_argv ) \
        omni_discard_retval() \
          { \
          throw; \
          } \
        int omni_main( int argc, char **argv ); \
        void launch( ) \
          { \
          omni_thread* th = new omni_thread( (void(*)(void*))omni_main );\
          th->start();\
          }\
        int omni_main( int argc, char **argv )


#endif // ndef __VXTHREAD_H__
