# Copyright 2016 Free Software Foundation, Inc.
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

include(GrPython)

# Base stuff
GR_PYTHON_INSTALL(PROGRAMS
    example_costas.py
    example_fll.py
    example_timing.py
    run_length.py
    gen_whitener.py
    snr_estimators.py
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}
)

install(
    FILES
    burst_shaper.grc
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}
)

# Narrowband
GR_PYTHON_INSTALL(PROGRAMS
    narrowband/uhd_interface.py
    narrowband/digital_bert_rx.py
    narrowband/digital_bert_tx.py
    narrowband/benchmark_add_channel.py
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/narrowband
)

# OFDM
GR_PYTHON_INSTALL(PROGRAMS
    ofdm/benchmark_add_channel.py
    ofdm/receive_path.py
    ofdm/transmit_path.py
    ofdm/uhd_interface.py
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/ofdm
)

install(
    FILES
    ofdm/tx_ofdm.grc
    ofdm/rx_ofdm.grc
    ofdm/ofdm_loopback.grc
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/ofdm
)

# DEMOD
install(
    FILES
    demod/constellation_soft_decoder.grc
    demod/digital_freq_lock.grc
    demod/pam_sync.grc
    demod/pam_timing.grc
    demod/symbol_sync_test_complex.grc
    demod/symbol_sync_test_float.grc
    demod/symbol_sync_test_float_ted_gain.m
    demod/test_corr_est.grc
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/demod
)


# PACKET
install(
    FILES
    packet/burst_tagger.grc
    packet/example_corr_est_and_clock_sync.grc
    packet/example_corr_est_and_phase_sync.grc
    packet/example_corr_est.grc
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
    packet/tx_stage6a.grc
    packet/tx_stage6.grc
    packet/uhd_packet_rx.grc
    packet/uhd_packet_rx_tun.grc
    packet/uhd_packet_tx.grc
    packet/uhd_packet_tx_tun.grc
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/packet
)
