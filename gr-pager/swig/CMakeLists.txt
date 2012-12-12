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
# Setup swig generation
########################################################################
include(GrPython)
include(GrSwig)

set(GR_SWIG_INCLUDE_DIRS
    ${GR_PAGER_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/pager_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

set(GR_SWIG_LIBRARIES gnuradio-pager)

GR_SWIG_MAKE(pager_swig pager_swig.i)

GR_SWIG_INSTALL(
    TARGETS pager_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/pager
    COMPONENT "pager_python"
)

install(
    FILES
    pager_swig.i
    pager_flex_deinterleave.i
    pager_flex_frame.i
    pager_flex_parse.i
    pager_flex_sync.i
    pager_slicer_fb.i
    ${CMAKE_CURRENT_BINARY_DIR}/pager_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "pager_swig"
)
