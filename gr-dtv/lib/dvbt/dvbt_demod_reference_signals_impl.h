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

#ifndef INCLUDED_DTV_DVBT_DEMOD_REFERENCE_SIGNALS_IMPL_H
#define INCLUDED_DTV_DVBT_DEMOD_REFERENCE_SIGNALS_IMPL_H

#include <gnuradio/dtv/dvbt_demod_reference_signals.h>
#include "dvbt_reference_signals_impl.h"

namespace gr {
  namespace dtv {

    class dvbt_demod_reference_signals_impl : public dvbt_demod_reference_signals
    {
      // configuration object for this class
      const dvbt_configure config;

     private:
      // Pilot Generator object
      dvbt_pilot_gen d_pg;

      //In and Out data length
      int d_ninput;
      int d_noutput;

      int d_init;
      int d_fi_start;

      int is_sync_start(int nitems);

     public:
      dvbt_demod_reference_signals_impl(int itemsize, int ninput, int noutput, \
        dvb_constellation_t constellation, dvbt_hierarchy_t hierarchy, \
        dvb_code_rate_t code_rate_HP, dvb_code_rate_t code_rate_LP, \
        dvb_guardinterval_t guard_interval, \
        dvbt_transmission_mode_t transmission_mode = gr::dtv::T2k, int include_cell_id = 0, int cell_id = 0);
      ~dvbt_demod_reference_signals_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
    };

  } // namespace dtv
} // namespace gr

#endif /* INCLUDED_DTV_DVBT_DEMOD_REFERENCE_SIGNALS_IMPL_H */

