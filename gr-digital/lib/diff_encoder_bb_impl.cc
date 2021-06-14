/* -*- c++ -*- */
/*
 * Copyright 2006,2010,2012 Free Software Foundation, Inc.
 * Copyright 2021 Daniel Estevez <daniel@destevez.net>
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "diff_encoder_bb_impl.h"
#include <gnuradio/io_signature.h>

#include <stdexcept>

namespace gr {
namespace digital {

diff_encoder_bb::sptr diff_encoder_bb::make(unsigned int modulus,
                                            enum diff_coding_type coding)
{
    return gnuradio::make_block_sptr<diff_encoder_bb_impl>(modulus, coding);
}

diff_encoder_bb_impl::diff_encoder_bb_impl(unsigned int modulus,
                                           enum diff_coding_type coding)
    : sync_block("diff_encoder_bb",
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 io_signature::make(1, 1, sizeof(unsigned char))),
      d_last_out(0),
      d_modulus(modulus),
      d_coding(coding)
{
    if (d_coding == DIFF_NRZI && d_modulus != 2) {
        throw std::runtime_error("diff_encoder: NRZI only supported with modulus 2");
    }
}

diff_encoder_bb_impl::~diff_encoder_bb_impl() {}

int diff_encoder_bb_impl::work(int noutput_items,
                               gr_vector_const_void_star& input_items,
                               gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    unsigned last_out = d_last_out;

    if (d_coding == DIFF_NRZI) {
        for (int i = 0; i < noutput_items; i++) {
            out[i] = ~(in[i] ^ last_out) & 1;
            last_out = out[i];
        }
    } else if (d_modulus == 2) {
        // optimized implementation for modulus 2
        for (int i = 0; i < noutput_items; i++) {
            out[i] = (in[i] ^ last_out) & 1;
            last_out = out[i];
        }
    } else {
        // implementation for modulus != 2
        for (int i = 0; i < noutput_items; i++) {
            out[i] = (in[i] + last_out) % d_modulus;
            last_out = out[i];
        }
    }

    d_last_out = last_out;
    return noutput_items;
}

} /* namespace digital */
} /* namespace gr */
