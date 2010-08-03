/*
 * Copyright 2004,2006,2007,2010 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Radio
 * 
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
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

pager_slicer_fb_sptr pager_make_slicer_fb(float alpha)
{
    return gnuradio::get_initial_sptr(new pager_slicer_fb(alpha));
}

pager_slicer_fb::pager_slicer_fb(float alpha) :
    gr_sync_block ("slicer_fb",
                   gr_make_io_signature (1, 1, sizeof(float)),
                   gr_make_io_signature (1, 1, sizeof(unsigned char)))
{
    d_alpha = alpha;
    d_beta = 1.0-alpha;
    d_avg = 0.0;
}

// Tracks average, minimum, and peak, then converts input into one of:
//
// [0, 1, 2, 3]
unsigned char pager_slicer_fb::slice(float sample)
{
    unsigned char decision;

    // Update DC level and remove
    d_avg = d_avg*d_beta+sample*d_alpha;
    sample -= d_avg;

    if (sample > 0) {
        if (sample > 2.0)          
            decision = 3;
        else
            decision = 2;
    }
    else {
        if (sample < -2.0)
            decision = 0;
        else
            decision = 1;
    }

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
