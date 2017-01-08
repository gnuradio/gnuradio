/* -*- c++ -*- */
/*
 * Copyright 2004,2011,2012,2014 Free Software Foundation, Inc.
 * Copyright (C) 2016-2017  Andy Walls <awalls.cx18@gmail.com>
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

#ifndef INCLUDED_DIGITAL_SYMBOL_SYNC_MM_FF_IMPL_H
#define INCLUDED_DIGITAL_SYMBOL_SYNC_MM_FF_IMPL_H

#include <gnuradio/digital/symbol_sync_mm_ff.h>
#include <gnuradio/digital/clock_tracking_loop.h>
#include <gnuradio/filter/mmse_fir_interpolator_ff.h>

namespace gr {
  namespace digital {

    class symbol_sync_mm_ff_impl : public symbol_sync_mm_ff
    {
    public:
      symbol_sync_mm_ff_impl(float sps,
                             float loop_bw,
                             float damping_factor,
                             float max_deviation,
                             int osps);
      ~symbol_sync_mm_ff_impl();

      void forecast(int noutput_items, gr_vector_int &ninput_items_required);
      int general_work(int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items);

      // Symbol Clock Tracking and Estimation
      float loop_bandwidth() const { return d_clock->get_loop_bandwidth(); }
      float damping_factor() const { return d_clock->get_damping_factor(); }
      float alpha() const { return d_clock->get_alpha(); }
      float beta() const { return d_clock->get_beta(); }

      void set_loop_bandwidth (float fn_norm) {
          d_clock->set_loop_bandwidth(fn_norm);
      }
      void set_damping_factor (float zeta) {
          d_clock->set_damping_factor(zeta);
      }
      void set_alpha (float alpha) { d_clock->set_alpha(alpha); }
      void set_beta (float beta) { d_clock->set_beta(beta); }

    private:
      // Timing Error Detector
      float d_error;
      float d_prev_error;
      float d_prev_y;
      float d_prev_decision;
      float d_prev2_y;
      float d_prev2_decision;
      int   d_ted_inputs_per_symbol_n;

      // Symbol Clock Tracking and Estimation
      clock_tracking_loop *d_clock;

      // Interpolator and Interpolator Positioning and Alignment
      filter::mmse_fir_interpolator_ff *d_interp;
      float d_interp_phase;
      float d_interp_phase_wrapped;
      int   d_interp_phase_n;
      float d_prev_interp_phase;
      float d_prev_interp_phase_wrapped;
      int   d_prev_interp_phase_n;

      // Block Internal Clocks
      // 4 clocks that run synchronously, aligned to the Symbol Clock:
      //     Interpolator Clock (always highest rate)
      //     Timing Error Detector Input Clock
      //     Output Sample Clock
      //     Symbol Clock (always lowest rate)
      int   d_interp_clock;
      float d_inst_interp_period;

      float d_interps_per_ted_input;
      int   d_interps_per_ted_input_n;
      bool  d_ted_input_clock;

      int   d_interps_per_output_sample_n;
      bool  d_output_sample_clock;
      float d_inst_output_period;

      float d_interps_per_symbol;
      int   d_interps_per_symbol_n;
      bool  d_symbol_clock;
      float d_inst_clock_period;
      float d_avg_clock_period;

      // Block output
      float d_osps;
      int   d_osps_n;

      // Tag Propagation and Symbol Clock Tracking Reset/Resync
      uint64_t d_filter_delay; // interpolator filter delay
      std::vector<tag_t> d_tags;
      std::vector<tag_t> d_new_tags;
      pmt::pmt_t d_time_est_key;
      pmt::pmt_t d_clock_est_key;

      // Optional Diagnostic Outputs
      int d_noutputs;
      float *d_out_error;
      float *d_out_instantaneous_clock_period;
      float *d_out_average_clock_period;

      // Timing Error Detector
      float slice(float x);
      float timing_error_detector(float curr_y);
      void revert_timing_error_detector_state();
      void sync_reset_timing_error_detector();

      // Symbol Clock and Interpolator Positioning and Alignment
      void next_interpolator_phase(float increment,
                                   float &phase,
                                   int   &phase_n,
                                   float &phase_wrapped);
      void advance_interpolator_phase(float increment);
      void revert_interpolator_phase();

      // Block Internal Clocks
      void update_internal_clock_outputs();
      void advance_internal_clocks();
      void revert_internal_clocks();
      void sync_reset_internal_clocks();
      bool ted_input_clock() { return d_ted_input_clock; }
      bool output_sample_clock() { return d_output_sample_clock; }
      bool symbol_clock() { return d_symbol_clock; }

      // Tag Propagation and Clock Tracking Reset/Resync
      void collect_tags(uint64_t nitems_rd, int count);
      bool find_sync_tag(uint64_t nitems_rd, int iidx,
                         int distance,
                         uint64_t &tag_offset,
                         float &timing_offset,
                         float &clock_period);
      void propagate_tags(uint64_t nitems_rd, int iidx,
                          float iidx_fraction,
                          float inst_output_period,
                          uint64_t nitems_wr, int oidx);
      void save_expiring_tags(uint64_t nitems_rd, int consumed);

      // Optional Diagnostic Outputs
      void setup_optional_outputs(gr_vector_void_star &output_items);
      void emit_optional_output(int oidx,
                                float error,
                                float inst_clock_period,
                                float avg_clock_period);
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_SYMBOL_SYNC_MM_FF_IMPL_H */
