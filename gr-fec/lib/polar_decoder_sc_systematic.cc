/* -*- c++ -*- */
/*
 * Copyright 2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/fec/polar_decoder_sc_systematic.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace fec {
namespace code {

generic_decoder::sptr polar_decoder_sc_systematic::make(
    int block_size, int num_info_bits, std::vector<int> frozen_bit_positions)
{
    return generic_decoder::sptr(
        new polar_decoder_sc_systematic(block_size, num_info_bits, frozen_bit_positions));
}

polar_decoder_sc_systematic::polar_decoder_sc_systematic(
    int block_size, int num_info_bits, std::vector<int> frozen_bit_positions)
    : polar_decoder_common(
          block_size, num_info_bits, frozen_bit_positions, std::vector<uint8_t>()),
      d_llr_vec(block_size * (block_power() + 1)),
      d_u_hat_vec(block_size * (block_power() + 1)),
      d_frame_vec(block_size)
{
}

polar_decoder_sc_systematic::~polar_decoder_sc_systematic() {}

void polar_decoder_sc_systematic::generic_work(void* in_buffer, void* out_buffer)
{
    const float* in = (const float*)in_buffer;
    unsigned char* out = (unsigned char*)out_buffer;

    initialize_decoder(d_u_hat_vec.data(), d_llr_vec.data(), in);
    sc_decode(d_llr_vec.data(), d_u_hat_vec.data());
    volk_encode_block(d_frame_vec.data(), d_u_hat_vec.data());
    extract_info_bits_reversed(out, d_frame_vec.data());
}

void polar_decoder_sc_systematic::sc_decode(float* llrs, unsigned char* u)
{
    for (int i = 0; i < block_size(); i++) {
        butterfly(llrs, u, 0, i, i);
        u[i] = retrieve_bit_from_llr(llrs[i], i);
    }
}

unsigned char polar_decoder_sc_systematic::retrieve_bit_from_llr(float llr, const int pos)
{
    if (is_frozen_bit(pos)) {
        return next_frozen_bit();
    }
    return llr_bit_decision(llr);
}

void polar_decoder_sc_systematic::extract_info_bits_reversed(unsigned char* outbuf,
                                                             const unsigned char* inbuf)
{
    for (int i = 0; i < num_info_bits(); i++) {
        *outbuf++ = inbuf[d_info_bit_positions_reversed[i]];
    }
}

} // namespace code
} /* namespace fec */
} /* namespace gr */
