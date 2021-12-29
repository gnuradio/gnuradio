# Copyright 2012,2014 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(
    FILES api.h
          cpm.h
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
          fastnoise_source.h
          feedforward_agc_cc.h
          fmdet_cf.h
          frequency_modulator_fc.h
          noise_source.h
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
          random_uniform_source.h
          sig_source.h
          sig_source_waveform.h
          simple_squelch_cc.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/analog)
