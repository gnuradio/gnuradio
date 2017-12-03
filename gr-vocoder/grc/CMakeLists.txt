# Copyright 2011,2016 Free Software Foundation, Inc.
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
