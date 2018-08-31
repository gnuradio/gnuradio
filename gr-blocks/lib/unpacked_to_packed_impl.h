/* -*- c++ -*- */
/*
 * Copyright 2006.2013,2018 Free Software Foundation, Inc.
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


#ifndef UNPACKED_TO_PACKED_IMPL_H
#define UNPACKED_TO_PACKED_IMPL_H

#include <gnuradio/blocks/unpacked_to_packed.h>

namespace gr {
  namespace blocks {

template<class T>
    class unpacked_to_packed_impl : public  unpacked_to_packed<T>
    {
    private:
      unsigned int    d_bits_per_chunk;
      endianness_t d_endianness;
      unsigned int    d_index;
      const unsigned int d_bits_per_type = sizeof(T) * 8;
      unsigned int  get_bit_be1(const T *in_vector, unsigned int bit_addr, unsigned int bits_per_chunk);

    public:
      unpacked_to_packed_impl (unsigned int bits_per_chunk,
                  endianness_t endianness);
      ~unpacked_to_packed_impl ();

      void forecast(int noutput_items,
                    gr_vector_int &ninput_items_required);
      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);

      bool check_topology(int ninputs, int noutputs)
      { return ninputs == noutputs; }
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* UNPACKED_TO_PACKED_IMPL_H */
