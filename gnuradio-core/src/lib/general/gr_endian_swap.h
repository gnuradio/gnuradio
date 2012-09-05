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

#ifndef INCLUDED_GR_ENDIAN_SWAP_H
#define INCLUDED_GR_ENDIAN_SWAP_H

#include <gr_core_api.h>
#include <gr_sync_block.h>

class gr_endian_swap;
typedef boost::shared_ptr<gr_endian_swap> gr_endian_swap_sptr;

GR_CORE_API gr_endian_swap_sptr
gr_make_endian_swap (size_t item_size_bytes=1);

/*!
 * \brief Convert stream of items into thier byte swapped version
 *
 * \param item_size_bytes number of bytes per item, 1=no-op,2=uint16_t,4=uint32_t,8=uint64_t
 */

class GR_CORE_API gr_endian_swap : public gr_sync_block
{
 private:
  friend GR_CORE_API gr_endian_swap_sptr
    gr_make_endian_swap (size_t item_size_bytes);
  gr_endian_swap (size_t item_size_bytes);

  size_t item_size_bytes;

 public:
  virtual int work (int noutput_items,
		    gr_vector_const_void_star &input_items,
		    gr_vector_void_star &output_items);
};


#endif /* INCLUDED_GR_ENDIAN_SWAP_H */
