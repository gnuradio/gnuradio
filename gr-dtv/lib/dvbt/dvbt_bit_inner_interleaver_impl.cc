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
#include "dvbt_bit_inner_interleaver_impl.h"

#define MAX_MODULATION_ORDER 6
#define INTERLEAVER_BLOCK_SIZE 126

namespace gr {
  namespace dtv {

    const int dvbt_bit_inner_interleaver_impl::d_bsize = INTERLEAVER_BLOCK_SIZE;


    dvbt_bit_inner_interleaver::sptr
    dvbt_bit_inner_interleaver::make(int nsize, \
        dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvbt_transmission_mode_t transmission)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_bit_inner_interleaver_impl(nsize, constellation, hierarchy, transmission));
    }

    /*
     * The private constructor
     */
    dvbt_bit_inner_interleaver_impl::dvbt_bit_inner_interleaver_impl(int nsize, dvb_constellation_t constellation, \
        dvbt_hierarchy_t hierarchy, dvbt_transmission_mode_t transmission)
      : block("dvbt_bit_inner_interleaver",
          io_signature::make(1, 2, sizeof(unsigned char) * nsize),
          io_signature::make(1, 1, sizeof (unsigned char) * nsize)),
      config(constellation, hierarchy, gr::dtv::C1_2, gr::dtv::C1_2, gr::dtv::GI_1_32, transmission),
      d_nsize(nsize),
      d_hierarchy(hierarchy)
    {
      d_v = config.d_m;
      d_hierarchy = config.d_hierarchy;

      d_perm = (unsigned char *)new (std::nothrow) unsigned char[d_v * d_bsize];
      if (d_perm == NULL) {
        GR_LOG_FATAL(d_logger, "Bit Inner Interleaver, cannot allocate memory for d_perm.");
        throw std::bad_alloc();
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
        GR_LOG_ERROR(d_logger, boost::format("Input size must be multiple of block size: nsize: %1% bsize: %2%") \
                     % d_nsize % d_bsize);
      }
    }

    /*
     * Our virtual destructor.
     */
    dvbt_bit_inner_interleaver_impl::~dvbt_bit_inner_interleaver_impl()
    {
      delete [] d_perm;
    }

    void
    dvbt_bit_inner_interleaver_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for (unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = noutput_items;
    }

    int
    dvbt_bit_inner_interleaver_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *inh = (const unsigned char *) input_items[0];
      const unsigned char *inl = (const unsigned char *) input_items[1];
      unsigned char *out = (unsigned char *) output_items[0];

      int bmax = noutput_items * d_nsize / d_bsize;

      // First index of d_b is Bit interleaver number
      // Second index of d_b is the position inside the Bit interleaver
      unsigned char d_b[MAX_MODULATION_ORDER][INTERLEAVER_BLOCK_SIZE];

      for (int bcount = 0; bcount < bmax; bcount++) {
        for (int i = 0; i < d_bsize; i++) {
          if (d_hierarchy == NH) {
            int c = inh[bcount * d_bsize + i];

            // Create the demultiplexer
            for (int k = 0; k < d_v; k++) {
              d_b[d_perm[(d_v * i) + k]][i] = (c >> (d_v - k - 1)) & 1;
            }
          }
          else {
            int ch = inh[(bcount * d_bsize) + i];
            int cl = inl[(bcount * d_bsize) + i];

            // High priority input - first 2 streams
            for (int k = 0; k < 2; k++) {
              d_b[(d_v * i + k) % 2][(d_v * i + k) / 2] = (ch >> (1 - k)) & 1;
            }

            // Low priority input - (v - 2) streams
            for (int k = 2; k < (d_v - 2); k++) {
              d_b[d_perm[d_v * i + k]][(d_v * i + k) / (d_v - 2)] = (cl >> (d_v - k - 1)) & 1;
            }
          }
        }

        // Take one bit from each interleaver
        // and format the output

        for (int w = 0; w < d_bsize; w++) {
          int val = 0;

          for (int e = 0; e < d_v; e++) {
            val = (val << 1) | d_b[e][d_lookup_H[w][e]];
          }

          out[(bcount * d_bsize) + w] = val;
        }
      }

      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

    const int dvbt_bit_inner_interleaver_impl::d_lookup_H[INTERLEAVER_BLOCK_SIZE][MAX_MODULATION_ORDER] =
    {
      {0, 63, 105, 42, 21, 84},
      {1, 64, 106, 43, 22, 85},
      {2, 65, 107, 44, 23, 86},
      {3, 66, 108, 45, 24, 87},
      {4, 67, 109, 46, 25, 88},
      {5, 68, 110, 47, 26, 89},
      {6, 69, 111, 48, 27, 90},
      {7, 70, 112, 49, 28, 91},
      {8, 71, 113, 50, 29, 92},
      {9, 72, 114, 51, 30, 93},
      {10, 73, 115, 52, 31, 94},
      {11, 74, 116, 53, 32, 95},
      {12, 75, 117, 54, 33, 96},
      {13, 76, 118, 55, 34, 97},
      {14, 77, 119, 56, 35, 98},
      {15, 78, 120, 57, 36, 99},
      {16, 79, 121, 58, 37, 100},
      {17, 80, 122, 59, 38, 101},
      {18, 81, 123, 60, 39, 102},
      {19, 82, 124, 61, 40, 103},
      {20, 83, 125, 62, 41, 104},
      {21, 84, 0, 63, 42, 105},
      {22, 85, 1, 64, 43, 106},
      {23, 86, 2, 65, 44, 107},
      {24, 87, 3, 66, 45, 108},
      {25, 88, 4, 67, 46, 109},
      {26, 89, 5, 68, 47, 110},
      {27, 90, 6, 69, 48, 111},
      {28, 91, 7, 70, 49, 112},
      {29, 92, 8, 71, 50, 113},
      {30, 93, 9, 72, 51, 114},
      {31, 94, 10, 73, 52, 115},
      {32, 95, 11, 74, 53, 116},
      {33, 96, 12, 75, 54, 117},
      {34, 97, 13, 76, 55, 118},
      {35, 98, 14, 77, 56, 119},
      {36, 99, 15, 78, 57, 120},
      {37, 100, 16, 79, 58, 121},
      {38, 101, 17, 80, 59, 122},
      {39, 102, 18, 81, 60, 123},
      {40, 103, 19, 82, 61, 124},
      {41, 104, 20, 83, 62, 125},
      {42, 105, 21, 84, 63, 0},
      {43, 106, 22, 85, 64, 1},
      {44, 107, 23, 86, 65, 2},
      {45, 108, 24, 87, 66, 3},
      {46, 109, 25, 88, 67, 4},
      {47, 110, 26, 89, 68, 5},
      {48, 111, 27, 90, 69, 6},
      {49, 112, 28, 91, 70, 7},
      {50, 113, 29, 92, 71, 8},
      {51, 114, 30, 93, 72, 9},
      {52, 115, 31, 94, 73, 10},
      {53, 116, 32, 95, 74, 11},
      {54, 117, 33, 96, 75, 12},
      {55, 118, 34, 97, 76, 13},
      {56, 119, 35, 98, 77, 14},
      {57, 120, 36, 99, 78, 15},
      {58, 121, 37, 100, 79, 16},
      {59, 122, 38, 101, 80, 17},
      {60, 123, 39, 102, 81, 18},
      {61, 124, 40, 103, 82, 19},
      {62, 125, 41, 104, 83, 20},
      {63, 0, 42, 105, 84, 21},
      {64, 1, 43, 106, 85, 22},
      {65, 2, 44, 107, 86, 23},
      {66, 3, 45, 108, 87, 24},
      {67, 4, 46, 109, 88, 25},
      {68, 5, 47, 110, 89, 26},
      {69, 6, 48, 111, 90, 27},
      {70, 7, 49, 112, 91, 28},
      {71, 8, 50, 113, 92, 29},
      {72, 9, 51, 114, 93, 30},
      {73, 10, 52, 115, 94, 31},
      {74, 11, 53, 116, 95, 32},
      {75, 12, 54, 117, 96, 33},
      {76, 13, 55, 118, 97, 34},
      {77, 14, 56, 119, 98, 35},
      {78, 15, 57, 120, 99, 36},
      {79, 16, 58, 121, 100, 37},
      {80, 17, 59, 122, 101, 38},
      {81, 18, 60, 123, 102, 39},
      {82, 19, 61, 124, 103, 40},
      {83, 20, 62, 125, 104, 41},
      {84, 21, 63, 0, 105, 42},
      {85, 22, 64, 1, 106, 43},
      {86, 23, 65, 2, 107, 44},
      {87, 24, 66, 3, 108, 45},
      {88, 25, 67, 4, 109, 46},
      {89, 26, 68, 5, 110, 47},
      {90, 27, 69, 6, 111, 48},
      {91, 28, 70, 7, 112, 49},
      {92, 29, 71, 8, 113, 50},
      {93, 30, 72, 9, 114, 51},
      {94, 31, 73, 10, 115, 52},
      {95, 32, 74, 11, 116, 53},
      {96, 33, 75, 12, 117, 54},
      {97, 34, 76, 13, 118, 55},
      {98, 35, 77, 14, 119, 56},
      {99, 36, 78, 15, 120, 57},
      {100, 37, 79, 16, 121, 58},
      {101, 38, 80, 17, 122, 59},
      {102, 39, 81, 18, 123, 60},
      {103, 40, 82, 19, 124, 61},
      {104, 41, 83, 20, 125, 62},
      {105, 42, 84, 21, 0, 63},
      {106, 43, 85, 22, 1, 64},
      {107, 44, 86, 23, 2, 65},
      {108, 45, 87, 24, 3, 66},
      {109, 46, 88, 25, 4, 67},
      {110, 47, 89, 26, 5, 68},
      {111, 48, 90, 27, 6, 69},
      {112, 49, 91, 28, 7, 70},
      {113, 50, 92, 29, 8, 71},
      {114, 51, 93, 30, 9, 72},
      {115, 52, 94, 31, 10, 73},
      {116, 53, 95, 32, 11, 74},
      {117, 54, 96, 33, 12, 75},
      {118, 55, 97, 34, 13, 76},
      {119, 56, 98, 35, 14, 77},
      {120, 57, 99, 36, 15, 78},
      {121, 58, 100, 37, 16, 79},
      {122, 59, 101, 38, 17, 80},
      {123, 60, 102, 39, 18, 81},
      {124, 61, 103, 40, 19, 82},
      {125, 62, 104, 41, 20, 83}
    };

  } /* namespace dtv */
} /* namespace gr */

