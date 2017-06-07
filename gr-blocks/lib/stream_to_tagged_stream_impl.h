/* -*- c++ -*- */
/*
 * Copyright 2013 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_IMPL_H
#define INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_IMPL_H

#include <gnuradio/blocks/stream_to_tagged_stream.h>

namespace gr {
  namespace blocks {

    class stream_to_tagged_stream_impl : public stream_to_tagged_stream
    {
     private:
      size_t d_itemsize;
      unsigned d_packet_len;
      pmt::pmt_t d_packet_len_pmt;
      pmt::pmt_t d_len_tag_key;
      uint64_t d_next_tag_pos;

     public:
      stream_to_tagged_stream_impl(size_t itemsize, int vlen, unsigned packet_len, const std::string &tag_len_key);
      ~stream_to_tagged_stream_impl();
      void set_packet_len(unsigned packet_len);
      void set_packet_len_pmt(unsigned packet_len);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      bool start();
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_STREAM_TO_TAGGED_STREAM_IMPL_H */

