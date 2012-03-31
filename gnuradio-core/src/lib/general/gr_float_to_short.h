/* -*- c++ -*- */
/*
 * Copyright 2004,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_FLOAT_TO_SHORT_H
#define INCLUDED_GR_FLOAT_TO_SHORT_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_float_to_short;
typedef boost::shared_ptr<gr_float_to_short> gr_float_to_short_sptr;

GR_CORE_API gr_float_to_short_sptr
gr_make_float_to_short (size_t vlen=1, float scale=1);

/*!
 * \brief Convert stream of float to a stream of short
 * \ingroup converter_blk
 *
 * \param vlen vector length of data streams.
 * \param scale a scalar multiplier to change the output signal scale.
 */

class GR_CORE_API gr_float_to_short : public gr_sync_block
{
  friend GR_CORE_API
    gr_float_to_short_sptr gr_make_float_to_short (size_t vlen, float scale);
  gr_float_to_short (size_t vlen, float scale);

  size_t d_vlen;
  float d_scale;

 public:
  /*!
   * Get the scalar multiplier value.
   */
  float scale() const;

  /*!
   * Set the scalar multiplier value.
   */
  void set_scale(float scale);

  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_FLOAT_TO_SHORT_H */
