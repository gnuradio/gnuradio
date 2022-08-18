# Copyright 2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

# Base stuff
gr_python_install(
    PROGRAMS berawgn.py
             example_costas.py
             example_fll.py
             example_timing.py
             gen_whitener.py
             run_length.py
             snr_estimators.py
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR})

install(FILES burst_shaper.grc mpsk_stage6.grc DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR})

# Narrowband
gr_python_install(
    PROGRAMS narrowband/benchmark_add_channel.py narrowband/digital_bert_rx.py
             narrowband/digital_bert_tx.py narrowband/uhd_interface.py
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/narrowband)

# OFDM
gr_python_install(
    PROGRAMS ofdm/benchmark_add_channel.py ofdm/receive_path.py ofdm/transmit_path.py
             ofdm/uhd_interface.py DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/ofdm)

install(FILES ofdm/ofdm_sync.m ofdm/ofdm_sync_pn.m ofdm/plot_ofdm.m ofdm/ofdm_loopback.grc
              ofdm/rx_ofdm.grc ofdm/tx_ofdm.grc
        DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/ofdm)

# DEMOD
install(
    FILES demod/constellation_soft_decoder.grc
          demod/digital_freq_lock.grc
          demod/pam_sync.grc
          demod/pam_timing.grc
          demod/symbol_sync_test_complex.grc
          demod/symbol_sync_test_float.grc
          demod/symbol_sync_test_float_ted_gain.m
          demod/test_corr_est.grc
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/demod)

# PACKET
install(
    FILES packet/burst_tagger.grc
          packet/example_corr_est.grc
          packet/example_corr_est_and_clock_sync.grc
          packet/example_corr_est_and_phase_sync.grc
          packet/formatter_crc.grc
          packet/formatter_ofdm.grc
          packet/packet_loopback_hier.grc
          packet/packet_rx.grc
          packet/packet_tx.grc
          packet/simple_bpsk_tx.grc
          packet/transmitter_sim_hier.grc
          packet/tx_stage0.grc
          packet/tx_stage1.grc
          packet/tx_stage2.grc
          packet/tx_stage3.grc
          packet/tx_stage4.grc
          packet/tx_stage5.grc
          packet/tx_stage6.grc
          packet/tx_stage6a.grc
          packet/uhd_packet_rx.grc
          packet/uhd_packet_rx_tun.grc
          packet/uhd_packet_tx.grc
          packet/uhd_packet_tx_tun.grc
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/packet)

# Equalizers
gr_python_install(
    PROGRAMS equalizers/linear_equalizer_compare.grc equalizers/le_vs_dfe.grc
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/equalizers)
