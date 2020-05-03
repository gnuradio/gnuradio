/* -*- c++ -*- */
/*
 * Copyright 2005,2010,2013,2016 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define GSM_SAMPLES_PER_FRAME 160
#include "gsm_fr_encode_sp_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
namespace vocoder {

gsm_fr_encode_sp::sptr gsm_fr_encode_sp::make()
{
    return gnuradio::make_block_sptr<gsm_fr_encode_sp_impl>();
}

gsm_fr_encode_sp_impl::gsm_fr_encode_sp_impl()
    : sync_decimator("vocoder_gsm_fr_encode_sp",
                     io_signature::make(1, 1, sizeof(short)),
                     io_signature::make(1, 1, sizeof(gsm_frame)),
                     GSM_SAMPLES_PER_FRAME)
{
    if ((d_gsm = gsm_create()) == 0)
        throw std::runtime_error("gsm_fr_encode_sp_impl: gsm_create failed");
}

gsm_fr_encode_sp_impl::~gsm_fr_encode_sp_impl() { gsm_destroy(d_gsm); }

int gsm_fr_encode_sp_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const short* in = (const short*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];

    for (int i = 0; i < noutput_items; i++) {
        gsm_encode(d_gsm, const_cast<short*>(in), out);
        in += GSM_SAMPLES_PER_FRAME;
        out += sizeof(gsm_frame);
    }

    return noutput_items;
}

} /* namespace vocoder */
} /* namespace gr */
