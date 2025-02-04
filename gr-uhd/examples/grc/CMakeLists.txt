# Copyright 2012 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
    FILES rfnoc_addsub.grc
          rfnoc_duc_radio.grc
          rfnoc_duc_radio_multichan.grc
          rfnoc_fir_filter.grc
          rfnoc_keep1.grc
          rfnoc_logpwr_split.grc
          rfnoc_moving_average.grc
          rfnoc_null_source_sink.grc
          rfnoc_radio_ddc.grc
          rfnoc_radio_ddc_multichan.grc
          rfnoc_siggen.grc
          rfnoc_split_stream.grc
          rfnoc_switchboard.grc
          rfnoc_vector_iir_split.grc
          rfnoc_window.grc
          uhd_const_wave.grc
          uhd_fft.grc
          uhd_msg_tune.grc
          uhd_normalized_gain.grc
          uhd_siggen_gui.grc
          uhd_two_tone_loopback.grc
          uhd_wbfm_receive.grc
    DESTINATION ${GR_PKG_UHD_EXAMPLES_DIR})

if(PYQT_FOUND)
      install(FILES rfnoc_replay.grc DESTINATION ${GR_PKG_UHD_EXAMPLES_DIR})
endif(PYQT_FOUND)
