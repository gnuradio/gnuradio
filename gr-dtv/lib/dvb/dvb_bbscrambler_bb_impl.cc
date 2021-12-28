/* -*- c++ -*- */
/*
 * Copyright 2015,2016,2019 Free Software Foundation, Inc.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "dvb_bbscrambler_bb_impl.h"
#include "dvb_descrambler_constants.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace dtv {

dvb_bbscrambler_bb::sptr dvb_bbscrambler_bb::make(dvb_standard_t standard,
                                                  dvb_framesize_t framesize,
                                                  dvb_code_rate_t rate)
{
    return gnuradio::make_block_sptr<dvb_bbscrambler_bb_impl>(standard, framesize, rate);
}

/*
 * The private constructor
 */
dvb_bbscrambler_bb_impl::dvb_bbscrambler_bb_impl(dvb_standard_t standard,
                                                 dvb_framesize_t framesize,
                                                 dvb_code_rate_t rate)
    : gr::sync_block("dvb_bbscrambler_bb",
                     gr::io_signature::make(1, 1, sizeof(unsigned char)),
                     gr::io_signature::make(1, 1, sizeof(unsigned char))),
      kbch(dvb::constants::kbch_mapping.at(framesize).at(rate)),
      frame_size(framesize)
{

    init_bb_randomiser();
    set_output_multiple(kbch);
}

/*
 * Our virtual destructor.
 */
dvb_bbscrambler_bb_impl::~dvb_bbscrambler_bb_impl() {}

void dvb_bbscrambler_bb_impl::init_bb_randomiser(void)
{
    int sr = 0x4A80;
    for (unsigned char& randomise_entry : bb_randomise) {
        int b = ((sr) ^ (sr >> 1)) & 1;
        randomise_entry = b;
        sr >>= 1;
        if (b) {
            sr |= 0x4000;
        }
    }
    bb_randomize32 = (uint32_t*)&bb_randomise[0];
    bb_randomize64 = (uint64_t*)&bb_randomise[0];
}

int dvb_bbscrambler_bb_impl::work(int noutput_items,
                                  gr_vector_const_void_star& input_items,
                                  gr_vector_void_star& output_items)
{
    const uint64_t* in = (const uint64_t*)input_items[0];
    uint64_t* out = (uint64_t*)output_items[0];
    const uint32_t* inm = (const uint32_t*)input_items[0];
    uint32_t* outm = (uint32_t*)output_items[0];

    if (frame_size != dvb_framesize_t::FECFRAME_MEDIUM) {
        for (int i = 0; i < noutput_items; i += kbch) {
            for (int j = 0; j < kbch / 8; ++j) {
                *out++ = *in++ ^ bb_randomize64[j];
            }
        }
    } else {
        for (int i = 0; i < noutput_items; i += kbch) {
            for (int j = 0; j < kbch / 4; ++j) {
                *outm++ = *inm++ ^ bb_randomize32[j];
            }
        }
    }

    // Tell runtime system how many output items we produced.
    return noutput_items;
}

} /* namespace dtv */
} /* namespace gr */
