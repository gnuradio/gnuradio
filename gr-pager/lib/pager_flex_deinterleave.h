/*
 * Copyright 2006 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PAGER_FLEX_DEINTERLEAVE_H
#define INCLUDED_PAGER_FLEX_DEINTERLEAVE_H

#include <pager_api.h>
#include <gr_sync_decimator.h>

class pager_flex_deinterleave;
typedef boost::shared_ptr<pager_flex_deinterleave> pager_flex_deinterleave_sptr;

PAGER_API pager_flex_deinterleave_sptr pager_make_flex_deinterleave();

/*!
 * \brief flex deinterleave description
 * \ingroup pager_blk
 */

class PAGER_API pager_flex_deinterleave : public gr_sync_decimator
{
private:
    // Constructors
    friend PAGER_API pager_flex_deinterleave_sptr pager_make_flex_deinterleave();
    pager_flex_deinterleave();

    // One FLEX block of deinterleaved data
    gr_int32 d_codewords[8];
   
public:

    int work(int noutput_items,
             gr_vector_const_void_star &input_items, 
             gr_vector_void_star &output_items);
};

#endif /* INCLUDED_PAGER_FLEX_DEINTERLEAVE_H */
