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

#include <gnuradio/fec/polar_encoder_systematic.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

namespace gr {
namespace fec {
namespace code {

generic_encoder::sptr polar_encoder_systematic::make(
    int block_size, int num_info_bits, std::vector<int> frozen_bit_positions)
{
    return generic_encoder::sptr(
        new polar_encoder_systematic(block_size, num_info_bits, frozen_bit_positions));
}

polar_encoder_systematic::polar_encoder_systematic(int block_size,
                                                   int num_info_bits,
                                                   std::vector<int> frozen_bit_positions)
    : polar_common(block_size, num_info_bits, frozen_bit_positions, std::vector<char>())
{
    d_volk_syst_intermediate = (unsigned char*)volk_malloc(
        sizeof(unsigned char) * block_size, volk_get_alignment());
}

polar_encoder_systematic::~polar_encoder_systematic()
{
    volk_free(d_volk_syst_intermediate);
}

void polar_encoder_systematic::generic_work(void* in_buffer, void* out_buffer)
{
    const unsigned char* in = (const unsigned char*)in_buffer;
    unsigned char* out = (unsigned char*)out_buffer;

    volk_encode(out, in);
    bit_reverse_and_reset_frozen_bits(d_volk_syst_intermediate, out);
    volk_encode_block(out, d_volk_syst_intermediate);
}

void polar_encoder_systematic::bit_reverse_and_reset_frozen_bits(
    unsigned char* outbuf, const unsigned char* inbuf)
{
    memset(outbuf, 0, sizeof(unsigned char) * block_size());
    for (int i = 0; i < num_info_bits(); i++) {
        outbuf[d_info_bit_positions[i]] = inbuf[d_info_bit_positions_reversed[i]];
    }
}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */
