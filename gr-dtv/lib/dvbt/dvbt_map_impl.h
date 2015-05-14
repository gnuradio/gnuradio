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

#ifndef INCLUDED_DTV_DVBT_MAP_IMPL_H
#define INCLUDED_DTV_DVBT_MAP_IMPL_H

#include <gnuradio/dtv/dvbt_map.h>
#include "dvbt_configure.h"

namespace gr {
  namespace dtv {

    class dvbt_map_impl : public dvbt_map
    {
     private:
      const dvbt_configure config;

      int d_nsize;

      //Constellation size
      unsigned char d_constellation_size;
      // Keeps transmission mode
      dvbt_transmission_mode_t d_transmission_mode;
      //Step on each axis of the constellation
      unsigned char d_step;
      //Keep Alpha internally
      unsigned char d_alpha;
      //Gain for the complex values
      float d_gain;

      gr_complex * d_constellation_points;

      void make_constellation_points(int size, int step, int alpha);
      gr_complex find_constellation_point(int val);

      //Return gray representation from natural binary
      unsigned int bin_to_gray(unsigned int val);

     public:
      dvbt_map_impl(int nsize, dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, dvbt_transmission_mode_t transmission, float gain);
      ~dvbt_map_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_MAP_IMPL_H */

