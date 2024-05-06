/* -*- c++ -*- */
/*
 * Copyright 2007,2010,2013,2015 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "peak_detector2_fb_impl.h"
#include <gnuradio/io_signature.h>
#include <cstring>
#include <limits>

namespace gr {
namespace blocks {

peak_detector2_fb::sptr
peak_detector2_fb::make(float threshold_factor_rise, int look_ahead, float alpha)
{
    return gnuradio::make_block_sptr<peak_detector2_fb_impl>(
        threshold_factor_rise, look_ahead, alpha);
}

peak_detector2_fb_impl::peak_detector2_fb_impl(float threshold_factor_rise,
                                               int look_ahead,
                                               float alpha)
    : sync_block("peak_detector2_fb",
                 io_signature::make(1, 1, sizeof(float)),
                 io_signature::make2(1, 2, sizeof(char), sizeof(float))),
      d_avg(0.0f),
      d_found(false)
{
    set_threshold_factor_rise(threshold_factor_rise);
    set_look_ahead(look_ahead);
    set_alpha(alpha);
}

peak_detector2_fb_impl::~peak_detector2_fb_impl() {}

void peak_detector2_fb_impl::set_threshold_factor_rise(float thr)
{
    gr::thread::scoped_lock lock(d_setlock);
    d_threshold_factor_rise = thr;
    invalidate();
}

void peak_detector2_fb_impl::set_look_ahead(int look)
{
    gr::thread::scoped_lock lock(d_setlock);
    d_look_ahead = look;
    invalidate();
}

void peak_detector2_fb_impl::set_alpha(float alpha) { d_alpha = alpha; }

void peak_detector2_fb_impl::invalidate()
{
    d_found = false;
    set_output_multiple(1);
}

int peak_detector2_fb_impl::work(int noutput_items,
                                 gr_vector_const_void_star& input_items,
                                 gr_vector_void_star& output_items)
{
    const float* iptr = (const float*)input_items[0];
    char* optr = (char*)output_items[0];
    float* sigout;

    if (output_items.size() == 2)
        sigout = (float*)output_items[1];

    memset(optr, 0, noutput_items * sizeof(char));

    gr::thread::scoped_lock lock(d_setlock);

    // have not crossed threshold yet
    if (d_found == false) {
        for (int i = 0; i < noutput_items; i++) {
            d_avg = d_alpha * iptr[i] + (1.0f - d_alpha) * d_avg;
            if (iptr[i] > d_avg * (1.0f + d_threshold_factor_rise)) {
                d_found = true;
                set_output_multiple(d_look_ahead);
                /*
                 * Consume any samples filtered so far except the last one (which
                 * caused the threshold to be exceeded). As per documentation
                 * in peak_detector2_fb.h this sample is part of the window where
                 * the peak is to be searched. The next time work() is called
                 * execution of the code jumps to the else-if clause below. There,
                 * the first sample will be the last sample processed here (because
                 * not consumed here).
                 */
                return i;
            }
            if (output_items.size() == 2)
                sigout[i] = d_avg;
        }
        return noutput_items;
    } // end d_found==false

    // can complete in this call
    else if (noutput_items >= d_look_ahead) {
        float peak_val = iptr[0];
        int peak_ind = 0;
        if (output_items.size() == 2)
            sigout[0] = d_avg;
        /*
         * Loop starts at the second sample because the first one has already been
         * filtered (see above). Result of the maximum search is correct due
         * to initialisations above.
         */
        for (int i = 1; i < d_look_ahead; i++) {
            d_avg = d_alpha * iptr[i] + (1.0f - d_alpha) * d_avg;
            if (output_items.size() == 2)
                sigout[i] = d_avg;
            if (iptr[i] > peak_val) {
                peak_val = iptr[i];
                peak_ind = i;
            }
        }
        optr[peak_ind] = 1;

        // restart the search
        invalidate();
        return d_look_ahead;
    } // end can complete in this call

    // cannot complete in this call
    else {
        return 0; // ask for more
    }
}

} /* namespace blocks */
} /* namespace gr */
