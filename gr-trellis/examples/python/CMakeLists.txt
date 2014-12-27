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
    test_tcm.py
    test_cpm.py
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
    fsm_files/awgn1o1_16rsc.fsm
    fsm_files/awgn1o1_4rsc.fsm
    fsm_files/awgn1o2_128.fsm
    fsm_files/awgn1o2_16.fsm
    fsm_files/awgn1o2_16rsc.fsm
    fsm_files/awgn1o2_4.fsm
    fsm_files/awgn1o2_4rsc.fsm
    fsm_files/awgn1o2_8.fsm
    fsm_files/awgn2o3_16.fsm
    fsm_files/awgn2o3_16ungerboecka.fsm
    fsm_files/awgn2o3_16ungerboeck.fsm
    fsm_files/awgn2o3_32ungerboecka.fsm
    fsm_files/awgn2o3_32ungerboeck.fsm
    fsm_files/awgn2o3_4.fsm
    fsm_files/awgn2o3_4_msb.fsm
    fsm_files/awgn2o3_4_msbG.fsm
    fsm_files/awgn2o3_4ungerboecka.fsm
    fsm_files/awgn2o3_4ungerboeck.fsm
    fsm_files/awgn2o3_64ungerboecka.fsm
    fsm_files/awgn2o3_64ungerboeck.fsm
    fsm_files/awgn2o3_8.fsm
    fsm_files/awgn2o3_8ungerboecka.fsm
    fsm_files/awgn2o3_8ungerboeck.fsm
    fsm_files/awgn2o4_4.fsm
    fsm_files/disconnected.fsm
    fsm_files/irregular.fsm
    fsm_files/joint_16_16.fsm
    fsm_files/joint_4_16.fsm
    fsm_files/rep3.fsm
    fsm_files/rep5.fsm
    fsm_files/simple.fsm
    fsm_files/uncoded4.fsm
    DESTINATION ${GR_PKG_TRELLIS_EXAMPLES_DIR}/fsm_files
    COMPONENT "trellis_examples"
)
