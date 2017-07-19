# Copyright 2012-2013 Free Software Foundation, Inc.
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

GR_INCLUDE_SUBDIRECTORY(hid)
GR_INCLUDE_SUBDIRECTORY(fcd)

########################################################################
# Setup the include and linker paths
########################################################################
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/hid
    ${CMAKE_CURRENT_SOURCE_DIR}/fcd
    ${GR_FCD_INCLUDE_DIRS}
    ${GR_AUDIO_INCLUDE_DIRS}
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_RUNTIME_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
    ${LIBUSB_INCLUDE_DIR}
)

link_directories(
	${Boost_LIBRARY_DIRS}
	${LIBUSB_LIBRARY_DIRS}
)

if(ENABLE_GR_CTRLPORT)
  ADD_DEFINITIONS(-DGR_CTRLPORT)
endif(ENABLE_GR_CTRLPORT)

include_directories(${LOG4CPP_INCLUDE_DIRS})
link_directories(${LOG4CPP_LIBRARY_DIRS})

########################################################################
# Setup library
########################################################################
list(APPEND gr_fcd_sources
    source_c_impl.cc
)

list(APPEND fcd_libs
    gnuradio-runtime
    gnuradio-blocks
    gnuradio-audio
    ${Boost_LIBRARIES}
)

if (WIN32)
	list(APPEND fcd_libs setupapi)

    #Add Windows DLL resource file if using MSVC
    if(MSVC)
        include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

        configure_file(
            ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-fcd.rc.in
            ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fcd.rc
        @ONLY)

        list(APPEND gr_fcd_sources ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-fcd.rc)
    endif(MSVC)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  FIND_LIBRARY(IOKIT_LIBRARY IOKit)
  list(APPEND fcd_libs ${IOKIT_LIBRARY})
  FIND_LIBRARY(CF_LIBRARY CoreFoundation)
  list(APPEND fcd_libs ${CF_LIBRARY})
elseif (NOT WIN32)
  list(APPEND fcd_libs
    ${LIBUSB_LIBRARIES}
  )
endif()


add_library(gnuradio-fcd SHARED ${gr_fcd_sources})
if (LINUX)
  list(APPEND fcd_libs rt)
endif()
target_link_libraries(gnuradio-fcd ${fcd_libs} ${LOG4CPP_LIBRARIES})


GR_LIBRARY_FOO(gnuradio-fcd RUNTIME_COMPONENT "fcd_runtime" DEVEL_COMPONENT "fcd_devel")
