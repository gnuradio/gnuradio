/* -*- c++ -*- */
/* 
 * Copyright 2015,2016 Free Software Foundation, Inc.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "dvbt_symbol_inner_interleaver_impl.h"

namespace gr {
  namespace dtv {

    const char dvbt_symbol_inner_interleaver_impl::d_bit_perm_2k[] = {4, 3, 9, 6, 2, 8, 1, 5, 7, 0};
    const char dvbt_symbol_inner_interleaver_impl::d_bit_perm_8k[] = {7, 1, 4, 2, 9, 6, 8, 10, 0, 3, 11, 5};

    void
    dvbt_symbol_inner_interleaver_impl::generate_H()
    {
      const int Mmax = d_fft_length;
      const int Nmax = d_payload_length;
      const int Nr = int(ceil(log2(d_fft_length)));
      int q = 0;

      for (int i = 0; i < Mmax; i++) {
        d_h[q] = ((i % 2) << (Nr - 1)) + calculate_R(i);
        if (d_h[q] < Nmax) {
          q++;
        }
      }
    }

    int
    dvbt_symbol_inner_interleaver_impl::H(int q)
    {
      return d_h[q];
    }

    int
    dvbt_symbol_inner_interleaver_impl::calculate_R(int i)
    {
      const int Nr = int(ceil(log2(d_fft_length)));
      int reg = 0;

      if (i == 0) {
        reg = 0;
      }
      else if (i == 1) {
        reg = 0;
      }
      else {
        reg = 1;
        for (int k = 3; k <= i; k++) {
          char new_bit = 0;

          if (d_transmission_mode == T2k) {
            new_bit = (reg ^ (reg >> 3)) & 1;
          }
          else if (d_transmission_mode == T8k) {
            new_bit = (reg ^ (reg >> 1) ^ (reg >> 4) ^ (reg >> 6)) & 1;
          }
          else {
            new_bit = (reg ^ (reg >> 3)) & 1;
          }

          int mask = (1 << Nr) - 1;
          reg = ((reg >> 1) | (new_bit << (Nr - 2))) & mask;
        }
      }

      int newreg = 0;

      for (int k = 0; k < (Nr - 1); k++) {
        char bit = (reg >> k) & 1;
        newreg = newreg | (bit << d_bit_perm[k]);
      }

      return newreg;
    }

    dvbt_symbol_inner_interleaver::sptr
    dvbt_symbol_inner_interleaver::make(int nsize, dvbt_transmission_mode_t transmission, int direction)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_symbol_inner_interleaver_impl(nsize, transmission, direction));
    }

    /*
     * The private constructor
     */
    dvbt_symbol_inner_interleaver_impl::dvbt_symbol_inner_interleaver_impl(int nsize, dvbt_transmission_mode_t transmission, int direction)
      : block("dvbt_symbol_inner_interleaver",
                          io_signature::make(1, 1, sizeof(unsigned char) * nsize),
                          io_signature::make(1, 1, sizeof(unsigned char) * nsize)),
      config(gr::dtv::MOD_16QAM, gr::dtv::NH, gr::dtv::C1_2, gr::dtv::C1_2, gr::dtv::GI_1_32, transmission),
      d_nsize(nsize), d_direction(direction),
      d_fft_length(0), d_payload_length(0),
      d_symbol_index(0)
    {
      d_symbols_per_frame = config.d_symbols_per_frame;
      d_transmission_mode = config.d_transmission_mode;
      d_fft_length = config.d_fft_length;
      d_payload_length = config.d_payload_length;
      d_direction = direction;

      // Verify if transmission mode matches with size of block
      assert(d_payload_length == d_nsize);

      // Allocate memory for h vector
      d_h = new (std::nothrow) int[d_fft_length];
      if (d_h == NULL) {
        GR_LOG_FATAL(d_logger, "Symbol Inner Interleaver, cannot allocate memory for d_h.");
        throw std::bad_alloc();
      }

      // Setup bit permutation vectors
      if (d_transmission_mode == T2k) {
        d_bit_perm = d_bit_perm_2k;
      }
      else if (d_transmission_mode == T8k) {
        d_bit_perm = d_bit_perm_8k;
      }
      else {
        d_bit_perm = d_bit_perm_2k;
      }

      // Generate the h function
      generate_H();
    }

    /*
     * Our virtual destructor.
     */
    dvbt_symbol_inner_interleaver_impl::~dvbt_symbol_inner_interleaver_impl()
    {
      delete [] d_h;
    }

    void
    dvbt_symbol_inner_interleaver_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    dvbt_symbol_inner_interleaver_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      // Demod reference signals sends a tag per OFDM frame
      // containing the symbol index.
      std::vector<tag_t> tags;
      const uint64_t nread = this->nitems_read(0); //number of items read on port 0

      // Read all tags on the input buffer
      this->get_tags_in_range(tags, 0, nread, nread + noutput_items, pmt::string_to_symbol("symbol_index"));

      for (int k = 0; k < noutput_items; k++) {
        int blocks = k * d_nsize;

        if (d_direction) {
          // Interleave
          for (int q = 0; q < d_nsize; q++) {
            if (d_symbol_index % 2) {
              out[blocks + q] = in[blocks + H(q)];
            }
            else {
              out[blocks + H(q)] = in[blocks + q];
            }
          }

          ++d_symbol_index;
          d_symbol_index = d_symbol_index % d_symbols_per_frame;
        }
        else {
          // Deinterleave
          d_symbol_index = pmt::to_long(tags[k].value);

          for (int q = 0; q < d_nsize; q++) {
            if (d_symbol_index % 2) {
              out[blocks + H(q)] = in[blocks + q];
            }
            else {
              out[blocks + q] = in[blocks + H(q)];
            }
          }
        }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

