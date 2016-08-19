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
#include "dvbt_demap_impl.h"
#include <volk/volk.h>

namespace gr {
  namespace dtv {

    dvbt_demap::sptr
    dvbt_demap::make(int nsize, dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, \
        dvbt_transmission_mode_t transmission, float gain)
    {
      return gnuradio::get_initial_sptr
        (new dvbt_demap_impl(nsize, constellation, hierarchy, transmission, gain));
    }

    /*
     * The private constructor
     */
    dvbt_demap_impl::dvbt_demap_impl(int nsize, dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, \
        dvbt_transmission_mode_t transmission, float gain)
      : block("dvbt_demap",
          io_signature::make(1, 1, sizeof (gr_complex) * nsize),
          io_signature::make(1, 1, sizeof (unsigned char) * nsize)),
      config(constellation, hierarchy, gr::dtv::C1_2, gr::dtv::C1_2, gr::dtv::GI_1_32, transmission),
      d_nsize(nsize),
      d_constellation_size(0),
      d_step(0),
      d_alpha(0),
      d_gain(0.0)
    {
      //Get parameters from config object
      d_constellation_size = config.d_constellation_size;
      d_transmission_mode = config.d_transmission_mode;
      d_step = config.d_step;
      d_alpha = config.d_alpha;
      d_gain = gain * config.d_norm;

      d_constellation_points = (gr_complex*) volk_malloc(sizeof(gr_complex) * d_constellation_size, volk_get_alignment());
      if (d_constellation_points == NULL) {
        GR_LOG_FATAL(d_logger, "DVB-T Demap, cannot allocate memory for d_constellation_points.");
        throw std::bad_alloc();
      }

      d_sq_dist = (float*) volk_malloc(sizeof(float) * d_constellation_size, volk_get_alignment());
      if (d_sq_dist == NULL) {
        GR_LOG_FATAL(d_logger, "DVB-T Demap, cannot allocate memory for d_sq_dist.");
        volk_free(d_constellation_points);
        throw std::bad_alloc();
      }

      make_constellation_points(d_constellation_size, d_step, d_alpha);
    }

    /*
     * Our virtual destructor.
     */
    dvbt_demap_impl::~dvbt_demap_impl()
    {
      volk_free(d_sq_dist);
      volk_free(d_constellation_points);
    }

    void
    dvbt_demap_impl::make_constellation_points(int size, int step, int alpha)
    {
      // The symmetry of the constellation is used to calculate
      // 16QAM from QPSK and 64QAM from 16QAM

      int bits_per_axis = log2(size) / 2;
      int steps_per_axis = sqrt(size) / 2 - 1;

      for (int i = 0; i < size; i++) {
        // This is the quadrant made of the first two bits starting from MSB
        int q = i >> (2 * (bits_per_axis - 1)) & 3;
        // Sign for correct calculation of I and Q in each quadrant
        int sign0 = (q >> 1) ? -1 : 1; int sign1 = (q & 1) ? -1 : 1;

        int x = (i >> (bits_per_axis - 1)) & ((1 << (bits_per_axis - 1)) - 1);
        int y = i & ((1 << (bits_per_axis - 1)) - 1);

        int xval = alpha + (steps_per_axis - x) * step;
        int yval = alpha + (steps_per_axis - y) * step;

        int val = (bin_to_gray(x) << (bits_per_axis - 1)) + bin_to_gray(y);

        // ETSI EN 300 744 Clause 4.3.5
        // Actually the constellation is gray coded
        // but the bits on each axis are not taken in consecutive order
        // So we need to convert from b0b2b4b1b3b5->b0b1b2b3b4b5(64QAM)

        x = 0; y = 0;

        for (int j = 0; j < (bits_per_axis - 1); j++) {
          x += ((val >> (1 + 2 * j)) & 1) << j;
          y += ((val >> (2 * j)) & 1) << j;
        }

        val = (q << 2 * (bits_per_axis - 1)) + (x << (bits_per_axis - 1)) + y;

        // Keep corresponding symbol bits->complex symbol in one vector
        // Normalize the signal using gain
        d_constellation_points[val] = d_gain * gr_complex(sign0 * xval, sign1 * yval);
      }
    }

    int
    dvbt_demap_impl::find_constellation_value(gr_complex val)
    {
      float min_dist = std::norm(val - d_constellation_points[0]);
      int min_index = 0;

      volk_32fc_x2_square_dist_32f(&d_sq_dist[0], &val, &d_constellation_points[0], d_constellation_size);

      for (int i = 0; i < d_constellation_size; i++) {
        if (d_sq_dist[i] < min_dist) {
          min_dist = d_sq_dist[i];
          min_index = i;
        }
      }

      //return d_constellation_bits[min_index];
      return min_index;
    }

    int
    dvbt_demap_impl::bin_to_gray(int val)
    {
      return (val >> 1) ^ val;
    }

    void
    dvbt_demap_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    dvbt_demap_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];

      // TODO - use DFE (Decision Feedback Equalizer)

      for (int i = 0; i < (noutput_items * d_nsize); i++) {
        out[i] = find_constellation_value(in[i]);
      }

      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace dtv */
} /* namespace gr */

