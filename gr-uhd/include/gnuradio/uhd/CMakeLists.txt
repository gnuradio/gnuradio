# Copyright 2011,2013 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES amsg_source.h
              api.h
              cmd_keys.h
              usrp_block.h
              usrp_sink.h
              usrp_source.h
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio/uhd)

if(ENABLE_UHD_RFNOC)
    install(
        FILES rfnoc_block.h
              rfnoc_block_generic.h
              rfnoc_ddc.h
              rfnoc_duc.h
              rfnoc_fir_filter.h
              rfnoc_graph.h
              rfnoc_replay.h
              rfnoc_rx_radio.h
              rfnoc_rx_streamer.h
              rfnoc_siggen.h
              rfnoc_tx_radio.h
              rfnoc_tx_streamer.h
              rfnoc_window.h
              rfnoc_fft.h
        DESTINATION ${GR_INCLUDE_DIR}/gnuradio/uhd)
endif(ENABLE_UHD_RFNOC)
