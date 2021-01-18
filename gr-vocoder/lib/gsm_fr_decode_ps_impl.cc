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
#include "gsm_fr_decode_ps_impl.h"
#include <gnuradio/io_signature.h>
#include <cassert>
#include <stdexcept>

namespace gr {
namespace vocoder {

gsm_fr_decode_ps::sptr gsm_fr_decode_ps::make()
{
    return gnuradio::make_block_sptr<gsm_fr_decode_ps_impl>();
}

gsm_fr_decode_ps_impl::gsm_fr_decode_ps_impl()
    : sync_interpolator("vocoder_gsm_fr_decode_ps",
                        io_signature::make(1, 1, sizeof(gsm_frame)),
                        io_signature::make(1, 1, sizeof(short)),
                        GSM_SAMPLES_PER_FRAME)
{
    if ((d_gsm = gsm_create()) == 0)
        throw std::runtime_error("gsm_fr_decode_ps_impl: gsm_create failed");
}

gsm_fr_decode_ps_impl::~gsm_fr_decode_ps_impl() { gsm_destroy(d_gsm); }

int gsm_fr_decode_ps_impl::work(int noutput_items,
                                gr_vector_const_void_star& input_items,
                                gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];
    short* out = (short*)output_items[0];

    assert((noutput_items % GSM_SAMPLES_PER_FRAME) == 0);

    for (int i = 0; i < noutput_items; i += GSM_SAMPLES_PER_FRAME) {
        gsm_decode(d_gsm, const_cast<unsigned char*>(in), out);
        in += sizeof(gsm_frame);
        out += GSM_SAMPLES_PER_FRAME;
    }

    return noutput_items;
}

} /* namespace vocoder */
} /* namespace gr */
