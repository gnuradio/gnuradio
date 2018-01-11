/* -*- c++ -*- */
/*
 * Copyright (C) 2017 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "symbol_sync_ff_impl.h"
#include <boost/math/common_factor.hpp>
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <stdexcept>

namespace gr {
  namespace digital {

    symbol_sync_ff::sptr
    symbol_sync_ff::make(enum ted_type detector_type,
                         float sps,
                         float loop_bw,
                         float damping_factor,
                         float ted_gain,
                         float max_deviation,
                         int osps,
                         constellation_sptr slicer,
                         ir_type interp_type,
                         int n_filters,
                         const std::vector<float> &taps)
    {
      return gnuradio::get_initial_sptr
        (new symbol_sync_ff_impl(detector_type,
                                 sps,
                                 loop_bw,
                                 damping_factor,
                                 ted_gain,
                                 max_deviation,
                                 osps,
                                 slicer,
                                 interp_type,
                                 n_filters,
                                 taps));
    }

    symbol_sync_ff_impl::symbol_sync_ff_impl(
                           enum ted_type detector_type,
                           float sps,
                           float loop_bw,
                           float damping_factor,
                           float ted_gain,
                           float max_deviation,
                           int osps,
                           constellation_sptr slicer,
                           ir_type interp_type,
                           int n_filters,
                           const std::vector<float> &taps)
      : block("symbol_sync_ff",
              io_signature::make(1, 1, sizeof(float)),
              io_signature::makev(1, 4, std::vector<int>(4, sizeof(float)))),
        d_ted(NULL),
        d_interp(NULL),
        d_inst_output_period(sps / static_cast<float>(osps)),
        d_inst_clock_period(sps),
        d_avg_clock_period(sps),
        d_osps(static_cast<float>(osps)),
        d_osps_n(osps),
        d_tags(),
        d_new_tags(),
        d_time_est_key(pmt::intern("time_est")),
        d_clock_est_key(pmt::intern("clock_est")),
        d_noutputs(1),
        d_out_error(NULL),
        d_out_instantaneous_clock_period(NULL),
        d_out_average_clock_period(NULL)
    {
      // Brute force fix of the output io_signature, because I can't get
      // an anonymous std::vector<int>() rvalue, with a const expression
      // initializing the vector, to work.  Lvalues seem to make everything
      // better.
      int output_io_sizes[4] = {
          sizeof(float),
          sizeof(float), sizeof(float), sizeof(float)
      };
      std::vector<int> output_io_sizes_vector(&output_io_sizes[0],
                                              &output_io_sizes[4]);
      set_output_signature(io_signature::makev(1, 4, output_io_sizes_vector));

      if (sps <= 1.0f)
        throw std::out_of_range("nominal samples per symbol must be > 1");

      if (osps < 1)
        throw std::out_of_range("output samples per symbol must be > 0");

      // Timing Error Detector
      d_ted = timing_error_detector::make(detector_type, slicer);
      if (d_ted == NULL)
        throw std::runtime_error("unable to create timing_error_detector");

      // Interpolating Resampler
      d_interp = interpolating_resampler_fff::make(interp_type,
                                                   d_ted->needs_derivative(),
                                                   n_filters, taps);
      if (d_interp == NULL)
       throw std::runtime_error("unable to create interpolating_resampler_fff");

      // Block Internal Clocks
      d_interps_per_symbol_n = boost::math::lcm(d_ted->inputs_per_symbol(),
                                                d_osps_n);
      d_interps_per_ted_input_n =
                            d_interps_per_symbol_n / d_ted->inputs_per_symbol();
      d_interps_per_output_sample_n =
                            d_interps_per_symbol_n / d_osps_n;

      d_interps_per_symbol = static_cast<float>(d_interps_per_symbol_n);
      d_interps_per_ted_input = static_cast<float>(d_interps_per_ted_input_n);

      d_interp_clock = d_interps_per_symbol_n - 1;
      sync_reset_internal_clocks();
      d_inst_interp_period = d_inst_clock_period / d_interps_per_symbol;

      if (d_interps_per_symbol > sps)
          GR_LOG_WARN(d_logger,
                      boost::format("block performing more interopolations per "
                                    "symbol (%3f) than input samples per symbol"
                                    "(%3f). Consider reducing osps or "
                                    "increasing sps") % d_interps_per_symbol
                                                      % sps);

      // Symbol Clock Tracking and Estimation
      d_clock = new clock_tracking_loop(loop_bw,
                                        sps + max_deviation,
                                        sps - max_deviation,
                                        sps,
                                        damping_factor,
                                        ted_gain);

      // Timing Error Detector
      d_ted->sync_reset();

      // Interpolating Resampler
      d_interp->sync_reset(sps);

      // Tag Propagation and Clock Tracking Reset/Resync
      set_relative_rate (d_osps / sps);
      set_tag_propagation_policy(TPP_DONT);
      d_filter_delay = (d_interp->ntaps() + 1) / 2;

      set_output_multiple(d_osps_n);
    }

    symbol_sync_ff_impl::~symbol_sync_ff_impl()
    {
      delete d_ted;
      delete d_interp;
      delete d_clock;
    }

    //
    // Block Internal Clocks
    //
    void
    symbol_sync_ff_impl::update_internal_clock_outputs()
    {
          // a d_interp_clock boolean output would always be true.
          d_ted_input_clock = (d_interp_clock % d_interps_per_ted_input_n == 0);
          d_output_sample_clock =
                          (d_interp_clock % d_interps_per_output_sample_n == 0);
          d_symbol_clock = (d_interp_clock % d_interps_per_symbol_n == 0);
    }

    void
    symbol_sync_ff_impl::advance_internal_clocks()
    {
          d_interp_clock = (d_interp_clock + 1) % d_interps_per_symbol_n;
          update_internal_clock_outputs();
    }

    void
    symbol_sync_ff_impl::revert_internal_clocks()
    {
          if (d_interp_clock == 0)
              d_interp_clock = d_interps_per_symbol_n - 1;
          else
              d_interp_clock--;
          update_internal_clock_outputs();
    }

    void
    symbol_sync_ff_impl::sync_reset_internal_clocks()
    {
          d_interp_clock = d_interps_per_symbol_n - 1;
          update_internal_clock_outputs();
    }

    //
    // Tag Propagation and Clock Tracking Reset/Resync
    //
    void
    symbol_sync_ff_impl::collect_tags(uint64_t nitems_rd, int count)
    {
        // Get all the tags in offset order
        // d_new_tags is used to look for time_est and clock_est tags.
        // d_tags is used for manual tag propagation.
        d_new_tags.clear();
        get_tags_in_range(d_new_tags, 0, nitems_rd, nitems_rd + count);
        std::sort(d_new_tags.begin(), d_new_tags.end(), tag_t::offset_compare);
        d_tags.insert(d_tags.end(), d_new_tags.begin(), d_new_tags.end());
        std::sort(d_tags.begin(), d_tags.end(), tag_t::offset_compare);
    }

    bool
    symbol_sync_ff_impl::find_sync_tag(uint64_t nitems_rd, int iidx,
                                       int distance,
                                       uint64_t &tag_offset,
                                       float &timing_offset,
                                       float &clock_period)
    {
        bool found;
        uint64_t soffset, eoffset;
        std::vector<tag_t>::iterator t;
        std::vector<tag_t>::iterator t2;

        // PLL Reset/Resynchronization to time_est & clock_est tags
        //
        // Look for a time_est tag between the current interpolated input sample
        // and the next predicted interpolated input sample. (both rounded up)
        soffset = nitems_rd + d_filter_delay + static_cast<uint64_t>(iidx + 1);
        eoffset = soffset + distance;
        found = false;
        for (t = d_new_tags.begin();
             t != d_new_tags.end();
             t = d_new_tags.erase(t)) {

            if (t->offset > eoffset) // search finished
                break;

            if (t->offset < soffset) // tag is in the past of what we care about
                continue;

            if (!pmt::eq(t->key, d_time_est_key) &&  // not a time_est tag
                !pmt::eq(t->key, d_clock_est_key)   ) // not a clock_est tag
                continue;

            found = true;
            tag_offset = t->offset;
            if (pmt::eq(t->key, d_time_est_key)) {
                // got a time_est tag
                timing_offset = static_cast<float>(pmt::to_double(t->value));
                // next instantaneous clock period estimate will be nominal
                clock_period = d_clock->get_nom_avg_period();

                // Look for a clock_est tag at the same offset,
                // as we prefer clock_est tags
                for (t2 = ++t; t2 != d_new_tags.end(); ++t2) {
                    if (t2->offset > t->offset) // search finished
                        break;
                    if (!pmt::eq(t->key, d_clock_est_key)) // not a clock_est
                        continue;
                    // Found a clock_est tag at the same offset
                    tag_offset = t2->offset;
                    timing_offset = static_cast<float>(
                                  pmt::to_double(pmt::tuple_ref(t2->value, 0)));
                    clock_period = static_cast<float>(
                                  pmt::to_double(pmt::tuple_ref(t2->value, 1)));
                    break;
                }
            } else {
                // got a clock_est tag
                timing_offset = static_cast<float>(
                                   pmt::to_double(pmt::tuple_ref(t->value, 0)));
                clock_period = static_cast<float>(
                                   pmt::to_double(pmt::tuple_ref(t->value, 1)));
            }

            if (!(timing_offset >= -1.0f && timing_offset <= 1.0f)) {
                // the time_est/clock_est tag's payload is invalid
                GR_LOG_WARN(d_logger,
                            boost::format("ignoring time_est/clock_est tag with"
                                          " value %.2f, outside of allowed "
                                          "range [-1.0, 1.0]") % timing_offset);
                found = false;
                continue;
            }

            if (t->offset == soffset && timing_offset < 0.0f) {
                // already handled times earlier than this previously
                found = false;
                continue;
            }

            if (t->offset == eoffset && timing_offset >= 0.0f) {
                // handle times greater than this later
                found = false;
                break;
            }

            if (found == true)
                break;
        }
        return found;
    }

    void
    symbol_sync_ff_impl::propagate_tags(uint64_t nitems_rd, int iidx,
                                        float iidx_fraction,
                                        float inst_output_period,
                                        uint64_t nitems_wr, int oidx)
    {
        // Tag Propagation
        //
        // Onto this output sample, place all the remaining tags that
        // came before the interpolated input sample, and all the tags
        // on and after the interpolated input sample, up to half way to
        // the next output sample.

        uint64_t mid_period_offset = nitems_rd + d_filter_delay
                   + static_cast<uint64_t>(iidx)
                   + static_cast<uint64_t>(llroundf(iidx_fraction
                                                    + inst_output_period/2.0f));

        uint64_t output_offset = nitems_wr + static_cast<uint64_t>(oidx);

        int i;
        std::vector<tag_t>::iterator t;
        for (t = d_tags.begin();
             t != d_tags.end() && t->offset <= mid_period_offset;
             t = d_tags.erase(t)) {
            t->offset = output_offset;
            for (i = 0; i < d_noutputs; i++)
                add_item_tag(i, *t);
        }
    }

    void
    symbol_sync_ff_impl::save_expiring_tags(uint64_t nitems_rd,
                                            int consumed)
    {
        // Deferred Tag Propagation
        //
        // Only save away input tags that will not be available
        // in the next call to general_work().  Otherwise we would
        // create duplicate tags next time around.
        // Tags that have already been propagated, have already been erased
        // from d_tags.

        uint64_t consumed_offset = nitems_rd + static_cast<uint64_t>(consumed);
        std::vector<tag_t>::iterator t;

        for (t = d_tags.begin(); t != d_tags.end(); ) {
            if (t->offset < consumed_offset)
                ++t;
            else
                t = d_tags.erase(t);
        }
    }

    //
    // Optional Diagnostic Outputs
    //
    void
    symbol_sync_ff_impl::setup_optional_outputs(
                                              gr_vector_void_star &output_items)
    {
        d_noutputs = output_items.size();
        d_out_error = NULL;
        d_out_instantaneous_clock_period = NULL;
        d_out_average_clock_period = NULL;

        if (d_noutputs < 2)
            return;
        d_out_error = (float *) output_items[1];

        if (d_noutputs < 3)
            return;
        d_out_instantaneous_clock_period = (float *) output_items[2];

        if (d_noutputs < 4)
            return;
        d_out_average_clock_period = (float *) output_items[3];
    }

    void
    symbol_sync_ff_impl::emit_optional_output(int oidx,
                                              float error,
                                              float inst_clock_period,
                                              float avg_clock_period)
    {
        if (d_noutputs < 2)
            return;
        d_out_error[oidx] = error;

        if (d_noutputs < 3)
            return;
        d_out_instantaneous_clock_period[oidx] = inst_clock_period;

        if (d_noutputs < 4)
            return;
        d_out_average_clock_period[oidx] = avg_clock_period;
    }

    void
    symbol_sync_ff_impl::forecast(int noutput_items,
                                  gr_vector_int &ninput_items_required)
    {
        unsigned ninputs = ninput_items_required.size();

        // The '+ 2' in the expression below is an effort to always have at
        // least one output sample, even if the main loop decides it has to
        // revert one computed sample and wait for the next call to
        // general_work().
        // The d_clock->get_max_avg_period() is also an effort to do the same,
        // in case we have the worst case allowable clock timing deviation on
        // input.
        int answer = static_cast<int>(
                                ceilf(static_cast<float>(noutput_items + 2)
                                      * d_clock->get_max_avg_period() / d_osps))
                     + static_cast<int>(d_interp->ntaps());

        for(unsigned i = 0; i < ninputs; i++)
            ninput_items_required[i] = answer;
    }

    int
    symbol_sync_ff_impl::general_work(int noutput_items,
                                      gr_vector_int &ninput_items,
                                      gr_vector_const_void_star &input_items,
                                      gr_vector_void_star &output_items)
    {
      // max input to consume
      int ni = ninput_items[0] - static_cast<int>(d_interp->ntaps());
      if (ni <= 0)
          return 0;

      const float *in = (const float *)input_items[0];
      float *out = (float *)output_items[0];

      setup_optional_outputs(output_items);

      int ii = 0; // input index
      int oo = 0; // output index
      float interp_output;
      float interp_derivative = 0.0f;
      float error;
      float look_ahead_phase = 0.0f;
      int look_ahead_phase_n = 0;
      float look_ahead_phase_wrapped = 0.0f;

      uint64_t nitems_rd = nitems_read(0);
      uint64_t nitems_wr = nitems_written(0);
      uint64_t sync_tag_offset;
      float sync_timing_offset;
      float sync_clock_period;

      // Tag Propagation and Symbol Clock Tracking Reset/Resync
      collect_tags(nitems_rd, ni);

      while (oo < noutput_items) {
        // Block Internal Clocks
        advance_internal_clocks();

        // Symbol Clock and Interpolator Positioning & Alignment
        interp_output = d_interp->interpolate(&in[ii],
                                              d_interp->phase_wrapped());
        if (output_sample_clock())
            out[oo] = interp_output;

        // Timing Error Detector
        if (ted_input_clock()) {
            if (d_ted->needs_derivative())
                interp_derivative =
                    d_interp->differentiate(&in[ii], d_interp->phase_wrapped());
            d_ted->input(interp_output, interp_derivative);
        }
        if (symbol_clock() && d_ted->needs_lookahead()) {
            // N.B. symbol_clock() == true implies ted_input_clock() == true
            // N.B. symbol_clock() == true implies output_sample_clock() == true

            // We must interpolate ahead to the *next* ted_input_clock, so the
            // ted will compute the error for *this* symbol.
            d_interp->next_phase(d_interps_per_ted_input
                                 * (d_clock->get_inst_period()
                                    / d_interps_per_symbol),
                                 look_ahead_phase,
                                 look_ahead_phase_n,
                                 look_ahead_phase_wrapped);

            if (ii + look_ahead_phase_n >= ni) {
                // We can't compute the look ahead interpolated output
                // because there's not enough input.
                // Revert the actions we took in the loop so far, and bail out.

                // Symbol Clock Tracking and Estimation
                // We haven't advanced the clock loop yet; no revert needed.

                // Timing Error Detector
                d_ted->revert(true); // keep the error value; it's still good

                // Symbol Clock and Interpolator Positioning & Alignment
                // Nothing to do

                // Block Internal Clocks
                revert_internal_clocks();
                break;
            }
            // Give the ted the look ahead input that it needs to compute
            // the error for *this* symbol.
            interp_output = d_interp->interpolate(&in[ii + look_ahead_phase_n],
                                                  look_ahead_phase_wrapped);
            if (d_ted->needs_derivative())
                interp_derivative =
                           d_interp->differentiate(&in[ii + look_ahead_phase_n],
                                                   look_ahead_phase_wrapped);
            d_ted->input_lookahead(interp_output, interp_derivative);
        }
        error = d_ted->error();

        if (symbol_clock()) {
            // Symbol Clock Tracking and Estimation
            d_clock->advance_loop(error);
            d_inst_clock_period = d_clock->get_inst_period();
            d_avg_clock_period = d_clock->get_avg_period();
            d_clock->phase_wrap();
            d_clock->period_limit();

            // Symbol Clock and Interpolator Positioning & Alignment
            d_inst_interp_period = d_inst_clock_period / d_interps_per_symbol;

            // Tag Propagation
            d_inst_output_period = d_inst_clock_period / d_osps;
        }

        // Symbol Clock, Interpolator Positioning & Alignment, and 
        // Tag Propagation
        if (symbol_clock()) {
            // N.B. symbol_clock() == true implies ted_input_clock() == true
            // N.B. symbol_clock() == true implies output_sample_clock() == true

            // This check and revert is needed either when
            // a) the samples per symbol to get to the next symbol is greater
            // than d_interp->ntaps() (normally 8); thus we would consume()
            // more input than we were given to get there.
            // b) we can't get to the next output so we can't do tag
            // propagation properly.
            d_interp->next_phase(d_inst_clock_period,
                                 look_ahead_phase,
                                 look_ahead_phase_n,
                                 look_ahead_phase_wrapped);

            if (ii + look_ahead_phase_n >= ni) {
                // We can't move forward because there's not enough input.
                // Revert the actions we took in the loop so far, and bail out.

                // Symbol Clock Tracking and Estimation
                d_clock->revert_loop();

                // Timing Error Detector
                d_ted->revert();

                // Symbol Clock and Interpolator Positioning & Alignment
                // Nothing to do;

                // Block Internal Clocks
                revert_internal_clocks();
                break;
            }
        }

        // Symbol Clock and Interpolator Positioning & Alignment
        d_interp->advance_phase(d_inst_interp_period);

        // Symbol Clock Tracking Reset/Resync to time_est and clock_est tags
        if (find_sync_tag(nitems_rd, ii, d_interp->phase_n(), sync_tag_offset,
                          sync_timing_offset, sync_clock_period) == true   ) {

            // Block Internal Clocks
            sync_reset_internal_clocks();

            // Timing Error Detector
            d_ted->sync_reset();
            error = d_ted->error();

            // Symbol Clock Tracking and Estimation

            // NOTE: the + 1 below was determined empirically, but doesn't
            // seem right on paper (maybe rounding in the computation of
            // d_filter_delay is the culprit).  Anyway, experiment trumps
            // theory *every* time; so + 1 it is.
            d_inst_clock_period = static_cast<float>(
                  static_cast<int>(sync_tag_offset - nitems_rd - d_filter_delay)
                  - ii + 1) + sync_timing_offset - d_interp->phase_wrapped();

            d_clock->set_inst_period(d_inst_clock_period);
            d_clock->set_avg_period(sync_clock_period);
            d_avg_clock_period = d_clock->get_avg_period();
            d_clock->set_phase(0.0f);

            // Symbol Clock and Interpolator Positioning & Alignment
            d_inst_interp_period = d_inst_clock_period;
            d_interp->revert_phase();
            d_interp->advance_phase(d_inst_interp_period);

            // Tag Propagation
            // Only needed if we reverted back to an output_sample_clock()
            // as this is only used for tag propagation.  Note that the
            // next output will also be both an output_sample_clock() and a
            // symbol_clock().
            d_inst_output_period = d_inst_clock_period;
        }

        if (output_sample_clock()) {
            // Diagnostic Output of Symbol Clock Tracking cycle results
            emit_optional_output(oo, error, d_inst_clock_period,
                                                            d_avg_clock_period);
            // Tag Propagation
            propagate_tags(nitems_rd, ii,
                           d_interp->prev_phase_wrapped(), d_inst_output_period,
                           nitems_wr, oo);

            // Symbol Clock and Interpolator Positioning & Alignment
            oo++;
        }

        // Symbol Clock and Interpolator Positioning & Alignment
        ii += d_interp->phase_n();
      }

      // Deferred Tag Propagation
      save_expiring_tags(nitems_rd, ii);

      // Symbol Clock and Interpolator Positioning & Alignment
      consume_each(ii);
      return oo;
    }

  } /* namespace digital */
} /* namespace gr */
