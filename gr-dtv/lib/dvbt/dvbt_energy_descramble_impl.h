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

#ifndef INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_IMPL_H
#define INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_IMPL_H

#include <gnuradio/dtv/dvbt_energy_descramble.h>

namespace gr {
  namespace dtv {

    class dvbt_energy_descramble_impl : public dvbt_energy_descramble
    {
     private:
      static const int d_nblocks;
      static const int d_bsize;
      static const int d_SYNC;
      static const int d_NSYNC;
      static const int d_MUX_PKT;

      // Register for PRBS
      int d_reg;

      // Index
      int d_index;
      // Search interval
      int d_search;

      void init_prbs();
      int clock_prbs(int clocks);

     public:
      dvbt_energy_descramble_impl(int nblocks);
      ~dvbt_energy_descramble_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_ENERGY_DESCRAMBLE_IMPL_H */

