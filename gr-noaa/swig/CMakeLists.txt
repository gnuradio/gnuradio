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
    ${GR_NOAA_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/noaa_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../lib)

set(GR_SWIG_LIBRARIES gnuradio-noaa)

GR_SWIG_MAKE(noaa_swig noaa_swig.i)

GR_SWIG_INSTALL(
    TARGETS noaa_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/noaa
    COMPONENT "noaa_python"
)

install(
    FILES
    noaa_swig.i
    noaa_hrpt_decoder.i
    noaa_hrpt_deframer.i
    noaa_hrpt_pll_cf.i
    ${CMAKE_CURRENT_BINARY_DIR}/noaa_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "noaa_swig"
)

GR_PYTHON_INSTALL(
    FILES
    __init__.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/noaa
    COMPONENT "noaa_python"
)
