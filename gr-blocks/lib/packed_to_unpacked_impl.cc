/* -*- c++ -*- */
/*
 * Copyright 2004,2013,2018 Free Software Foundation, Inc.
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "packed_to_unpacked_impl.h"
#include <gnuradio/io_signature.h>
#include <assert.h>

namespace gr {
  namespace blocks {

    template <class T>
    typename packed_to_unpacked<T>::sptr
    packed_to_unpacked<T>::make(unsigned int bits_per_chunk,
                 endianness_t endianness)
    {
      return gnuradio::get_initial_sptr
        (new packed_to_unpacked_impl<T> (bits_per_chunk, endianness));
    }

    template <class T>
    packed_to_unpacked_impl<T> ::packed_to_unpacked_impl(unsigned int bits_per_chunk,
                             endianness_t endianness)
    : block("packed_to_unpacked",
               io_signature::make(1, -1, sizeof(T)),
               io_signature::make(1, -1, sizeof(T))),
      d_bits_per_chunk(bits_per_chunk), d_endianness(endianness), d_index(0)
    {
      assert(bits_per_chunk <= this->d_bits_per_type);
      assert(bits_per_chunk > 0);

      this->set_relative_rate((uint64_t)this->d_bits_per_type, (uint64_t)bits_per_chunk);
    }

    template <class T>
    packed_to_unpacked_impl<T> ::~packed_to_unpacked_impl()
    {
    }

    template <class T>
    void
    packed_to_unpacked_impl<T> ::forecast(int noutput_items,
                          gr_vector_int &ninput_items_required)
    {
      int input_required = (int)ceil((d_index + noutput_items * d_bits_per_chunk)
                                     / (1.0 * this->d_bits_per_type));
      unsigned ninputs = ninput_items_required.size();
      for(unsigned int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = input_required;
        //printf("Forecast wants %d needs %d\n",noutput_items,ninput_items_required[i]);
      }
    }

    template <class T>
    unsigned int
    packed_to_unpacked_impl<T>::get_bit_le(const T *in_vector, unsigned int bit_addr)
    {
      T x = in_vector[bit_addr >> this->d_log2_l_type];
      return (x >> (bit_addr & (this->d_bits_per_type-1))) & 1;
    }

    template <class T>
    unsigned int
    packed_to_unpacked_impl<T>::get_bit_be(const T *in_vector, unsigned int bit_addr)
    {
      T x = in_vector[bit_addr >> this->d_log2_l_type];
      return (x >> ((this->d_bits_per_type-1) - (bit_addr & (this->d_bits_per_type-1)))) & 1;
    }

    template <class T>
    int
    packed_to_unpacked_impl<T> ::general_work(int noutput_items,
                              gr_vector_int &ninput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      unsigned int index_tmp = d_index;

      assert(input_items.size() == output_items.size());
      int nstreams = input_items.size();

      for (int m=0; m < nstreams; m++){
        const T *in = (T *)input_items[m];
        T *out = (T *)output_items[m];
        index_tmp = d_index;

        // per stream processing

        switch(d_endianness) {
        case GR_MSB_FIRST:
          for(int i = 0; i < noutput_items; i++) {
            //printf("here msb %d\n",i);
            T x = 0;
            for(unsigned int j = 0; j < d_bits_per_chunk; j++, index_tmp++)
              x = (x<<1) | get_bit_be(in, index_tmp);
            out[i] = x;
          }
          break;

        case GR_LSB_FIRST:
          for(int i = 0; i < noutput_items; i++) {
            //printf("here lsb %d\n",i);
            T x = 0;
            for(unsigned int j = 0; j < d_bits_per_chunk; j++, index_tmp++)
              x = (x<<1) | get_bit_le(in, index_tmp);
            out[i] = x;
          }
          break;

        default:
          assert(0);
        }

        //printf("almost got to end\n");
        assert(ninput_items[m] >= (int)((d_index+(this->d_bits_per_type-1)) >> this->d_log2_l_type));
      }

      d_index = index_tmp;
      this->consume_each(d_index >> this->d_log2_l_type);
      d_index = d_index & (this->d_bits_per_type-1);
      //printf("got to end\n");
      return noutput_items;
    }

template class packed_to_unpacked<std::uint8_t>;
template class packed_to_unpacked<std::int16_t>;
template class packed_to_unpacked<std::int32_t>;
  } /* namespace blocks */
} /* namespace gr */
