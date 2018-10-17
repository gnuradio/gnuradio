# Copyright 2013 Free Software Foundation, Inc.
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
# Setup dependencies
########################################################################
include(GrBoost)
include(GrPython)

find_package(MPLIB REQUIRED)

########################################################################
# Setup compatibility checks and defines
########################################################################
include(${CMAKE_CURRENT_SOURCE_DIR}/ConfigChecks.cmake)
GR_CHECK_LINUX_SCHED_AVAIL()

########################################################################
# Register component
########################################################################
include(GrComponent)
GR_REGISTER_COMPONENT("gnuradio-runtime" ENABLE_GNURADIO_RUNTIME
    Boost_FOUND
    ENABLE_VOLK
    PYTHONINTERP_FOUND
    MPLIB_FOUND
    LOG4CPP_FOUND
)

GR_SET_GLOBAL(GNURADIO_RUNTIME_INCLUDE_DIRS
    ${CMAKE_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/gnuradio-runtime/include
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/include
    ${LOG4CPP_INCLUDE_DIRS}
)

GR_SET_GLOBAL(GNURADIO_RUNTIME_SWIG_INCLUDE_DIRS
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}/gnuradio-runtime/swig
    ${CMAKE_BINARY_DIR}/gnuradio-runtime/swig
)

get_filename_component(GNURADIO_RUNTIME_PYTHONPATH
    ${CMAKE_SOURCE_DIR}/python ABSOLUTE
)
GR_SET_GLOBAL(GNURADIO_RUNTIME_PYTHONPATH ${GNURADIO_RUNTIME_PYTHONPATH})

########################################################################
# Register controlport component
########################################################################

FIND_PACKAGE(SWIG)

GR_REGISTER_COMPONENT("gr-ctrlport" ENABLE_GR_CTRLPORT
  Boost_FOUND
  SWIG_FOUND
  SWIG_VERSION_CHECK
  ENABLE_GNURADIO_RUNTIME
)

########################################################################
# Begin conditional configuration
########################################################################
if(ENABLE_GNURADIO_RUNTIME)

get_filename_component(GR_RUNTIME_PYTHONPATH
    ${CMAKE_CURRENT_SOURCE_DIR}/python ABSOLUTE
)
GR_SET_GLOBAL(GR_RUNTIME_PYTHONPATH ${GR_RUNTIME_PYTHONPATH})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.conf
    DESTINATION ${GR_PREFSDIR}
)

install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_log_default.conf
    DESTINATION ${GR_PREFSDIR}
)

set(PC_ADD_LIBS "${MPLIB_PC_ADD_LIBS} ${PC_ADD_LIBS}")
set(PC_ADD_CFLAGS ${MPLIB_PC_ADD_CFLAGS})

########################################################################
# Add subdirectories
########################################################################
add_subdirectory(include)
add_subdirectory(lib)
add_subdirectory(apps)
if(ENABLE_PYTHON)
     add_subdirectory(swig)
     add_subdirectory(python)
    add_subdirectory(examples)
endif(ENABLE_PYTHON)

########################################################################
# Create Pkg Config File
########################################################################
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.pc
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.pc
    DESTINATION ${GR_LIBRARY_DIR}/pkgconfig
)

endif(ENABLE_GNURADIO_RUNTIME)
