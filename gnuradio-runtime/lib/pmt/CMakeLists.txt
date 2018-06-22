# Copyright 2010-2013 Free Software Foundation, Inc.
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
# This file included, use CMake directory variables
########################################################################
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_BINARY_DIR})

set(pmt_sources
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt_unv.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt_io.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt_pool.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/pmt_serialize.cc
)

list(APPEND gnuradio_pmt_libs
    ${Boost_LIBRARIES}
    ${LOG4CPP_LIBRARIES}
)

#Add Windows DLL resource file if using MSVC
if(MSVC)
    include(${CMAKE_SOURCE_DIR}/cmake/Modules/GrVersion.cmake)

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/gnuradio-pmt.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pmt.rc
    @ONLY)

    list(APPEND gr_pmt_sources
        ${CMAKE_CURRENT_BINARY_DIR}/gnuradio-pmt.rc
    )
endif(MSVC)


add_library(gnuradio-pmt SHARED ${pmt_sources})
target_link_libraries(gnuradio-pmt ${gnuradio_pmt_libs})

GR_LIBRARY_FOO(gnuradio-pmt)

if(ENABLE_STATIC_LIBS)
  add_library(gnuradio-pmt_static STATIC ${pmt_sources})

  add_dependencies(gnuradio-pmt_static)

  if(NOT WIN32)
    set_target_properties(gnuradio-pmt_static
      PROPERTIES OUTPUT_NAME gnuradio-pmt)
  endif(NOT WIN32)

  install(TARGETS gnuradio-pmt_static
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    )
endif(ENABLE_STATIC_LIBS)
