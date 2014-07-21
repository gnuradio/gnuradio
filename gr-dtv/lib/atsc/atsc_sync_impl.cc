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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "atsc_sync_impl.h"
#include "atsc_types.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace dtv {

    atsc_sync::sptr
    atsc_sync::make(float rate)
    {
      return gnuradio::get_initial_sptr
	(new atsc_sync_impl(rate));
    }

    atsc_sync_impl::atsc_sync_impl(float rate)
      : gr::block("dtv_atsc_sync",
                  io_signature::make(1, 1, sizeof(float)),
                  io_signature::make(1, 1, sizeof(atsc_soft_data_segment))),
	d_next_input(0), d_rx_clock_to_symbol_freq(rate/ATSC_SYMBOL_RATE),
	d_si(0)
    {
      d_loop.set_taps(LOOP_FILTER_TAP);
      reset();
    }

    void
    atsc_sync_impl::reset()
    {
      d_w = d_rx_clock_to_symbol_freq;
      d_mu = 0.5;

      for (int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++)
        sample_mem[i] = 0;

      d_timing_adjust = 0;
      d_counter = 0;
      d_symbol_index = 0;
      d_seg_locked = false;

      d_sr = 0;

      for (int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++)
        d_integrator[i] = SSI_MIN;
    }

    atsc_sync_impl::~atsc_sync_impl()
    {
    }

    void
    atsc_sync_impl::forecast(int noutput_items,
                             gr_vector_int &ninput_items_required)
    {
      unsigned ninputs = ninput_items_required.size();
      for (unsigned i = 0; i < ninputs; i++)
        ninput_items_required[i] = static_cast<int>(noutput_items * d_rx_clock_to_symbol_freq * ATSC_DATA_SEGMENT_LENGTH) + 1500 - 1;
    }

    int
    atsc_sync_impl::general_work(int noutput_items,
                                 gr_vector_int &ninput_items,
                                 gr_vector_const_void_star &input_items,
                                 gr_vector_void_star &output_items)
    {
	const float *in = (const float *) input_items[0];
	atsc_soft_data_segment *soft_data_segment_out = (atsc_soft_data_segment *) output_items[0];

	float interp_sample;

	// amount actually consumed
	d_si = 0;

	for (output_produced = 0; output_produced < noutput_items && (d_si + (int)d_interp.ntaps()) < ninput_items[0];) {
          // First we interpolate a sample from input to work with
          interp_sample = d_interp.interpolate(&in[d_si], d_mu);

          // Apply our timing adjustment slowly over several samples
          d_mu += ADJUSTMENT_GAIN * 1e3 * d_timing_adjust;

          double s = d_mu + d_w;
          double float_incr = floor(s);
          d_mu = s - float_incr;
          d_incr = (int)float_incr;

          assert (d_incr >= 1 && d_incr <= 3);
          d_si += d_incr;

          // Remember the sample at this count position
          sample_mem[d_counter] = interp_sample;

          // Is the sample positive or negative?
          int bit = (interp_sample < 0 ? 0 : 1);

          // Put the sign bit into our shift register
          d_sr = ((bit & 1) << 3) | (d_sr >> 1);

          // When +,-,-,+ (0x9, 1001) samples show up we have likely found a segment
          // sync, it is more likely the segment sync will show up at about the same
          // spot every ATSC_DATA_SEGMENT_LENGTH samples so we add some weight
          // to this spot every pass to prevent random +,-,-,+ symbols from
          // confusing our synchronizer
          d_integrator[d_counter] += ((d_sr == 0x9) ? +2 : -1);
          if(d_integrator[d_counter] < SSI_MIN) d_integrator[d_counter] = SSI_MIN;
          if(d_integrator[d_counter] > SSI_MAX) d_integrator[d_counter] = SSI_MAX;

          d_symbol_index++;
          if( d_symbol_index >= ATSC_DATA_SEGMENT_LENGTH )
            d_symbol_index = 0;

          d_counter++;
          if( d_counter >= ATSC_DATA_SEGMENT_LENGTH ) {	// counter just wrapped...
            int best_correlation_value = d_integrator[0];
            int best_correlation_index = 0;

            for(int i = 1; i < ATSC_DATA_SEGMENT_LENGTH; i++)
              if (d_integrator[i] > best_correlation_value) {
                best_correlation_value = d_integrator[i];
                best_correlation_index = i;
              }

            d_seg_locked = best_correlation_value >= MIN_SEG_LOCK_CORRELATION_VALUE;

            // the coefficients are -1,-1,+1,+1
            //d_timing_adjust = sample_mem[best_correlation_index - 3] +
            //                   sample_mem[best_correlation_index - 2] -
            //                   sample_mem[best_correlation_index - 1] -
            //                   sample_mem[best_correlation_index];

            //printf( "d_timing_adjust = %f\n", d_timing_adjust );

            int corr_count = best_correlation_index;

            d_timing_adjust = -sample_mem[corr_count--];
            if( corr_count < 0 ) corr_count = ATSC_DATA_SEGMENT_LENGTH - 1;
            d_timing_adjust -= sample_mem[corr_count--];
            if( corr_count < 0 ) corr_count = ATSC_DATA_SEGMENT_LENGTH - 1;
            d_timing_adjust += sample_mem[corr_count--];
            if( corr_count < 0 ) corr_count = ATSC_DATA_SEGMENT_LENGTH - 1;
            d_timing_adjust += sample_mem[corr_count--];

            d_symbol_index = SYMBOL_INDEX_OFFSET - 1 - best_correlation_index;
            if (d_symbol_index < 0)
              d_symbol_index += ATSC_DATA_SEGMENT_LENGTH;

            d_counter = 0;
          }

          // If we are locked we can start filling and producing data packets
          // Due to the way we lock the first data packet will almost always be
          // half full, this is OK becouse the fs_checker will not let packets though
          // untill a non-corrupted field packet is found
          if( d_seg_locked ) {
            data_mem[d_symbol_index] = interp_sample;

            if( d_symbol_index >= (ATSC_DATA_SEGMENT_LENGTH - 1) )
              {
                for( int i = 0; i < ATSC_DATA_SEGMENT_LENGTH; i++ )
                  soft_data_segment_out[output_produced].data[i] = data_mem[i];
                output_produced++;
              }
          }
	}

	consume_each(d_si);
	return output_produced;

    }

  } /* namespace dtv */
} /* namespace gr */
