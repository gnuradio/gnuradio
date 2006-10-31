/*
 * Copyright 2004,2006 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <pager_slicer_fb.h>
#include <gr_io_signature.h>

pager_slicer_fb_sptr pager_make_slicer_fb(float alpha, float beta)
{
    return pager_slicer_fb_sptr(new pager_slicer_fb(alpha, beta));
}

pager_slicer_fb::pager_slicer_fb(float alpha, float beta) :
    gr_sync_block ("slicer_fb",
                   gr_make_io_signature (1, 1, sizeof(float)),
                   gr_make_io_signature (1, 1, sizeof(unsigned char)))
{
    d_alpha = alpha;
    d_beta = beta;
    d_max = 0.0;
    d_hi = 0.0;
    d_avg = 0.0;
    d_lo = 0.0;
    d_min = 0.0;
}

// Tracks average, minimum, and peak, then converts input into one of:
//
// [0, 1, 2, 3]
unsigned char pager_slicer_fb::slice(float sample)
{
    unsigned char decision;

    // Update DC level and remove
    d_avg = d_avg*(1.0-d_alpha)+sample*d_alpha;
    sample -= d_avg;

    if (sample > 0) {
        if (sample > d_hi) {                // In max region
            d_max = d_max*(1.0-d_alpha) + sample*d_alpha;
            decision = 3;
        }
        else {
            d_max -= (d_max-d_avg)*d_beta;  // decay otherwise
            decision = 2;
        }
    }
    else {
        if (sample < d_lo) {                // In min region
            d_min = d_min*(1.0-d_alpha) + sample*d_alpha;
            decision = 0;
        }
        else {
            d_min -= (d_min-d_avg)*d_beta;  // decay otherwise
            decision = 1;
        }
    }

    d_hi = d_max*2.0/3.0;
    d_lo = d_min*2.0/3.0;

    //fprintf(stderr, "%f %d\n", sample, decision);
    return decision;
}

int pager_slicer_fb::work(int noutput_items,
                          gr_vector_const_void_star &input_items,
		                  gr_vector_void_star &output_items)
{
    float *iptr = (float *) input_items[0];
    unsigned char *optr = (unsigned char *) output_items[0];

    int size = noutput_items;

    for (int i = 0; i < size; i++)
        *optr++ = slice(*iptr++);

    return noutput_items;
}
