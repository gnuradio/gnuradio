/* -*- c++ -*- */
/*
 * Copyright 2013, 2018 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_IMPL_H
#define INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_IMPL_H

#include <gnuradio/digital/ofdm_cyclic_prefixer.h>
#include <vector>

namespace gr {
namespace digital {

class ofdm_cyclic_prefixer_impl : public ofdm_cyclic_prefixer
{
private:
    //! FFT length
    int d_fft_len;
    //!  State, that determines the current output length used.
    unsigned d_state;
    //! Variable being initialized with the largest CP.
    int d_cp_max;
    //! Variable being initialized with the smallest CP.
    int d_cp_min;
    //! Length of pulse rolloff in samples
    int d_rolloff_len;
    //! Vector, that holds different CP lengths
    std::vector<int> d_cp_lengths;
    //! Buffers the up-flank (at the beginning of the cyclic prefix)
    std::vector<float> d_up_flank;
    //! Buffers the down-flank (which trails the symbol)
    std::vector<float> d_down_flank;
    //! Vector, that holds tail of the predecessor symbol.
    std::vector<gr_complex> d_delay_line; // We do this explicitly to avoid outputting
                                          // zeroes (i.e. no history!)
    //! Holds the length tag key.
    const std::string d_len_tag_key;

protected:
    int calculate_output_stream_length(const gr_vector_int& ninput_items);

public:
    ofdm_cyclic_prefixer_impl(int fft_len,
                              const std::vector<int>& cp_lengths,
                              int rolloff_len,
                              const std::string& len_tag_key);
    ~ofdm_cyclic_prefixer_impl();

    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items);
};

} // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_OFDM_CYCLIC_PREFIXER_IMPL_H */
