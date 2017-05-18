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

########################################################################
# Setup CPack components
########################################################################
include(GrPackage)
CPACK_SET(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION "GNU Radio Runtime")

CPACK_COMPONENT("runtime_runtime"
    GROUP        "Runtime"
    DISPLAY_NAME "Runtime"
    DESCRIPTION  "Dynamic link libraries"
    DEPENDS      "runtime_runtime"
)

CPACK_COMPONENT("runtime_devel"
    GROUP        "Runtime"
    DISPLAY_NAME "Development"
    DESCRIPTION  "C++ headers, package config, import libraries"
    DEPENDS      "runtime_devel"
)

CPACK_COMPONENT("runtime_python"
    GROUP        "Runtime"
    DISPLAY_NAME "Python"
    DESCRIPTION  "Python modules for runtime"
    DEPENDS      "runtime_python;runtime_runtime"
)

CPACK_COMPONENT("runtime_swig"
    GROUP        "Runtime"
    DISPLAY_NAME "SWIG"
    DESCRIPTION  "SWIG development .i files"
    DEPENDS      "runtime_swig;runtime_python;runtime_devel"
)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-runtime.conf.in
    ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.conf
@ONLY)

install(
    FILES ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-runtime.conf
    DESTINATION ${GR_PREFSDIR}
    COMPONENT "runtime_runtime"
)

if(ENABLE_GR_LOG AND HAVE_LOG4CPP)
install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gr_log_default.conf
    DESTINATION ${GR_PREFSDIR}
    COMPONENT "runtime_runtime"
)
set(PC_ADD_LIBS -llog4cpp)
endif(ENABLE_GR_LOG AND HAVE_LOG4CPP)

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
    COMPONENT "runtime_devel"
)

endif(ENABLE_GNURADIO_RUNTIME)
