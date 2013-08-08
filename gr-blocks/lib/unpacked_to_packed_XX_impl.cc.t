/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2013 Free Software Foundation, Inc.
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

// @WARNING@

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "@NAME_IMPL@.h"
#include <gnuradio/io_signature.h>
#include <assert.h>

namespace gr {
  namespace blocks {

    static const unsigned int BITS_PER_TYPE = sizeof(@O_TYPE@) * 8;

    @NAME@::sptr
    @NAME@::make(unsigned int bits_per_chunk,
                 endianness_t endianness)
    {
      return gnuradio::get_initial_sptr
        (new @NAME_IMPL@(bits_per_chunk, endianness));
    }

    @NAME_IMPL@::@NAME_IMPL@(unsigned int bits_per_chunk,
                             endianness_t endianness)
    : block("@NAME@",
               io_signature::make(1, -1, sizeof(@I_TYPE@)),
               io_signature::make(1, -1, sizeof(@O_TYPE@))),
      d_bits_per_chunk(bits_per_chunk), d_endianness(endianness), d_index(0)
    {
      assert(bits_per_chunk <= BITS_PER_TYPE);
      assert(bits_per_chunk > 0);

      set_relative_rate(bits_per_chunk/(1.0 * BITS_PER_TYPE));
    }

    @NAME_IMPL@::~@NAME_IMPL@()
    {
    }

    void
    @NAME_IMPL@::forecast(int noutput_items,
                          gr_vector_int &ninput_items_required)
    {
      int input_required = (int)ceil((d_index+noutput_items * 1.0 * BITS_PER_TYPE)
                                     / d_bits_per_chunk);
      unsigned ninputs = ninput_items_required.size();
      for(unsigned int i = 0; i < ninputs; i++) {
        ninput_items_required[i] = input_required;
      }
    }

    unsigned int
    get_bit_be1(const @I_TYPE@ *in_vector, unsigned int bit_addr,
                unsigned int bits_per_chunk)
    {
      unsigned int byte_addr = (int)bit_addr/bits_per_chunk;
      @I_TYPE@ x = in_vector[byte_addr];
      unsigned int residue = bit_addr - byte_addr * bits_per_chunk;
      //printf("Bit addr %d  byte addr %d  residue %d  val  %d\n",bit_addr,byte_addr,residue,(x>>(bits_per_chunk-1-residue))&1);
      return (x >> (bits_per_chunk-1-residue)) & 1;
    }

    int
    @NAME_IMPL@::general_work(int noutput_items,
                              gr_vector_int &ninput_items,
                              gr_vector_const_void_star &input_items,
                              gr_vector_void_star &output_items)
    {
      unsigned int index_tmp = d_index;

      assert(input_items.size() == output_items.size());
      int nstreams = input_items.size();

      for(int m=0; m< nstreams; m++) {
        const @I_TYPE@ *in = (@I_TYPE@ *)input_items[m];
        @O_TYPE@ *out = (@O_TYPE@ *)output_items[m];
        index_tmp=d_index;

        // per stream processing

        //assert((ninput_items[m]-d_index)*d_bits_per_chunk >= noutput_items*BITS_PER_TYPE);

        switch(d_endianness) {

        case GR_MSB_FIRST:
          for(int i = 0; i < noutput_items; i++) {
            @O_TYPE@ tmp=0;
            for(unsigned int j = 0; j < BITS_PER_TYPE; j++) {
              tmp = (tmp<<1) | get_bit_be1(in, index_tmp, d_bits_per_chunk);
              index_tmp++;
            }
            out[i] = tmp;
          }
          break;

        case GR_LSB_FIRST:
          for(int i = 0; i < noutput_items; i++) {
            unsigned long tmp=0;
            for(unsigned int j = 0; j < BITS_PER_TYPE; j++) {
              tmp = (tmp>>1) | (get_bit_be1(in, index_tmp, d_bits_per_chunk) << (BITS_PER_TYPE-1));
              index_tmp++;
            }
            out[i] = tmp;
          }
          break;

        default:
          assert(0);
        }
      }

      d_index = index_tmp;
      consume_each((int)(d_index/d_bits_per_chunk));
      d_index = d_index%d_bits_per_chunk;

      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */
