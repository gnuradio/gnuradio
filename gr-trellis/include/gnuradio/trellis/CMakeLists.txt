# Copyright 2012,2014 Free Software Foundation, Inc.
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
# Invoke macro to generate various headers
########################################################################
include(GrMiscUtils)
GR_EXPAND_X_H(trellis encoder_XX                 bb bs bi ss si ii)
GR_EXPAND_X_H(trellis sccc_encoder_XX            bb bs bi ss si ii)
GR_EXPAND_X_H(trellis pccc_encoder_XX            bb bs bi ss si ii)
GR_EXPAND_X_H(trellis metrics_X                  s i f c)
GR_EXPAND_X_H(trellis viterbi_X                  b s i)
GR_EXPAND_X_H(trellis viterbi_combined_XX        sb ss si ib is ii fb fs fi cb cs ci)
GR_EXPAND_X_H(trellis sccc_decoder_X             b s i)
GR_EXPAND_X_H(trellis sccc_decoder_combined_XX   fb fs fi cb cs ci)
GR_EXPAND_X_H(trellis pccc_decoder_X             b s i)
GR_EXPAND_X_H(trellis pccc_decoder_combined_XX   fb fs fi cb cs ci)

add_custom_target(trellis_generated_includes DEPENDS
    ${generated_includes}
)

########################################################################
# Install header files
########################################################################
install(FILES
    ${generated_includes}
    api.h
    base.h
    calc_metric.h
    constellation_metrics_cf.h
    core_algorithms.h
    fsm.h
    interleaver.h
    permutation.h
    quicksort_index.h
    siso_type.h
    siso_combined_f.h
    siso_f.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/trellis
    COMPONENT "trellis_devel"
)

