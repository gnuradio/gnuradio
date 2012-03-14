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

install(FILES
    vocoder_alaw_decode_bs.xml
    vocoder_alaw_encode_sb.xml
    vocoder_block_tree.xml
    vocoder_codec2_decode_ps.xml
    vocoder_codec2_encode_sp.xml
    vocoder_cvsd_decode_bs.xml
    vocoder_cvsd_decode.xml
    vocoder_cvsd_encode_sb.xml
    vocoder_cvsd_encode.xml
    vocoder_g721_decode_bs.xml
    vocoder_g721_encode_sb.xml
    vocoder_g723_24_decode_bs.xml
    vocoder_g723_24_encode_sb.xml
    vocoder_g723_40_decode_bs.xml
    vocoder_g723_40_encode_sb.xml
    vocoder_gsm_fr_decode_ps.xml
    vocoder_gsm_fr_encode_sp.xml
    vocoder_ulaw_decode_bs.xml
    vocoder_ulaw_encode_sb.xml
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "vocoder_python"
)
