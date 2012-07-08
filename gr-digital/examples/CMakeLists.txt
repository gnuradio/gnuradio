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
    COMPONENT "digital_python"
)

# Narrowband
GR_PYTHON_INSTALL(PROGRAMS
    narrowband/transmit_path.py
    narrowband/receive_path.py
    narrowband/uhd_interface.py
    narrowband/benchmark_tx.py
    narrowband/benchmark_rx.py
    narrowband/tx_voice.py
    narrowband/rx_voice.py
    narrowband/digital_bert_rx.py
    narrowband/digital_bert_tx.py
    narrowband/tunnel.py
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/narrowband
    COMPONENT "digital_python"
)

# OFDM
GR_PYTHON_INSTALL(PROGRAMS
    ofdm/benchmark_add_channel.py
    ofdm/benchmark_rx.py
    ofdm/benchmark_tx.py
    ofdm/gr_plot_ofdm.py
    ofdm/ofdm_mod_demod_test.py
    ofdm/receive_path.py
    ofdm/transmit_path.py
    ofdm/tunnel.py
    ofdm/uhd_interface.py
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/ofdm
    COMPONENT "digital_python"
)

# DEMOD
install(
    FILES
    demod/digital_freq_lock.grc
    demod/mpsk_demod.grc
    demod/pam_sync.grc
    demod/pam_timing.grc
    demod/ber_simulation.grc
    demod/dpsk_loopback.grc   
    demod/gfsk_loopback.grc
    DESTINATION ${GR_PKG_DIGITAL_EXAMPLES_DIR}/demod
    COMPONENT "digital_python"
)
