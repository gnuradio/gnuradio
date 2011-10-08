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

INCLUDE(GrPython)

GR_PYTHON_INSTALL(PROGRAMS
    transmit_path.py
    receive_path.py
    uhd_interface.py
    benchmark_tx.py
    benchmark_rx.py
    tx_voice.py
    rx_voice.py
    run_length.py
    gen_whitener.py
    digital_bert_rx.py
    digital_bert_tx.py
    tunnel.py
    DESTINATION ${GR_PKG_DATA_DIR}/examples/digital
    COMPONENT "digital_python"
)
