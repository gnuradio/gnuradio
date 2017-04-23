/* -*- c++ -*- */
/* 
 * Copyright 2017 Free Software Foundation, Inc.
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


#ifndef INCLUDED_BLOCKS_CONCATENATE_H
#define INCLUDED_BLOCKS_CONCATENATE_H

#include <gnuradio/blocks/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace blocks {

    /*!
     * \brief Concatenate full streams
     * \ingroup stream_operators_blk
     *
     * \details
     * This block takes the input from several sources and put all the items
     * after each other. All the items from the source connected to the
     * first input are produced first, then all the items from the second
     * source, and so on. The upstream blocks need to return -1 in the
     * general_work method to make this block aware of when they have
     * completed producing items.
     */
    class BLOCKS_API concatenate : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<concatenate> sptr;

      /*!
       * \param itemsize The size of the items at all inputs and outputs.
       * \param ninputs Number of input streams to concatenate
       */
      static sptr make(size_t itemsize, int ninputs);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_CONCATENATE_H */

