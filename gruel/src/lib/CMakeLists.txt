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
include(GrMiscUtils)
include(CheckCXXSourceCompiles)

GR_CHECK_HDR_N_DEF(signal.h HAVE_SIGNAL_H)
GR_CHECK_HDR_N_DEF(sched.h HAVE_SCHED_H)

set(CMAKE_REQUIRED_LIBRARIES -lpthread)
CHECK_CXX_SOURCE_COMPILES("
    #include <signal.h>
    int main(){pthread_sigmask(0, 0, 0); return 0;}
    " HAVE_PTHREAD_SIGMASK
)
GR_ADD_COND_DEF(HAVE_PTHREAD_SIGMASK)

set(CMAKE_REQUIRED_LIBRARIES -lpthread)
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
include_directories(${GRUEL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gruel_sources
    realtime.cc
    sys_pri.cc
    thread_body_wrapper.cc
    thread_group.cc
)

list(APPEND gruel_libs ${Boost_LIBRARIES})

if(HAVE_PTHREAD_SETSCHEDPARAM)
    list(APPEND gruel_libs pthread)
endif()

add_library(gruel SHARED ${gruel_sources})
target_link_libraries(gruel ${gruel_libs})
GR_LIBRARY_FOO(gruel RUNTIME_COMPONENT "gruel_runtime" DEVEL_COMPONENT "gruel_devel")

########################################################################
# Setup tests
# Set the test environment so the build libs will be found under MSVC.
########################################################################
if(ENABLE_TESTING)

include_directories(${CPPUNIT_INCLUDE_DIRS})
link_directories(${CPPUNIT_LIBRARY_DIRS})

include(GrTest)
set(GR_TEST_TARGET_DEPS gruel)
list(APPEND test_gruel_sources test_gruel.cc)
add_executable(test_gruel ${test_gruel_sources})
target_link_libraries(test_gruel gruel ${CPPUNIT_LIBRARIES})
GR_ADD_TEST(gruel-test test_gruel)

endif(ENABLE_TESTING)
