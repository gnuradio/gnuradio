/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_BLOCKS_REPACK_BITS_BB_IMPL_H
#define INCLUDED_BLOCKS_REPACK_BITS_BB_IMPL_H

#include <gnuradio/blocks/repack_bits_bb.h>

namespace gr {
namespace blocks {

class repack_bits_bb_impl : public repack_bits_bb
{
private:
    int d_k; //! Bits on input stream
    int d_l; //! Bits on output stream
    const bool d_packet_mode;
    int d_in_index;            // Current bit of input byte
    int d_out_index;           // Current bit of output byte
    const bool d_align_output; //! true if the output shall be aligned, false if the input
                               //! shall be aligned
    const endianness_t d_endianness;

protected:
    int calculate_output_stream_length(const gr_vector_int& ninput_items) override;

public:
    repack_bits_bb_impl(int k,
                        int l,
                        const std::string& len_tag_key,
                        bool align_output,
                        endianness_t endianness = GR_LSB_FIRST);
    ~repack_bits_bb_impl() override;
    void set_k_and_l(int k,
                     int l) override; // callback function for bits per input byte k and
                                      // bits per output byte l
    int work(int noutput_items,
             gr_vector_int& ninput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_REPACK_BITS_BB_IMPL_H */
