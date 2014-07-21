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

#ifndef INCLUDED_DTV_ATSC_SYNC_IMPL_H
#define INCLUDED_DTV_ATSC_SYNC_IMPL_H

#include <gnuradio/dtv/atsc_consts.h>
#include <gnuradio/dtv/atsc_sync.h>
#include <gnuradio/filter/single_pole_iir.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>

namespace gr {
  namespace dtv {

    class atsc_sync_impl : public atsc_sync
    {
    private:
      static const double LOOP_FILTER_TAP = 0.0005;	// 0.0005 works
      static const double ADJUSTMENT_GAIN = 1.0e-5 / (10 * ATSC_DATA_SEGMENT_LENGTH);
      static const int	  SYMBOL_INDEX_OFFSET = 3;
      static const int	  MIN_SEG_LOCK_CORRELATION_VALUE = 5;
      static const int	  SSI_MIN = -16;
      static const int	  SSI_MAX =  15;

      gr::filter::single_pole_iir<float,float,float> d_loop; // ``VCO'' loop filter
      gr::filter::mmse_fir_interpolator_ff d_interp;

      unsigned long long d_next_input;
      double             d_rx_clock_to_symbol_freq;
      int                d_si;
      double             d_w;	// ratio of PERIOD of Tx to Rx clocks
      double             d_mu;	// fractional delay [0,1]
      int                d_incr;

      float		 sample_mem[ATSC_DATA_SEGMENT_LENGTH];
      float		 data_mem[ATSC_DATA_SEGMENT_LENGTH];

      double             d_timing_adjust;
      int                d_counter;	// free running mod 832 counter
      int                d_symbol_index;
      bool               d_seg_locked;
      int                d_sr;	// 4 bit shift register
      signed char        d_integrator[ATSC_DATA_SEGMENT_LENGTH];
      int		 output_produced;

    public:
      atsc_sync_impl(float rate);
      ~atsc_sync_impl();

      void reset();

      void forecast(int noutput_items,
                    gr_vector_int &ninput_items_required);

      virtual int general_work(int noutput_items,
                               gr_vector_int &ninput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items);
    };

  } /* namespace dtv */
} /* namespace gr */

#endif /* INCLUDED_DTV_ATSC_SYNC_IMPL_H */
