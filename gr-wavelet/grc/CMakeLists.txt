# Copyright 2026 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
    FILES wavelet.tree.yml
          wavelet_wavelet_ff.block.yml
          wavelet_wvps_ff.block.yml
          wavelet_squash_ff.block.yml
    DESTINATION ${GRC_BLOCKS_DIR})
