# Copyright 2012 Free Software Foundation, Inc.
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

#####################################################################
include(GrPython)

GR_PYTHON_INSTALL(
    FILES
    Generate_LDPC_matrix_functions.py
    Generate_LDPC_matrix.py
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fec/LDPC
    COMPONENT "fec_python"
)

install(
    FILES
    n_0100_k_0023_gap_10.alist
    n_0100_k_0027_gap_04.alist
    n_0100_k_0042_gap_02.alist
    n_0300_k_0103_gap_24.alist
    n_0300_k_0122_gap_06.alist
    n_0300_k_0152_gap_03.alist
    n_0500_k_0127_gap_21.alist
    n_0500_k_0202_gap_11.alist
    n_0512_k_0130_gap_21.alist
    n_0700_k_0282_gap_16.alist
    n_1100_k_0442_gap_24.alist
    n_1300_k_0522_gap_30.alist
    n_1800_k_0902_gap_28.alist
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/fec/LDPC
)
