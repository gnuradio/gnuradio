# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
    FILES uhd_const_wave.grc
          uhd_fft.grc
          uhd_msg_tune.grc
          uhd_normalized_gain.grc
          uhd_siggen_gui.grc
          uhd_two_tone_loopback.grc
          uhd_wbfm_receive.grc
    DESTINATION ${GR_PKG_UHD_EXAMPLES_DIR})
