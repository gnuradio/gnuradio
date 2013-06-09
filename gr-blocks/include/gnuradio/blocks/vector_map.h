/* -*- c++ -*- */
/*
 * Copyright 2012,2013 Free Software Foundation, Inc.
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

#include <gnuradio/blocks/api.h>
#include <vector>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Maps elements from a set of input vectors to a set of output vectors.
     * \ingroup stream_operators_blk
     *
     * \details
     * If in[i] is the input vector in the i'th stream then the output
     * vector in the j'th stream is:
     *
     * out[j][k] = in[mapping[j][k][0]][mapping[j][k][1]]
     *
     * That is mapping is of the form (out_stream1_mapping,
     * out_stream2_mapping, ...)  and out_stream1_mapping is of the
     * form (element1_mapping, element2_mapping, ...)  and
     * element1_mapping is of the form (in_stream, in_element).
     */
    class BLOCKS_API vector_map : virtual public sync_block
    {
    public:
      // gr::blocks::vector_map::sptr
      typedef boost::shared_ptr<vector_map> sptr;

      /*!
       * Build a vector map block.
       *
       * \param item_size (integer) size of vector elements
       * \param in_vlens (vector of integers) number of elements in each
       *                 input vector
       * \param mapping (vector of vectors of vectors of integers) how to
       *                map elements from input to output vectors
       */
      static sptr make(size_t item_size, std::vector<size_t> in_vlens,
                       std::vector< std::vector< std::vector<size_t> > > mapping);

      virtual void set_mapping(std::vector< std::vector< std::vector<size_t> > > mapping) = 0;
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_VECTOR_MAP_H */
