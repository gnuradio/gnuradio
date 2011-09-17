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
INCLUDE(${CMAKE_CURRENT_SOURCE_DIR}/ConfigChecks.cmake)

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

LIST(APPEND gnuradio_core_sources bug_work_around_6.cc)
LIST(APPEND test_gnuradio_core_sources bug_work_around_6.cc)

########################################################################
# Setup the include and linker paths
########################################################################
INCLUDE_DIRECTORIES(${GNURADIO_CORE_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

ADD_DEFINITIONS(${GSL_DEFINITIONS})
INCLUDE_DIRECTORIES(${GSL_INCLUDE_DIRS})
LINK_DIRECTORIES(${GSL_LIBRARY_DIRS})

INCLUDE_DIRECTORIES(${FFTW3F_INCLUDE_DIRS})
LINK_DIRECTORIES(${FFTW3F_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
LIST(APPEND gnuradio_core_libs
    gruel
    ${Boost_LIBRARIES}
    ${GSL_LIBRARIES}
    ${FFTW3F_LIBRARIES}
)

ADD_LIBRARY(gnuradio-core SHARED ${gnuradio_core_sources})
TARGET_LINK_LIBRARIES(gnuradio-core ${gnuradio_core_libs})
SET_TARGET_PROPERTIES(gnuradio-core PROPERTIES DEFINE_SYMBOL "gnuradio_core_EXPORTS")
SET_TARGET_PROPERTIES(gnuradio-core PROPERTIES SOVERSION ${LIBVER})
#avoid fftw and gsl link in dependent libraries:
SET_TARGET_PROPERTIES(gnuradio-core PROPERTIES LINK_INTERFACE_LIBRARIES "gruel")

INSTALL(TARGETS gnuradio-core
    LIBRARY DESTINATION ${GR_LIBRARY_DIR} COMPONENT "core_runtime" # .so/.dylib file
    ARCHIVE DESTINATION ${GR_LIBRARY_DIR} COMPONENT "core_devel" # .lib file
    RUNTIME DESTINATION ${GR_RUNTIME_DIR} COMPONENT "core_runtime" # .dll file
)

########################################################################
# Setup executables
########################################################################
ADD_EXECUTABLE(gnuradio-config-info gnuradio-config-info.cc)
TARGET_LINK_LIBRARIES(gnuradio-config-info gnuradio-core ${Boost_LIBRARIES})
INSTALL(
    TARGETS gnuradio-config-info
    DESTINATION ${GR_RUNTIME_DIR}
    COMPONENT "core_runtime"
)

########################################################################
# Setup tests
########################################################################
IF(ENABLE_TESTING)

INCLUDE_DIRECTORIES(${CPPUNIT_INCLUDE_DIRS})
LINK_DIRECTORIES(${CPPUNIT_LIBRARY_DIRS})

ADD_LIBRARY(test-gnuradio-core SHARED ${test_gnuradio_core_sources})
TARGET_LINK_LIBRARIES(test-gnuradio-core gnuradio-core ${CPPUNIT_LIBRARIES} ${Boost_LIBRARIES})

ENDIF(ENABLE_TESTING)
