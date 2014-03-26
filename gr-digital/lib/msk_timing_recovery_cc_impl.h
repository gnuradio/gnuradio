/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_IMPL_H
#define INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_IMPL_H

#include <gnuradio/digital/msk_timing_recovery_cc.h>
#include <gnuradio/filter/mmse_fir_interpolator_cc.h>
#include <boost/circular_buffer.hpp>
#include <gnuradio/filter/fir_filter_with_buffer.h>

namespace gr {
  namespace digital {

    class msk_timing_recovery_cc_impl : public msk_timing_recovery_cc
    {
     private:
        float d_sps;
        float d_gain;
        float d_limit;
        filter::mmse_fir_interpolator_cc *d_interp;
        filter::kernel::fir_filter_with_buffer_fff *d_decim;
        gr_complex d_dly_conj_1, d_dly_conj_2, d_dly_diff_1;
        float d_mu, d_omega, d_gain_omega;
        int d_div;
        int d_osps;
        int d_loop_rate;

     public:
      msk_timing_recovery_cc_impl(float sps, float gain, float limit, int osps);
      ~msk_timing_recovery_cc_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);
      void set_gain(float gain);
      float get_gain(void);

      void set_limit(float limit);
      float get_limit(void);

      void set_sps(float sps);
      float get_sps(void);
    };
  } // namespace digital
} // namespace gr

#endif /* INCLUDED_DIGITAL_MSK_TIMING_RECOVERY_CC_IMPL_H */

