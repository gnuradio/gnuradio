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
    trellis_encoder_xx.xml
    trellis_siso_combined_f.xml
    trellis_viterbi_x.xml
    trellis_metrics_x.xml
    trellis_siso_f.xml
    trellis_permutation.xml
    trellis_viterbi_combined_xx.xml
    trellis_sccc_encoder_xx.xml
    trellis_sccc_decoder_x.xml
    trellis_sccc_decoder_combined_xx.xml
    trellis_pccc_encoder_xx.xml
    trellis_pccc_decoder_x.xml
    trellis_pccc_decoder_combined_xx.xml
    DESTINATION ${GRC_BLOCKS_DIR}
    COMPONENT "trellis_python"
)
