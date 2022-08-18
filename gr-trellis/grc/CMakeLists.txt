# Copyright 2011 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
    FILES trellis_encoder_xx.block.yml
          trellis_siso_combined_f.block.yml
          trellis_viterbi_x.block.yml
          trellis_metrics_x.block.yml
          trellis_siso_f.block.yml
          trellis_permutation.block.yml
          trellis_viterbi_combined_xx.block.yml
          trellis_sccc_encoder_xx.block.yml
          trellis_sccc_decoder_x.block.yml
          trellis_sccc_decoder_combined_xx.block.yml
          trellis_pccc_encoder_xx.block.yml
          trellis_pccc_decoder_x.block.yml
          trellis_pccc_decoder_combined_xx.block.yml
    DESTINATION ${GRC_BLOCKS_DIR})
