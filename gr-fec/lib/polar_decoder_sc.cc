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

#include <gnuradio/fec/polar_decoder_sc.h>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>

#include <cmath>
#include <cstdio>

namespace gr {
namespace fec {
namespace code {

generic_decoder::sptr polar_decoder_sc::make(int block_size,
                                             int num_info_bits,
                                             std::vector<int> frozen_bit_positions,
                                             std::vector<char> frozen_bit_values)
{
    return generic_decoder::sptr(new polar_decoder_sc(
        block_size, num_info_bits, frozen_bit_positions, frozen_bit_values));
}

polar_decoder_sc::polar_decoder_sc(int block_size,
                                   int num_info_bits,
                                   std::vector<int> frozen_bit_positions,
                                   std::vector<char> frozen_bit_values)
    : polar_decoder_common(
          block_size, num_info_bits, frozen_bit_positions, frozen_bit_values)
{
    d_llr_vec = (float*)volk_malloc(sizeof(float) * block_size * (block_power() + 1),
                                    volk_get_alignment());
    memset(d_llr_vec, 0, sizeof(float) * block_size * (block_power() + 1));
    d_u_hat_vec = (unsigned char*)volk_malloc(block_size * (block_power() + 1),
                                              volk_get_alignment());
    memset(d_u_hat_vec, 0, sizeof(unsigned char) * block_size * (block_power() + 1));
}

polar_decoder_sc::~polar_decoder_sc()
{
    volk_free(d_llr_vec);
    volk_free(d_u_hat_vec);
}

void polar_decoder_sc::generic_work(void* in_buffer, void* out_buffer)
{
    const float* in = (const float*)in_buffer;
    unsigned char* out = (unsigned char*)out_buffer;

    initialize_decoder(d_u_hat_vec, d_llr_vec, in);
    sc_decode(d_llr_vec, d_u_hat_vec);
    extract_info_bits(out, d_u_hat_vec);
}

void polar_decoder_sc::sc_decode(float* llrs, unsigned char* u)
{
    for (int i = 0; i < block_size(); i++) {
        butterfly(llrs, u, 0, i, i);
        u[i] = retrieve_bit_from_llr(llrs[i], i);
    }
}

unsigned char polar_decoder_sc::retrieve_bit_from_llr(float llr, const int pos)
{
    if (is_frozen_bit(pos)) {
        return next_frozen_bit();
    }
    return llr_bit_decision(llr);
}

} /* namespace code */
} /* namespace fec */
} /* namespace gr */
