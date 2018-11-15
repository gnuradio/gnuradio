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

set(viterbi_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/metrics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/tab.c
    ${CMAKE_CURRENT_SOURCE_DIR}/viterbi.c
)

########################################################################
# Append gnuradio-runtime library sources
########################################################################
SET_SOURCE_FILES_PROPERTIES( ${viterbi_sources} PROPERTIES LANGUAGE CXX )
list(APPEND gnuradio_fec_sources ${viterbi_sources})

########################################################################
# Install runtime headers
########################################################################
#install(
#    FILES ${CMAKE_CURRENT_SOURCE_DIR}/viterbi.h
#    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/fec
#)

########################################################################
# Create some text executables (not registered tests)
# Its not much to build so the sources are just re-listed,
# rather than create a new library just for these two apps.
########################################################################
#ADD_EXECUTABLE(viterbi_encode ${CMAKE_CURRENT_SOURCE_DIR}/encode.cc ${viterbi_sources})
#ADD_EXECUTABLE(viterbi_decode ${CMAKE_CURRENT_SOURCE_DIR}/decode.cc ${viterbi_sources})
