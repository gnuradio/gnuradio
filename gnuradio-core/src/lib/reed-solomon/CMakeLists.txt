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
IF(NOT MSVC) #FIXME can't build reed-solomon in MSVC.
#The rs sources use a lot of non-standard code
#like the array allocation and some c99 functions.
#In short, will not fix unless someone asks nicely.
SET(gr_core_rs_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/encode_rs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/decode_rs.c
    ${CMAKE_CURRENT_SOURCE_DIR}/init_rs.c
)

########################################################################
# Setup sources and includes
########################################################################
LIST(APPEND gnuradio_core_sources ${gr_core_rs_sources})

INSTALL(
    FILES ${CMAKE_CURRENT_SOURCE_DIR}/rs.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "core_devel"
)

########################################################################
# Register unit tests
########################################################################
IF(ENABLE_TESTING)
ADD_EXECUTABLE(gr_core_rstest
    ${gr_core_rs_sources}
    ${CMAKE_CURRENT_SOURCE_DIR}/rstest.c
    ${CMAKE_CURRENT_SOURCE_DIR}/exercise.c
)
ADD_TEST(gr-core-reed-solomon-test gr_core_rstest)
ENDIF(ENABLE_TESTING)
ENDIF(NOT MSVC)
