/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "plateau_detector_fb_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
namespace blocks {

plateau_detector_fb::sptr plateau_detector_fb::make(int max_len, float threshold)
{
    return gnuradio::make_block_sptr<plateau_detector_fb_impl>(max_len, threshold);
}

plateau_detector_fb_impl::plateau_detector_fb_impl(int max_len, float threshold)
    : block("plateau_detector_fb",
            io_signature::make(1, 1, sizeof(float)),
            io_signature::make(1, 1, sizeof(char))),
      d_max_len(max_len),
      d_threshold(threshold)
{
}

plateau_detector_fb_impl::~plateau_detector_fb_impl() {}

void plateau_detector_fb_impl::forecast(int, gr_vector_int& ninput_items_required)
{
    ninput_items_required[0] = 2 * d_max_len;
}

int plateau_detector_fb_impl::general_work(int noutput_items,
                                           gr_vector_int& ninput_items,
                                           gr_vector_const_void_star& input_items,
                                           gr_vector_void_star& output_items)
{
    // thread-safe protection from ::set_threshold
    gr::thread::scoped_lock l(d_setlock);

    const float* in = (const float*)input_items[0];
    unsigned char* out = (unsigned char*)output_items[0];
    int flank_start;
    noutput_items = std::min(noutput_items, ninput_items[0]);
    memset((void*)out, 0x00, noutput_items);
    int i;
    for (i = 0; i < noutput_items; i++) {
        if (in[i] >= d_threshold) {
            if (noutput_items - i <
                2 * d_max_len) { // If we can't finish, come back later
                break;
            }
            flank_start = i;
            while (i < noutput_items && in[i] >= d_threshold)
                i++;
            if ((i - flank_start) > 1) { // 1 Sample is not a plateau
                out[flank_start + (i - flank_start) / 2] = 1;
                i = std::min(i + d_max_len, noutput_items - 1);
            }
        }
    }

    this->consume_each(i);
    return i;
}

void plateau_detector_fb_impl::set_threshold(float threshold)
{
    // thread-safe protection from ::set_threshold
    gr::thread::scoped_lock l(d_setlock);
    d_threshold = threshold;
}

float plateau_detector_fb_impl::threshold() const { return d_threshold; }

} /* namespace blocks */
} /* namespace gr */
