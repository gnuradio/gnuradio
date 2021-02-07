/* -*- c++ -*- */
/*
 * Copyright 2013-2014 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "depuncture_bb_impl.h"
#include <gnuradio/io_signature.h>
#include <pmt/pmt.h>
#include <volk/volk.h>
#include <cstdio>
#include <string>

namespace gr {
namespace fec {

depuncture_bb::sptr
depuncture_bb::make(int puncsize, int puncpat, int delay, uint8_t symbol)
{
    return gnuradio::make_block_sptr<depuncture_bb_impl>(
        puncsize, puncpat, delay, symbol);
}

depuncture_bb_impl::depuncture_bb_impl(int puncsize,
                                       int puncpat,
                                       int delay,
                                       uint8_t symbol)
    : block("depuncture_bb",
            io_signature::make(1, 1, sizeof(unsigned char)),
            io_signature::make(1, 1, sizeof(unsigned char))),
      d_puncsize(puncsize),
      d_delay(delay),
      d_sym(symbol)
{
    // Create a mask of all 1's of puncsize length
    int mask = 0;
    for (int i = 0; i < d_puncsize; i++)
        mask |= 1 << i;

    // Rotate the pattern for the delay value; then mask it if there
    // are any excess 1's in the pattern.
    for (int i = 0; i < d_delay; ++i) {
        puncpat = ((puncpat & 1) << (d_puncsize - 1)) + (puncpat >> 1);
    }
    d_puncpat = puncpat & mask;

    // Calculate the number of holes in the pattern. The mask is all
    // 1's given puncsize and puncpat is a pattern with >= puncsize
    // 0's (masked to ensure this). The difference between the
    // number of 1's in the mask and the puncpat is the number of
    // holes.
    uint32_t count_mask = 0, count_pat = 0;
    volk_32u_popcnt(&count_mask, static_cast<uint32_t>(mask));
    volk_32u_popcnt(&count_pat, static_cast<uint32_t>(d_puncpat));
    d_puncholes = count_mask - count_pat;

    set_fixed_rate(true);
    set_relative_rate((uint64_t)d_puncsize, (uint64_t)(d_puncsize - d_puncholes));
    set_output_multiple(d_puncsize);
    // set_msg_handler(<portname>, [this](pmt::pmt_t msg) { this->catch_msg(msg); });
}

depuncture_bb_impl::~depuncture_bb_impl() {}

int depuncture_bb_impl::fixed_rate_ninput_to_noutput(int ninput)
{
    return std::lround((d_puncsize / (double)(d_puncsize - d_puncholes)) * ninput);
}

int depuncture_bb_impl::fixed_rate_noutput_to_ninput(int noutput)
{
    return std::lround(((d_puncsize - d_puncholes) / (double)(d_puncsize)) * noutput);
}

void depuncture_bb_impl::forecast(int noutput_items, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] =
        std::lround(((d_puncsize - d_puncholes) / (double)(d_puncsize)) * noutput_items);
}

/*
void depuncture_bb_impl::catch_msg(pmt::pmt_t msg)
{
  long mlong = pmt::pmt_to_long(msg);
  for(int i = 0; i < mlong; ++i) {
    d_puncholes = (d_puncholes >> 1) | ((d_puncholes & 1) << (d_puncsize - 1));
  }
}
*/

int depuncture_bb_impl::general_work(int noutput_items,
                                     gr_vector_int& ninput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    const uint8_t* in = (const uint8_t*)input_items[0];
    uint8_t* out = (uint8_t*)output_items[0];

    for (int i = 0, k = 0; i < noutput_items / output_multiple(); ++i) {
        for (int j = 0; j < output_multiple(); ++j) {
            out[i * output_multiple() + j] =
                ((d_puncpat >> (d_puncsize - 1 - j)) & 1) ? in[k++] : d_sym;
        }
    }

    /*
    GR_LOG_DEBUG(d_debug_logger, ">>>>>> start");
    for(int i = 0, k=0; i < noutput_items; ++i) {
      if((d_puncpat >> (d_puncsize - 1 - (i % d_puncsize))) & 1) {
        GR_LOG_DEBUG(d_debug_logger, boost::format("%1%...%2%") \
                     % out[i] % in[k++]);
      }
      else {
        GR_LOG_DEBUG(d_debug_logger, boost::format("snit %1%") % out[i]);
      }
    }

    GR_LOG_DEBUG(d_debug_logger, boost::format("comp: %1%, %2%\n") \
                 % noutput_items % ninput_items[0]);
    GR_LOG_DEBUG(d_debug_logger, boost::format("consuming %1%") \
                 % ((int)(((1.0/relative_rate()) * noutput_items) + .5)));
    */

    consume_each(std::lround((1.0 / relative_rate()) * noutput_items));
    return noutput_items;
}

} /* namespace fec */
} /* namespace gr */
