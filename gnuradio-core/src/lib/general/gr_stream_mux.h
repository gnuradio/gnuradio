/* -*- c++ -*- */
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

#ifndef INCLUDED_GR_STREAM_MUX_H
#define INCLUDED_GR_STREAM_MUX_H


#include <gr_core_api.h>
#include <gr_block.h>
#include <vector>

/*!
 * \brief Creates a stream muxing block to multiplex many streams into
 * one with a specified format.
 * \ingroup converter_blk
 *
 * \param itemsize the item size of the stream
 * \param length   a vector (list/tuple) specifying the number of
 *                 items from each stream the mux together.
 *                 Warning: this requires that at least as many items
 *                 per stream are available or the system will wait
 *                 indefinitely for the items.
 *
 */
class gr_stream_mux;
typedef boost::shared_ptr<gr_stream_mux> gr_stream_mux_sptr;



GR_CORE_API gr_stream_mux_sptr 
gr_make_stream_mux (size_t itemsize, const std::vector<int> &lengths);


/*!
 * \brief Stream muxing block to multiplex many streams into
 * one with a specified format.
 * 
 * Muxes N streams together producing an output stream that
 * contains N0 items from the first stream, N1 items from the second,
 * etc. and repeats:
 *
 * [N0, N1, N2, ..., Nm, N0, N1, ...]
 */

class GR_CORE_API gr_stream_mux : public gr_block
{
  friend GR_CORE_API gr_stream_mux_sptr
    gr_make_stream_mux (size_t itemsize, const std::vector<int> &lengths);
  
 protected:
   gr_stream_mux (size_t itemsize, const std::vector<int> &lengths);

 private:
  size_t d_itemsize;
  unsigned int d_stream;    // index of currently selected stream
  int d_residual;           // number if items left to put into current stream
  gr_vector_int d_lengths;  // number if items to pack per stream
 
  void increment_stream();

 public:
  ~gr_stream_mux(void);

 void forecast (int noutput_items, gr_vector_int &ninput_items_required);

  int general_work(int noutput_items,
		   gr_vector_int &ninput_items,
		   gr_vector_const_void_star &input_items,
		   gr_vector_void_star &output_items);

};


#endif
