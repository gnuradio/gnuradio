# Copyright 2010-2011 Free Software Foundation, Inc.
# 
# This file is part of GNU Radio
# 
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
# 
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
INCLUDE(GrMiscUtils)
INCLUDE(CheckCXXSourceCompiles)

GR_CHECK_HDR_N_DEF(signal.h HAVE_SIGNAL_H)
GR_CHECK_HDR_N_DEF(sched.h HAVE_SCHED_H)

SET(CMAKE_REQUIRED_LIBRARIES -lpthread)
CHECK_CXX_SOURCE_COMPILES("
    #include <signal.h>
    int main(){pthread_sigmask(0, 0, 0); return 0;}
    " HAVE_PTHREAD_SIGMASK
)
GR_ADD_COND_DEF(HAVE_PTHREAD_SIGMASK)

SET(CMAKE_REQUIRED_LIBRARIES -lpthread)
CHECK_CXX_SOURCE_COMPILES("
    #include <pthread.h>
    int main(){
        pthread_t pthread;
        pthread_setschedparam(pthread,  0, 0);
        return 0;
    } " HAVE_PTHREAD_SETSCHEDPARAM
)
GR_ADD_COND_DEF(HAVE_PTHREAD_SETSCHEDPARAM)

CHECK_CXX_SOURCE_COMPILES("
    #include <sched.h>
    int main(){
        pid_t pid;
        sched_setscheduler(pid, 0, 0);
        return 0;
    } " HAVE_SCHED_SETSCHEDULER
)
GR_ADD_COND_DEF(HAVE_SCHED_SETSCHEDULER)

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
GR_INCLUDE_SUBDIRECTORY(msg)
GR_INCLUDE_SUBDIRECTORY(pmt)

########################################################################
# Setup the include and linker paths
########################################################################
INCLUDE_DIRECTORIES(${GRUEL_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND gruel_sources
    realtime.cc
    sys_pri.cc
    thread_body_wrapper.cc
    thread_group.cc
)

ADD_LIBRARY(gruel SHARED ${gruel_sources})
TARGET_LINK_LIBRARIES(gruel ${Boost_LIBRARIES})
SET_TARGET_PROPERTIES(gruel PROPERTIES DEFINE_SYMBOL "gruel_EXPORTS")
SET_TARGET_PROPERTIES(gruel PROPERTIES SOVERSION ${LIBVER})

INSTALL(TARGETS gruel
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "gruel_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "gruel_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "gruel_runtime" # .dll file
)

########################################################################
# Setup tests
# Set the test environment so the build libs will be found under MSVC.
########################################################################
IF(ENABLE_TESTING)

INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIRS})
LINK_DIRECTORIES(${CPPUNIT_LIBRARY_DIRS})

INCLUDE(GrTest)
SET(GR_TEST_TARGET_DEPS gruel)
LIST(APPEND test_gruel_sources test_gruel.cc)
ADD_EXECUTABLE(test_gruel ${test_gruel_sources})
TARGET_LINK_LIBRARIES(test_gruel gruel ${CPPUNIT_LIBRARIES})
GR_ADD_TEST(gruel-test test_gruel)

ENDIF(ENABLE_TESTING)
