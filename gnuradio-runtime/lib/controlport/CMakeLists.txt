# Copyright 2013,2019 Free Software Foundation, Inc.
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

if(ENABLE_GR_CTRLPORT)

# Keep track of the number of backends ControlPort supports
SET(CTRLPORT_BACKENDS 0)

# Add definition so we can compile in ControlPort to the blocks.
add_definitions(-DGR_CTRLPORT)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

list(APPEND gnuradio_ctrlport_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/rpcmanager.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/rpcserver_aggregator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/rpcserver_booter_aggregator.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/rpcserver_selector.cc
)


OPTION(ENABLE_CTRLPORT_THRIFT "Enable ControlPort Thrift support" ON)

if(ENABLE_CTRLPORT_THRIFT)

# Look if Thrift is installed and use it as a ControlPort backend.
FIND_PACKAGE(Thrift)

if(THRIFT_FOUND)

# If Thrift was found, make sure the selected c++ standard is
# compatible: Thrift <= 0.10 requires using boost:shared_ptr, which
# will work with any c++ standard. Thrift >= 0.11.0 requires using
# std::shared_ptr, which requires c++11 or later.

if(${THRIFT_VERSION} VERSION_GREATER_EQUAL 0.11.0)
  if(CMAKE_CXX_STANDARD)
    if(${CMAKE_CXX_STANDARD} VERSION_LESS 11)
      message(WARNING "\nWarning: Thrift version is ${THRIFT_VERSION}, which requires C++11 for building. The current c++ standard to use is C++${CMAKE_CXX_STANDARD}, which will not work with this Thrift version. Trying to set CMake internally to use C++11 ...")
      set(CMAKE_CXX_STANDARD 11)
    endif()
  else()
    message(WARNING "\nWarning: Thrift version is ${THRIFT_VERSION}, which requires C++11 for building. The c++ standard to use is not currently set. Trying to set CMake internally to use C++11 ...")
    set(CMAKE_CXX_STANDARD 11)
  endif()
endif()

# Add a macro for whether to require using std::shared_ptr.
# depends on the version of Thrift.
if(${THRIFT_VERSION} VERSION_LESS_EQUAL 0.10.0)
  # old version that uses boost::shared_ptr
  add_definitions("-DCTRLPORT_USE_STD_SHARED_PTR=0")
else()
  # new version that uses c++11 std::shared_ptr
  add_definitions("-DCTRLPORT_USE_STD_SHARED_PTR=1")
endif()

include_directories(${THRIFT_INCLUDE_DIRS})
MATH(EXPR CTRLPORT_BACKENDS "${CTRLPORT_BACKENDS} + 1")

# Indicate thrift as an installed backend in the cmake summary.
message(STATUS "Found and enabling Thrift backend to ControlPort")
GR_APPEND_SUBCOMPONENT("thrift")

# Run Thrift To compile C++ and Python files
message(STATUS "Running thrift to build C++ bindings")
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/thrift/)

list(APPEND gnuradio_thrift_generated_sources
  ${CMAKE_CURRENT_BINARY_DIR}/thrift/gnuradio_types.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/thrift/gnuradio_constants.cpp
  ${CMAKE_CURRENT_BINARY_DIR}/thrift/ControlPort.cpp
  )
add_custom_command(
  DEPENDS ${CMAKE_SOURCE_DIR}/gnuradio-runtime/lib/controlport/thrift/gnuradio.thrift
  OUTPUT ${gnuradio_thrift_generated_sources}
  COMMAND ${THRIFT_BIN} --gen cpp -out ${CMAKE_CURRENT_BINARY_DIR}/thrift/ ${CMAKE_CURRENT_SOURCE_DIR}/thrift/gnuradio.thrift
  )
list(APPEND gnuradio_ctrlport_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/thrift/rpcserver_thrift.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/thrift/rpcpmtconverters_thrift.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/thrift/rpcserver_booter_thrift.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/thrift/thrift_application_base.cc
  # add files built by compiling gnuradio.thrift
  ${gnuradio_thrift_generated_sources}
  )

# Add  required libraries here
list(APPEND gnuradio_runtime_libs
  ${THRIFT_LIBRARIES}
)

# Add install rule to move example Thrift configuration file into
# $prefix/etc/gnuradio
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/thrift/thrift.conf.example
  DESTINATION ${SYSCONFDIR}/${CMAKE_PROJECT_NAME}
  COMPONENT "runtime_runtime"
)

else(THRIFT_FOUND)

# building CTRLPORT, but not using Thrift. CTRLPORT does not require
# std::shared_ptr, can use either std::shared_ptr or
# boost::shared_ptr. If using c++11 or later, choose the former,
# else choose the latter.
if(CMAKE_CXX_STANDARD)
  if(${CMAKE_CXX_STANDARD} VERSION_GREATER_EQUAL 11)
    add_definitions("-DCTRLPORT_USE_STD_SHARED_PTR=1")
  else()
    add_definitions("-DCTRLPORT_USE_STD_SHARED_PTR=0")
  endif()
else()
  add_definitions("-DCTRLPORT_USE_STD_SHARED_PTR=0")
endif()

endif(THRIFT_FOUND)
endif(ENABLE_CTRLPORT_THRIFT)

########################################################################
# Add controlport stuff to gnuradio-runtime
########################################################################

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Save the number of backends for testing against later
set(
  CTRLPORT_BACKENDS ${CTRLPORT_BACKENDS}
  CACHE INTERNAL "Number of ControlPort backends available"
)

endif(ENABLE_GR_CTRLPORT)
