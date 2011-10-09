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
# Setup compatibility checks and defines
########################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/ConfigChecks.cmake)

########################################################################
# Include subdirs rather to populate to the sources lists.
########################################################################
GR_INCLUDE_SUBDIRECTORY(missing)
GR_INCLUDE_SUBDIRECTORY(runtime)
GR_INCLUDE_SUBDIRECTORY(filter)
GR_INCLUDE_SUBDIRECTORY(viterbi)
GR_INCLUDE_SUBDIRECTORY(general)
GR_INCLUDE_SUBDIRECTORY(gengen)
GR_INCLUDE_SUBDIRECTORY(reed-solomon)
GR_INCLUDE_SUBDIRECTORY(io)
GR_INCLUDE_SUBDIRECTORY(hier)

list(APPEND gnuradio_core_sources bug_work_around_6.cc)
list(APPEND test_gnuradio_core_sources bug_work_around_6.cc)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(${GNURADIO_CORE_INCLUDE_DIRS})

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

add_definitions(${GSL_DEFINITIONS})
include_directories(${GSL_INCLUDE_DIRS})
link_directories(${GSL_LIBRARY_DIRS})

include_directories(${FFTW3F_INCLUDE_DIRS})
link_directories(${FFTW3F_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gnuradio_core_libs
    gruel
    ${Boost_LIBRARIES}
    ${GSL_LIBRARIES}
    ${FFTW3F_LIBRARIES}
)

#need to link with librt on ubuntu 11.10 for shm_*
if(LINUX)
    list(APPEND gnuradio_core_libs rt)
endif()

add_library(gnuradio-core SHARED ${gnuradio_core_sources})
target_link_libraries(gnuradio-core ${gnuradio_core_libs})
set_target_properties(gnuradio-core PROPERTIES DEFINE_SYMBOL "gnuradio_core_EXPORTS")
set_target_properties(gnuradio-core PROPERTIES SOVERSION ${LIBVER})
#avoid fftw and gsl link in dependent libraries:
set_target_properties(gnuradio-core PROPERTIES LINK_INTERFACE_LIBRARIES "gruel")

install(TARGETS gnuradio-core
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "core_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "core_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "core_runtime" # .dll file
)

########################################################################
# Setup executables
########################################################################
add_executable(gnuradio-config-info gnuradio-config-info.cc)
target_link_libraries(gnuradio-config-info gnuradio-core ${Boost_LIBRARIES})
install(
    TARGETS gnuradio-config-info
    DESTINATION ${GR_RUNTIME_DIR}
    COMPONENT "core_runtime"
)

########################################################################
# Setup tests
########################################################################
if(ENABLE_TESTING)

include_directories(${CPPUNIT_INCLUDE_DIRS})
link_directories(${CPPUNIT_LIBRARY_DIRS})

add_library(test-gnuradio-core SHARED ${test_gnuradio_core_sources})
target_link_libraries(test-gnuradio-core gnuradio-core ${CPPUNIT_LIBRARIES} ${Boost_LIBRARIES})

endif(ENABLE_TESTING)
