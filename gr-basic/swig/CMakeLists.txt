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
    ${GR_BASIC_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

set(GR_SWIG_LIBRARIES gnuradio-basic)

GR_SWIG_MAKE(basic_swig basic_swig.i)

GR_SWIG_INSTALL(
    TARGETS basic_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/basic
    COMPONENT "basic_python"
)

install(
    FILES
    basic_swig.i
    basic_delay.i
    basic_sig_source.i
    basic_stream_selector.i
    basic_ops.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "basic_swig"
)
