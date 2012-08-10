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

list(APPEND gnuradio_core_sources bug_work_around_6.cc)
list(APPEND test_gnuradio_core_sources bug_work_around_6.cc)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(${GNURADIO_CORE_INCLUDE_DIRS})
include_directories(${VOLK_INCLUDE_DIRS})

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

include_directories(${FFTW3F_INCLUDE_DIRS})
link_directories(${FFTW3F_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gnuradio_core_libs
    gruel
    ${Boost_LIBRARIES}
    ${FFTW3F_LIBRARIES}
)

if(FFTW3F_THREADS_LIBRARIES)
	list(APPEND gnuradio_core_libs ${FFTW3F_THREADS_LIBRARIES} )
	add_definitions("-DFFTW3F_THREADS")
endif()

#need to link with librt on ubuntu 11.10 for shm_*
if(LINUX)
    list(APPEND gnuradio_core_libs rt)
endif()

# Link against libvolk
list(APPEND gnuradio_core_libs volk)

add_library(gnuradio-core SHARED ${gnuradio_core_sources})
target_link_libraries(gnuradio-core ${gnuradio_core_libs})
GR_LIBRARY_FOO(gnuradio-core RUNTIME_COMPONENT "core_runtime" DEVEL_COMPONENT "core_devel")
set_target_properties(gnuradio-core PROPERTIES LINK_INTERFACE_LIBRARIES "gruel")

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
