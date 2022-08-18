# Copyright 2012,2014,2018 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Install header files
########################################################################
install(
    FILES api.h
          base.h
          calc_metric.h
          constellation_metrics_cf.h
          core_algorithms.h
          encoder.h
          fsm.h
          interleaver.h
          metrics.h
          pccc_decoder_blk.h
          pccc_decoder_combined_blk.h
          pccc_encoder.h
          permutation.h
          quicksort_index.h
          sccc_encoder.h
          sccc_decoder_blk.h
          sccc_decoder_combined_blk.h
          siso_type.h
          siso_combined_f.h
          siso_f.h
          viterbi.h
          viterbi_combined.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/trellis)
