# Copyright 2011,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(FILES
    vocoder_alaw_decode_bs.block.yml
    vocoder_alaw_encode_sb.block.yml
    vocoder_cvsd_decode_bs.block.yml
    vocoder_cvsd_decode.block.yml
    vocoder_cvsd_encode_sb.block.yml
    vocoder_cvsd_encode.block.yml
    vocoder_g721_decode_bs.block.yml
    vocoder_g721_encode_sb.block.yml
    vocoder_g723_24_decode_bs.block.yml
    vocoder_g723_24_encode_sb.block.yml
    vocoder_g723_40_decode_bs.block.yml
    vocoder_g723_40_encode_sb.block.yml
    vocoder_ulaw_decode_bs.block.yml
    vocoder_ulaw_encode_sb.block.yml
    DESTINATION ${GRC_BLOCKS_DIR}
)

if(LIBCODEC2_FOUND)
  install(FILES
    vocoder_codec2_decode_ps.block.yml
    vocoder_codec2_encode_sp.block.yml
    DESTINATION ${GRC_BLOCKS_DIR}
    )
endif(LIBCODEC2_FOUND)

if(LIBCODEC2_HAS_FREEDV_API)
  install(FILES
    vocoder_freedv_rx_ss.block.yml
    vocoder_freedv_tx_ss.block.yml
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "vocoder_python"
    )
endif(LIBCODEC2_HAS_FREEDV_API)

if(LIBGSM_FOUND)
  install(FILES
    vocoder_gsm_fr_decode_ps.block.yml
    vocoder_gsm_fr_encode_sp.block.yml
    DESTINATION ${GRC_BLOCKS_DIR}
    )
endif(LIBGSM_FOUND)
