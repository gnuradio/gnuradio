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

#ifndef INCLUDED_BLOCKS_REPACK_BITS_BB_IMPL_H
#define INCLUDED_BLOCKS_REPACK_BITS_BB_IMPL_H

#include <gnuradio/blocks/repack_bits_bb.h>

namespace gr {
  namespace blocks {

    class repack_bits_bb_impl : public repack_bits_bb
    {
     private:
      int d_k; //! Bits on input stream
      int d_l; //! Bits on output stream
      const bool d_packet_mode;
      int d_in_index; // Current bit of input byte
      int d_out_index; // Current bit of output byte
      bool d_align_output; //! true if the output shall be aligned, false if the input shall be aligned
      endianness_t d_endianness;

     protected:
      int calculate_output_stream_length(const gr_vector_int &ninput_items);

     public:
      repack_bits_bb_impl(int k, int l, const std::string &len_tag_key,
                          bool align_output, endianness_t endianness=GR_LSB_FIRST);
      ~repack_bits_bb_impl();
      void set_k_and_l(int k, int l);//callback function for bits per input byte k and bits per output byte l
      int work(int noutput_items,
	       gr_vector_int &ninput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_REPACK_BITS_BB_IMPL_H */
