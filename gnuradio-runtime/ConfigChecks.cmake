# Copyright 2010-2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
INCLUDE(GrMiscUtils)
INCLUDE(CheckCXXSourceCompiles)

IF(MSVC) #add this directory for our provided headers
LIST(APPEND CMAKE_REQUIRED_INCLUDES ${PROJECT_SOURCE_DIR}/msvc)
ENDIF(MSVC)

GR_CHECK_HDR_N_DEF(sys/types.h HAVE_SYS_TYPES_H)
GR_CHECK_HDR_N_DEF(sys/mman.h HAVE_SYS_MMAN_H)
GR_CHECK_HDR_N_DEF(sys/ipc.h HAVE_SYS_IPC_H)
GR_CHECK_HDR_N_DEF(sys/shm.h HAVE_SYS_SHM_H)
GR_CHECK_HDR_N_DEF(signal.h HAVE_SIGNAL_H)


########################################################################
CHECK_CXX_SOURCE_COMPILES("
    #include <signal.h>
    int main(){sigaction(0, 0, 0); return 0;}
    " HAVE_SIGACTION
)
GR_ADD_COND_DEF(HAVE_SIGACTION)

########################################################################
CHECK_CXX_SOURCE_COMPILES("
    #include <unistd.h>
    int main(){sysconf(0); return 0;}
    " HAVE_SYSCONF
)
GR_ADD_COND_DEF(HAVE_SYSCONF)

CHECK_CXX_SOURCE_COMPILES("
    #include <unistd.h>
    int main(){getpagesize(); return 0;}
    " HAVE_GETPAGESIZE
)
GR_ADD_COND_DEF(HAVE_GETPAGESIZE)

########################################################################
SET(CMAKE_REQUIRED_LIBRARIES -lpthread)
CHECK_CXX_SOURCE_COMPILES("
    #include <signal.h>
    int main(){pthread_sigmask(0, 0, 0); return 0;}
    " HAVE_PTHREAD_SIGMASK
)
GR_ADD_COND_DEF(HAVE_PTHREAD_SIGMASK)
SET(CMAKE_REQUIRED_LIBRARIES)

########################################################################
CHECK_CXX_SOURCE_COMPILES("
    #include <windows.h>
    int main(){
        HANDLE handle;
        int size;
        LPCTSTR lpName;
        handle = CreateFileMapping(
                         INVALID_HANDLE_VALUE,    // use paging file
                         NULL,                    // default security
                         PAGE_READWRITE,          // read/write access
                         0,                       // max. object size
                         size,                    // buffer size
                         lpName);                 // name of mapping object
        return 0;
    } " HAVE_CREATEFILEMAPPING
)
GR_ADD_COND_DEF(HAVE_CREATEFILEMAPPING)

########################################################################
SET(CMAKE_REQUIRED_LIBRARIES -lrt)
CHECK_CXX_SOURCE_COMPILES("
    #include <sys/types.h>
    #include <sys/mman.h>
    int main(){shm_open(0, 0, 0); return 0;}
    " HAVE_SHM_OPEN
)
GR_ADD_COND_DEF(HAVE_SHM_OPEN)
SET(CMAKE_REQUIRED_LIBRARIES)

########################################################################
CHECK_CXX_SOURCE_COMPILES("
    #define _GNU_SOURCE
    #include <math.h>
    int main(){double x, sin, cos; sincos(x, &sin, &cos); return 0;}
    " HAVE_SINCOS
)
GR_ADD_COND_DEF(HAVE_SINCOS)

CHECK_CXX_SOURCE_COMPILES("
    #define _GNU_SOURCE
    #include <math.h>
    int main(){float x, sin, cos; sincosf(x, &sin, &cos); return 0;}
    " HAVE_SINCOSF
)
GR_ADD_COND_DEF(HAVE_SINCOSF)

CHECK_CXX_SOURCE_COMPILES("
    #include <math.h>
    int main(){sinf(0); return 0;}
    " HAVE_SINF
)
GR_ADD_COND_DEF(HAVE_SINF)

CHECK_CXX_SOURCE_COMPILES("
    #include <math.h>
    int main(){cosf(0); return 0;}
    " HAVE_COSF
)
GR_ADD_COND_DEF(HAVE_COSF)

########################################################################
CHECK_CXX_SOURCE_COMPILES("
    #include <sys/mman.h>
    int main(){mmap(0, 0, 0, 0, 0, 0); return 0;}
    " HAVE_MMAP
)
GR_ADD_COND_DEF(HAVE_MMAP)
