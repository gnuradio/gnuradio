# Copyright 2012 Free Software Foundation, Inc.
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
# Setup swig generation
########################################################################
include(GrPython)
include(GrSwig)

set(GR_SWIG_TARGET_DEPS core_swig)

set(GR_SWIG_INCLUDE_DIRS
    ${GR_BLOCKS_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}/../include
)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/blocks_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

set(GR_SWIG_LIBRARIES gnuradio-blocks)

GR_SWIG_MAKE(blocks_swig blocks_swig.i)

GR_SWIG_INSTALL(
    TARGETS blocks_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/blocks
    COMPONENT "blocks_python"
)

install(
    FILES
    blocks_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/blocks_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "blocks_swig"
)
