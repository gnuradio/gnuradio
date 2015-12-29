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
#include <volk/volk.h>

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

      d_taps.resize(NTAPS, 0.0f);

      d_buff_not_filled = true;

      const int alignment_multiple =
	volk_get_alignment() / sizeof(float);
      set_alignment(std::max(1, alignment_multiple));
    }

    atsc_equalizer_impl::~atsc_equalizer_impl()
    {
    }

    std::vector<float>
    atsc_equalizer_impl::taps() const
    {
      return d_taps;
    }

    std::vector<float>
    atsc_equalizer_impl::data() const
    {
      std::vector<float> ret(&data_mem2[0], &data_mem2[ATSC_DATA_SEGMENT_LENGTH-1]);
      return ret;
    }

    void
    atsc_equalizer_impl::filterN(const float *input_samples,
                                 float *output_samples,
                                 int nsamples)
    {
      for (int j = 0; j < nsamples; j++) {
        output_samples[j] = 0;
        volk_32f_x2_dot_prod_32f(&output_samples[j],
                                 &input_samples[j],
                                 &d_taps[0], NTAPS);
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
        volk_32f_x2_dot_prod_32f(&output_samples[j],
                                 &input_samples[j],
                                 &d_taps[0], NTAPS);

        float e = output_samples[j] - training_pattern[j];

        // update taps...
        float tmp_taps[NTAPS];
        volk_32f_s32f_multiply_32f(tmp_taps, &input_samples[j], BETA*e, NTAPS);
        volk_32f_x2_subtract_32f(&d_taps[0], &d_taps[0], tmp_taps, NTAPS);
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
        memcpy(&data_mem[NPRETAPS], in[i].data,
               ATSC_DATA_SEGMENT_LENGTH*sizeof(float));
        d_flags = in[i].pli._flags;
        d_segno = in[i].pli._segno;
        d_buff_not_filled = false;
        i++;
      }

      for (; i < noutput_items; i++) {

        memcpy(&data_mem[ATSC_DATA_SEGMENT_LENGTH + NPRETAPS], in[i].data,
               (NTAPS - NPRETAPS)*sizeof(float));

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

          memcpy(out[output_produced].data, data_mem2,
                 ATSC_DATA_SEGMENT_LENGTH*sizeof(float));

          out[output_produced].pli._flags = d_flags;
          out[output_produced].pli._segno = d_segno;
          output_produced++;
        }

        memcpy(data_mem, &data_mem[ATSC_DATA_SEGMENT_LENGTH],
               NPRETAPS*sizeof(float));
        memcpy(&data_mem[NPRETAPS], in[i].data,
               ATSC_DATA_SEGMENT_LENGTH*sizeof(float));

        d_flags = in[i].pli._flags;
        d_segno = in[i].pli._segno;
      }

      consume_each(noutput_items);
      return output_produced;
    }

    void
    atsc_equalizer_impl::setup_rpc()
    {
#ifdef GR_CTRLPORT
      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<atsc_equalizer, std::vector<float> >(
	  alias(), "taps",
	  &atsc_equalizer::taps,
	  pmt::make_f32vector(1,-10),
	  pmt::make_f32vector(1,10),
	  pmt::make_f32vector(1,0),
	  "", "Equalizer Taps", RPC_PRIVLVL_MIN,
          DISPTIME)));

      add_rpc_variable(
        rpcbasic_sptr(new rpcbasic_register_get<atsc_equalizer, std::vector<float> >(
	  alias(), "data",
	  &atsc_equalizer::data,
	  pmt::make_f32vector(1,-10),
	  pmt::make_f32vector(1,10),
	  pmt::make_f32vector(1,0),
	  "", "Post-equalizer Data", RPC_PRIVLVL_MIN,
          DISPTIME)));
#endif /* GR_CTRLPORT */
    }

  } /* namespace dtv */
} /* namespace gr */
