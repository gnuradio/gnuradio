/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_GR_VECTOR_MAP_H
#define INCLUDED_GR_VECTOR_MAP_H

#include <vector>
#include <gr_core_api.h>
#include <gr_sync_interpolator.h>
#include <gruel/thread.h>

class gr_vector_map;
typedef boost::shared_ptr<gr_vector_map> gr_vector_map_sptr;

GR_CORE_API gr_vector_map_sptr
gr_make_vector_map (size_t item_size, std::vector<size_t> in_vlens,
		    std::vector< std::vector< std::vector<size_t> > > mapping);

/*!
 * \brief Maps elements from a set of input vectors to a set of output vectors.
 *
 * If in[i] is the input vector in the i'th stream then the output
 * vector in the j'th stream is:
 *
 * out[j][k] = in[mapping[j][k][0]][mapping[j][k][1]]
 *
 * That is mapping is of the form (out_stream1_mapping,
 * out_stream2_mapping, ...)  and out_stream1_mapping is of the form
 * (element1_mapping, element2_mapping, ...)  and element1_mapping is
 * of the form (in_stream, in_element).
 *
 * \param item_size (integer) size of vector elements
 *
 * \param in_vlens (vector of integers) number of elements in each
 *                 input vector
 *
 * \param mapping (vector of vectors of vectors of integers) how to
 *                map elements from input to output vectors
 *
 * \ingroup slicedice_blk
 */
class GR_CORE_API gr_vector_map : public gr_sync_block
{
  friend GR_CORE_API gr_vector_map_sptr
    gr_make_vector_map(size_t item_size, std::vector<size_t> in_vlens,
		       std::vector< std::vector< std::vector<size_t> > > mapping);
  size_t d_item_size;
  std::vector<size_t> d_in_vlens;
  std::vector< std::vector< std::vector<size_t> > > d_mapping;
  gruel::mutex d_mutex; // mutex to protect set/work access

 protected:
  gr_vector_map(size_t item_size, std::vector<size_t> in_vlens,
		std::vector< std::vector< std::vector<size_t> > > mapping);

 public:
  int work(int noutput_items,
	   gr_vector_const_void_star &input_items,
	   gr_vector_void_star &output_items);

  void set_mapping(std::vector< std::vector< std::vector<size_t> > > mapping);
};

#endif /* INCLUDED_GR_VECTOR_MAP_H */
