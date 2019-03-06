# Copyright 2010-2012 Free Software Foundation, Inc.
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
#MSVC workaround: we can't have dynamically sized arrays.
#So ifdef a max array bounds that is larger than NN and NROOTS
#Its a bit of a hack, but if you look at the code, its so full of ifdefs,
#and lacks optimization where it should be pre-allocating these arrays.
if(MSVC)
    set_source_files_properties(
        ${CMAKE_CURRENT_SOURCE_DIR}/exercise.c
        ${CMAKE_CURRENT_SOURCE_DIR}/decode_rs.c
        PROPERTIES COMPILE_DEFINITIONS "MAX_ARRAY=256;"
    )
endif(MSVC)

add_library(gr_fec_rs OBJECT
    ${CMAKE_CURRENT_SOURCE_DIR}/encode_rs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_rs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/init_rs.c
  )
target_include_directories(gr_fec_rs
  PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../include>
  PRIVATE $<TARGET_PROPERTY:gnuradio-runtime,INCLUDE_DIRECTORIES>
  )
set_target_properties(gr_fec_rs PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(gnuradio-fec PRIVATE $<TARGET_OBJECTS:gr_fec_rs>)

########################################################################
# Register unit tests
########################################################################
if(ENABLE_TESTING)
add_executable(gr_fec_rstest
    ${CMAKE_CURRENT_SOURCE_DIR}/exercise.c
    ${CMAKE_CURRENT_SOURCE_DIR}/rstest.c
    $<TARGET_OBJECTS:gr_fec_rs>
    )
  target_link_libraries(gr_fec_rstest gnuradio-runtime)
  target_include_directories(gr_fec_rstest
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../../include
    )
add_test(test_rs gr_fec_rstest)
endif(ENABLE_TESTING)
