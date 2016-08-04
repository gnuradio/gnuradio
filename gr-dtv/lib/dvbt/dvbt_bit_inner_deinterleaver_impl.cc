/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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
#include "dvbt_bit_inner_deinterleaver_impl.h"
#include <stdio.h>

#define MAX_MODULATION_ORDER 6
#define INTERLEAVER_BLOCK_SIZE 126

namespace gr {
  namespace dtv {

    const int dvbt_bit_inner_deinterleaver_impl::d_bsize = INTERLEAVER_BLOCK_SIZE;

    int
    dvbt_bit_inner_deinterleaver_impl::H(int e, int w)
    {
      int rez = 0;

      switch (e) {
        case 0:
          rez = w;
          break;
        case 1:
          rez = (w + 63) % d_bsize;
          break;
        case 2:
          rez = (w + 105) % d_bsize;
          break;
        case 3:
          rez = (w + 42) % d_bsize;
          break;
        case 4:
          rez = (w + 21) % d_bsize;
          break;
        case 5:
          rez = (w + 84) % d_bsize;
          break;
        default:
          break;
      }

      return rez;
    }

    dvbt_bit_inner_deinterleaver::sptr
    dvbt_bit_inner_deinterleaver::make(int nsize, \
        dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvbt_transmission_mode_t transmission)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_bit_inner_deinterleaver_impl(nsize, constellation, hierarchy, transmission));
    }

    /*
     * The private constructor
     */
    dvbt_bit_inner_deinterleaver_impl::dvbt_bit_inner_deinterleaver_impl(int nsize, dvb_constellation_t constellation, \
        dvbt_hierarchy_t hierarchy, dvbt_transmission_mode_t transmission)
      : block("dvbt_bit_inner_deinterleaver",
          io_signature::make(1, 1, sizeof (unsigned char) * nsize),
          io_signature::make(1, 2, sizeof (unsigned char) * nsize)),
      config(constellation, hierarchy, gr::dtv::C1_2, gr::dtv::C1_2, gr::dtv::GI_1_32, transmission),
      d_nsize(nsize),
      d_hierarchy(hierarchy)
    {
      d_v = config.d_m;
      d_hierarchy = config.d_hierarchy;

      d_perm = (unsigned char *)new unsigned char[d_v * d_bsize];
      if (d_perm == NULL) {
        std::cout << "Cannot allocate memory for d_perm" << std::endl;
        exit(1);
      }

      //Init permutation table (used for b[e][do])
      for (int i = 0; i <  d_bsize * d_v; i++) {
        if (d_hierarchy == NH) {
          d_perm[i] = ((i % d_v) / (d_v / 2)) + 2 * (i % (d_v / 2));
        }
        else {
          d_perm[i] = (i % (d_v - 2)) / ((d_v - 2) / 2) + 2 * (i % ((d_v - 2) / 2)) + 2;
        }
      }

      if (d_nsize % d_bsize) {
        std::cout << "Error: Input size must be multiple of block size: " \
          << "nsize: " << d_nsize << "bsize: " << d_bsize << std::endl;
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbt_bit_inner_deinterleaver_impl::~dvbt_bit_inner_deinterleaver_impl()
    {
      delete [] d_perm;
    }

    void
    dvbt_bit_inner_deinterleaver_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    dvbt_bit_inner_deinterleaver_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *outh = (unsigned char *) output_items[0];
      unsigned char *outl = (unsigned char *) output_items[1];

      int bmax = noutput_items * d_nsize / d_bsize;

      // First index of d_b is Bit interleaver number
      // Second index of d_b is the position inside Bit interleaver
      unsigned char d_b[MAX_MODULATION_ORDER][INTERLEAVER_BLOCK_SIZE];

      for (int bcount = 0; bcount < bmax; bcount++) {
        for (int w = 0; w < d_bsize; w++) {
          int c = in[(bcount * d_bsize) + w];

          for (int e = 0; e < d_v; e++) {
            d_b[e][H(e, w)] = (c >> (d_v - e - 1)) & 1;
          }
        }

        for (int i = 0; i < d_bsize; i++) {
          if (d_hierarchy == NH) {
            // Create output from demultiplexer
            int c = 0;

            for (int k = 0; k < d_v; k++) {
              c = (c << 1) | d_b[d_perm[(d_v * i) + k]][i];
            }

            outh[(bcount * d_bsize) + i] = c;
          }
          else {
            int c = 0;

            // High priority output - first 2 streams
            for (int k = 0; k < 2; k++) {
              c = (c << 1) | d_b[(d_v * i + k) % 2][(d_v * i + k) / 2];
            }

            outh[(bcount * d_bsize) + i] = c;

            c = 0;
            // Low priority output - (v - 2) streams
            for (int k = 2; k < (d_v - 2); k++) {
              c = (c << 1) | d_b[d_perm[d_v * i + k]][(d_v * i + k) / (d_v - 2)];
            }

            outl[(bcount * d_bsize) + i] = c;
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

