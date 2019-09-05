/* -*- c++ -*- */
/*
 * Copyright 2012-2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_IMPL_H
#define INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_IMPL_H

#include <gnuradio/blocks/plateau_detector_fb.h>

namespace gr {
namespace blocks {

class plateau_detector_fb_impl : public plateau_detector_fb
{
private:
    int d_max_len;
    float d_threshold;

public:
    plateau_detector_fb_impl(int max_len, float threshold);
    ~plateau_detector_fb_impl();

    void forecast(int noutput_items, gr_vector_int& ninput_items_required);

    int general_work(int noutput_items,
                     gr_vector_int& ninput_items,
                     gr_vector_const_void_star& input_items,
                     gr_vector_void_star& output_items);

    virtual void set_threshold(float threshold);
    virtual float threshold() const;
};

} // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_PLATEAU_DETECTOR_FB_IMPL_H */
