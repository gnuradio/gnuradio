# Copyright 2011 Free Software Foundation, Inc.
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

LIST(APPEND gr_digital_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/digital_gmskmod_bc.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/digital_cpmmod_bc.cc
)

INSTALL(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/digital_gmskmod_bc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/digital_cpmmod_bc.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "digital_devel"
)

INSTALL(
    FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/_digital_hier.i
    ${CMAKE_CURRENT_SOURCE_DIR}/digital_gmskmod_bc.i
    ${CMAKE_CURRENT_SOURCE_DIR}/digital_cpmmod_bc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "digital_swig"
)
