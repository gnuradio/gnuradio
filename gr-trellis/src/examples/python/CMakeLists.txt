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

GR_PYTHON_INSTALL(
    PROGRAMS
    fsm_utils.py
    test_tcm.py
    test_tcm_parallel.py
    test_tcm_combined.py
    test_sccc_hard.py
    test_sccc_soft.py
    test_sccc_turbo.py
    test_viterbi_equalization1.py
    test_viterbi_equalization.py
    test_turbo_equalization.py
    test_turbo_equalization1.py
    test_turbo_equalization2.py
    DESTINATION ${GR_PKG_TRELLIS_EXAMPLES_DIR}
    COMPONENT "trellis_examples"
)

install(
    FILES README
    DESTINATION ${GR_PKG_DATA_DIR}/examples/trellis
    COMPONENT "trellis_examples"
)

install(
    FILES
    fsm_files/awgn1o2_128.fsm
    fsm_files/awgn1o2_16.fsm
    fsm_files/awgn1o2_4.fsm
    fsm_files/awgn1o2_8.fsm
    fsm_files/awgn2o3_16.fsm
    fsm_files/awgn2o3_4.fsm
    fsm_files/awgn2o3_4_msb.fsm
    fsm_files/awgn2o3_4_msbG.fsm
    fsm_files/awgn2o3_8.fsm
    fsm_files/awgn2o4_4.fsm
    fsm_files/disconnected.fsm
    fsm_files/rep3.fsm
    fsm_files/rep5.fsm
    fsm_files/simple.fsm
    DESTINATION ${GR_PKG_TRELLIS_EXAMPLES_DIR}/fsm_files
    COMPONENT "trellis_examples"
)
