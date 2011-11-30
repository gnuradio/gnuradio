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
    ${GR_VOCODER_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/vocoder_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(GR_SWIG_LIBRARIES gnuradio-vocoder)

GR_SWIG_MAKE(vocoder_swig vocoder_swig.i)

GR_SWIG_INSTALL(
    TARGETS vocoder_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/vocoder
    COMPONENT "vocoder_python"
)

install(
    FILES
    vocoder_swig.i
    vocoder_alaw_decode_bs.i
    vocoder_alaw_encode_sb.i
    vocoder_codec2_decode_ps.i
    vocoder_codec2_encode_sp.i
    vocoder_cvsd_decode_bs.i
    vocoder_cvsd_encode_sb.i
    vocoder_g721_decode_bs.i
    vocoder_g721_encode_sb.i
    vocoder_g723_24_decode_bs.i
    vocoder_g723_24_encode_sb.i
    vocoder_g723_40_decode_bs.i
    vocoder_g723_40_encode_sb.i
    vocoder_gsm_fr_encode_sp.i
    vocoder_gsm_fr_decode_ps.i
    vocoder_ulaw_decode_bs.i
    vocoder_ulaw_encode_sb.i
    ${CMAKE_CURRENT_BINARY_DIR}/vocoder_swig_doc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "vocoder_swig"
)
