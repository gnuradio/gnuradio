# Copyright 2012,2013,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(FILES
    api.h
    alaw_decode_bs.h
    alaw_encode_sb.h
    cvsd_decode_bs.h
    cvsd_encode_sb.h
    g721_decode_bs.h
    g721_encode_sb.h
    g723_24_decode_bs.h
    g723_24_encode_sb.h
    g723_40_decode_bs.h
    g723_40_encode_sb.h
    ulaw_decode_bs.h
    ulaw_encode_sb.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/vocoder
)
if(LIBCODEC2_FOUND)
install(FILES
    codec2.h
    codec2_decode_ps.h
    codec2_encode_sp.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/vocoder
)
endif(LIBCODEC2_FOUND)
if(LIBCODEC2_HAS_FREEDV_API)
install(FILES
    freedv_api.h
    freedv_rx_ss.h
    freedv_tx_ss.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/vocoder
    COMPONENT "vocoder_devel"
)
endif(LIBCODEC2_HAS_FREEDV_API)
if(LIBGSM_FOUND)
install(FILES
    gsm_fr_decode_ps.h
    gsm_fr_encode_sp.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/vocoder
)
endif(LIBGSM_FOUND)
