/*
 * Copyright 2006,2007 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PAGER_SLICER_FB_H
#define INCLUDED_PAGER_SLICER_FB_H

#include <pager_api.h>
#include <gr_sync_block.h>

class pager_slicer_fb;
typedef boost::shared_ptr<pager_slicer_fb> pager_slicer_fb_sptr;

PAGER_API pager_slicer_fb_sptr pager_make_slicer_fb(float alpha);

/*!
 * \brief slicer description
 * \ingroup pager_blk
 */
class PAGER_API pager_slicer_fb : public gr_sync_block
{
private:
    friend PAGER_API pager_slicer_fb_sptr pager_make_slicer_fb(float alpha);
    pager_slicer_fb(float alpha);

    unsigned char slice(float sample);

    float d_alpha;      // DC removal time constant
    float d_beta;	// 1.0-d_alpha
    float d_avg;        // Average value for DC offset subtraction

public:
    int work (int noutput_items,
              gr_vector_const_void_star &input_items, 
              gr_vector_void_star &output_items);

    float dc_offset() const { return d_avg; }
};

#endif /* INCLUDED_PAGER_SLICER_FB_H */
