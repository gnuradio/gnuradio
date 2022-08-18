# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

include(GrPython)

gr_python_install(PROGRAMS test_tcm.py DESTINATION ${GR_PKG_TRELLIS_EXAMPLES_DIR})

install(FILES README DESTINATION ${GR_PKG_DATA_DIR}/examples/trellis)

install(
    FILES fsm_files/awgn1o1_16rsc.fsm
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
    DESTINATION ${GR_PKG_TRELLIS_EXAMPLES_DIR}/fsm_files)
