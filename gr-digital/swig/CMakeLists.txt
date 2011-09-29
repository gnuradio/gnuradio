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
INCLUDE(GrPython)
INCLUDE(GrSwig)

SET(GR_SWIG_INCLUDE_DIRS
    ${GR_DIGITAL_INCLUDE_DIRS}
    ${GNURADIO_CORE_SWIG_INCLUDE_DIRS}
)

SET(GR_SWIG_LIBRARIES gnuradio-digital)

GR_SWIG_MAKE(digital_swig digital_swig.i)

GR_SWIG_INSTALL(
    TARGETS digital_hier digital_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/digital
    COMPONENT "digital_python"
)

INSTALL(
    FILES
    digital_binary_slicer_fb.i
    digital_clock_recovery_mm_cc.i
    digital_clock_recovery_mm_ff.i
    digital_constellation.i
    digital_constellation_receiver_cb.i
    digital_constellation_decoder_cb.i
    digital_correlate_access_code_bb.i
    digital_costas_loop_cc.i
    digital_cma_equalizer_cc.i
    digital_crc32.i
    digital_fll_band_edge_cc.i
    digital_lms_dd_equalizer_cc.i
    digital_kurtotic_equalizer_cc.i
    digital_mpsk_receiver_cc.i
    digital_gmskmod_bc.i
    digital_cpmmod_bc.i
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/swig
    COMPONENT "digital_swig"
)
