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
# This file included, use CMake directory variables
########################################################################
#MSVC workaround: we cant have dynamically sized arrays.
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

set(gr_core_rs_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/encode_rs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_rs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/init_rs.c
)

########################################################################
# Setup sources and includes
########################################################################
list(APPEND gnuradio_core_sources ${gr_core_rs_sources})

install(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/rs.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Register unit tests
########################################################################
if(ENABLE_TESTING)
add_executable(gr_core_rstest
    ${gr_core_rs_sources}
    ${CMAKE_CURRENT_SOURCE_DIR}/rstest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/exercise.c
)
add_test(gr-core-reed-solomon-test gr_core_rstest)
endif(ENABLE_TESTING)
