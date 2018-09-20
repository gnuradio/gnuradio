# Copyright 2012,2014,2018 Free Software Foundation, Inc.
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

########################################################################
# Install header files
########################################################################
install(FILES
    api.h
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
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/trellis
)
