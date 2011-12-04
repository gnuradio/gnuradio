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

set(GR_SWIG_INCLUDE_DIRS ${GRUEL_INCLUDE_DIRS})
set(GR_SWIG_LIBRARIES gruel)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/pmt_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gruel ${CMAKE_CURRENT_BINARY_DIR}/../include/gruel)

GR_SWIG_MAKE(pmt_swig pmt_swig.i)

GR_SWIG_INSTALL(
    TARGETS pmt_swig
    DESTINATION ${GR_PYTHON_DIR}/gruel/pmt
    COMPONENT "gruel_python"
)

install(
    FILES gr_intrusive_ptr.i pmt_swig.i
    ${CMAKE_CURRENT_BINARY_DIR}/pmt_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "gruel_swig"
)
