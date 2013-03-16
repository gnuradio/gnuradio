# Copyright 2013 Free Software Foundation, Inc.
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
    consts.h
    derandomizer.h
    randomizer.h
    rs_decoder.h
    rs_encoder.h
    interleaver.h
    deinterleaver.h
    trellis_encoder.h
    viterbi_decoder.h
    ds_to_softds.h
    field_sync_mux.h
    field_sync_demux.h
    equalizer.h
    fs_checker.h
    bit_timing_loop.h
    fpll.h
    depad.h
    pad.h
    types.h
    basic_trellis_encoder_impl.h
    data_interleaver_impl.h
    diag_output_impl.h
    equalizer_impl.h
    equalizer_lms_impl.h
    equalizer_lms2_impl.h
    equalizer_nop_impl.h
    exp2_lp_impl.h
    fake_single_viterbi_impl.h
    fs_checker_impl.h
    fs_checker_naive_impl.h
    fs_correlator_impl.h
    fs_correlator_naive_impl.h
    pnXXX_impl.h
    randomizer_impl.h
    reed_solomon_impl.h
    root_raised_cosine_impl.h
    root_raised_cosine_bandpass_impl.h
    single_viterbi_impl.h
    slicer_agc_impl.h
    sliding_correlator_impl.h
    sssr_impl.h
    syminfo_impl.h
    sync_tag_impl.h
    trellis_encoder_impl.h
    viterbi_decoder_impl.h
    vsbtx_lp_impl.h
    convolutional_interleaver.h
    create_atsci_equalizer.h
    create_atsci_fs_checker.h
    create_atsci_fs_correlator.h
    fpll_btloop_coupling.h
    interleaver_fifo.h
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio
    COMPONENT "atsc_devel"
)

