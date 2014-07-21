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

#include "atsc_equalizer_impl.h"
#include "atsc_types.h"
#include "atsc_pnXXX_impl.h"
#include <gnuradio/io_signature.h>

namespace gr {
  namespace dtv {

    atsc_equalizer::sptr
    atsc_equalizer::make()
    {
      return gnuradio::get_initial_sptr
	(new atsc_equalizer_impl());
    }

    static float
    bin_map (int bit)
    {
      return bit ? +5 : -5;
    }

    static void
    init_field_sync_common (float *p, int mask)
    {
      int i = 0;

      p[i++] = bin_map (1);			// data segment sync pulse
      p[i++] = bin_map (0);
      p[i++] = bin_map (0);
      p[i++] = bin_map (1);

      for (int j = 0; j < 511; j++)		// PN511
        p[i++] = bin_map (atsc_pn511[j]);

      for (int j = 0; j < 63; j++)		// PN63
        p[i++] = bin_map (atsc_pn63[j]);

      for (int j = 0; j < 63; j++)		// PN63, toggled on field 2
        p[i++] = bin_map (atsc_pn63[j] ^ mask);

      for (int j = 0; j < 63; j++)		// PN63
        p[i++] = bin_map (atsc_pn63[j]);
    }

    atsc_equalizer_impl::atsc_equalizer_impl()
      : gr::block("dtv_atsc_equalizer",
                  io_signature::make(1, 1, sizeof(atsc_soft_data_segment)),
                  io_signature::make(1, 1, sizeof(atsc_soft_data_segment)))
    {
      init_field_sync_common(training_sequence1, 0);
      init_field_sync_common(training_sequence2, 1);

      for (int i = 0; i < NTAPS; i++)
        d_taps[i] = 0.0;

      d_buff_not_filled = true;
    }

    atsc_equalizer_impl::~atsc_equalizer_impl()
    {
    }

    void
    atsc_equalizer_impl::filterN(const float *input_samples, float *output_samples, int nsamples)
    {
      for (int j = 0; j < nsamples; j++) {
        output_samples[j] = 0;
        for(int i = 0; i < NTAPS; i++)
          output_samples[j] += d_taps[i] * input_samples[j + i];
      }
    }

    void
    atsc_equalizer_impl::adaptN(const float *input_samples,
                                const float *training_pattern,
                                float *output_samples,
                                int    nsamples)
    {
      static const double BETA = 0.00005;	// FIXME figure out what this ought to be
	                                        // FIXME add gear-shifting

      for(int j = 0; j < nsamples; j++) {
        output_samples[j] = 0;
        for( int i = 0; i < NTAPS; i++ )
          output_samples[j] += d_taps[i] * input_samples[j + i];

        double e = output_samples[j] - training_pattern[j];

        // update taps...
        for( int i = 0; i < NTAPS; i++ )
          d_taps[i] -= BETA * e * (double)(input_samples[j + i]);
      }
    }

    int
    atsc_equalizer_impl::general_work(int noutput_items,
                                      gr_vector_int &ninput_items,
                                      gr_vector_const_void_star &input_items,
                                      gr_vector_void_star &output_items)
    {
      const atsc_soft_data_segment *in = (const atsc_soft_data_segment *) input_items[0];
      atsc_soft_data_segment *out = (atsc_soft_data_segment *) output_items[0];

      int output_produced = 0;
      int i = 0;

      if(d_buff_not_filled) {
        for(int j = 0; j < ATSC_DATA_SEGMENT_LENGTH; j++)
          data_mem[NPRETAPS + j] = in[i].data[j];
        d_flags = in[i].pli._flags;
        d_segno = in[i].pli._segno;
        d_buff_not_filled = false;
        i++;
      }

      for (; i < noutput_items; i++) {

        for(int j = 0; j < NTAPS - NPRETAPS; j++)
          data_mem[ATSC_DATA_SEGMENT_LENGTH + NPRETAPS + j] = in[i].data[j];

        if(d_segno == -1) {
          if(d_flags & 0x0010) {
            adaptN(data_mem, training_sequence2, data_mem2, KNOWN_FIELD_SYNC_LENGTH);
            //filterN(&data_mem[KNOWN_FIELD_SYNC_LENGTH], data_mem2, ATSC_DATA_SEGMENT_LENGTH - KNOWN_FIELD_SYNC_LENGTH);
          }
          else if( !(d_flags & 0x0010) ) {
            adaptN( data_mem, training_sequence1, data_mem2, KNOWN_FIELD_SYNC_LENGTH );
            //filterN(&data_mem[KNOWN_FIELD_SYNC_LENGTH], data_mem2, ATSC_DATA_SEGMENT_LENGTH - KNOWN_FIELD_SYNC_LENGTH);
          }
        }
        else {
          filterN(data_mem, data_mem2, ATSC_DATA_SEGMENT_LENGTH);

          for(int j = 0; j < ATSC_DATA_SEGMENT_LENGTH; j++)
            out[output_produced].data[j] = data_mem2[j];

          out[output_produced].pli._flags = d_flags;
          out[output_produced].pli._segno = d_segno;
          output_produced++;
        }

        for( int j = 0; j < NPRETAPS; j++ )
          data_mem[j] = data_mem[ATSC_DATA_SEGMENT_LENGTH + j];

        for(int j = 0; j < ATSC_DATA_SEGMENT_LENGTH; j++)
          data_mem[NPRETAPS + j] = in[i].data[j];

        d_flags = in[i].pli._flags;
        d_segno = in[i].pli._segno;
      }

      consume_each(noutput_items);
      return output_produced;
    }

  } /* namespace dtv */
} /* namespace gr */
