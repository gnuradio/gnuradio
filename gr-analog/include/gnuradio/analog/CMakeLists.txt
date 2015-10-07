# Copyright 2012,2014 Free Software Foundation, Inc.
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
# Invoke macro to generate various headers
########################################################################
include(GrMiscUtils)
GR_EXPAND_X_H(analog noise_source_X     s i f c)
GR_EXPAND_X_H(analog fastnoise_source_X s i f c)
GR_EXPAND_X_H(analog sig_source_X       s i f c)
GR_EXPAND_X_H(analog random_uniform_source_X b s i)

add_custom_target(analog_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${generated_includes}
    api.h
    cpm.h
    noise_type.h
    agc.h
    agc2.h
    noise_type.h
    squelch_base_ff.h
    squelch_base_cc.h
    agc_cc.h
    agc_ff.h
    agc2_cc.h
    agc2_ff.h
    agc3_cc.h
    cpfsk_bc.h
    ctcss_squelch_ff.h
    dpll_bb.h
    feedforward_agc_cc.h
    fmdet_cf.h
    frequency_modulator_fc.h
    phase_modulator_fc.h
    pll_carriertracking_cc.h
    pll_freqdet_cf.h
    pll_refout_cc.h
    probe_avg_mag_sqrd_c.h
    probe_avg_mag_sqrd_cf.h
    probe_avg_mag_sqrd_f.h
    pwr_squelch_cc.h
    pwr_squelch_ff.h
    quadrature_demod_cf.h
    rail_ff.h
    sig_source_waveform.h
    simple_squelch_cc.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/analog
    COMPONENT "analog_devel"
)

