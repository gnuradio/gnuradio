/* -*- c++ -*- */
/*
 * Copyright 2011,2013,2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef INCLUDED_VOCODER_CODEC2_DECODE_PS_IMPL_H
#define INCLUDED_VOCODER_CODEC2_DECODE_PS_IMPL_H

#include <gnuradio/vocoder/codec2_decode_ps.h>

namespace gr {
namespace vocoder {

class codec2_decode_ps_impl : public codec2_decode_ps
{
private:
    CODEC2* d_codec2;
    int d_samples_per_frame, d_bits_per_frame, d_bytes_per_frame;
    std::vector<unsigned char> d_frame_buf; //!< Store 1 packed frame for decoding

    void pack_frame(
        const unsigned char* in_unpacked,
        unsigned char* out_packed); //!< Pack the bytes from unpacked bits for codec2

public:
    codec2_decode_ps_impl(int mode, int samples_per_frame, int bits_per_frame);
    ~codec2_decode_ps_impl() override;

    int work(int noutput_items,
             gr_vector_const_void_star& input_items,
             gr_vector_void_star& output_items) override;
};

} /* namespace vocoder */
} /* namespace gr */

#endif /* INCLUDED_VOCODER_CODEC2_DECODE_PS_IMPL_H */
