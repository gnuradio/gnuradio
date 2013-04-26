/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#include <gr_io_signature.h>
#include "ofdm_frame_equalizer_vcvc_impl.h"

namespace gr {
  namespace digital {

    ofdm_frame_equalizer_vcvc::sptr
    ofdm_frame_equalizer_vcvc::make(
	digital_ofdm_equalizer_base_sptr equalizer,
	const std::string &len_tag_key,
	bool propagate_channel_state,
	int fixed_frame_len
    )
    {
      return gnuradio::get_initial_sptr (
	  new ofdm_frame_equalizer_vcvc_impl(
	    equalizer, len_tag_key, propagate_channel_state, fixed_frame_len
	  )
      );
    }

    ofdm_frame_equalizer_vcvc_impl::ofdm_frame_equalizer_vcvc_impl(
	digital_ofdm_equalizer_base_sptr equalizer,
	const std::string &len_tag_key,
	bool propagate_channel_state,
	int fixed_frame_len
    ) : gr_tagged_stream_block("ofdm_frame_equalizer_vcvc",
	  gr_make_io_signature(1, 1, sizeof (gr_complex) * equalizer->fft_len()),
	  gr_make_io_signature(1, 1, sizeof (gr_complex) * equalizer->fft_len()),
	  len_tag_key),
      d_fft_len(equalizer->fft_len()),
      d_eq(equalizer),
      d_propagate_channel_state(propagate_channel_state),
      d_fixed_frame_len(len_tag_key.empty() ? fixed_frame_len : 0),
      d_channel_state(equalizer->fft_len(), gr_complex(1, 0))
    {
      if (d_fixed_frame_len) {
	set_output_multiple(d_fixed_frame_len);
      }
    }

    ofdm_frame_equalizer_vcvc_impl::~ofdm_frame_equalizer_vcvc_impl()
    {
    }


    int
    ofdm_frame_equalizer_vcvc_impl::work(int noutput_items,
	  gr_vector_int &ninput_items,
	  gr_vector_const_void_star &input_items,
	  gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      int carrier_offset = 0;
      int frame_len = 0;
      if (d_fixed_frame_len) {
	frame_len = d_fixed_frame_len;
      } else {
	frame_len = ninput_items[0];
      }

      std::vector<gr_tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+1);
      for (unsigned i = 0; i < tags.size(); i++) {
	if (pmt::pmt_symbol_to_string(tags[i].key) == "ofdm_sync_chan_taps") {
	  d_channel_state = pmt::pmt_c32vector_elements(tags[i].value);
	  remove_item_tag(0, tags[i]);
	}
      }

      memcpy((void *) out, (void *) in, sizeof(gr_complex) * d_fft_len * frame_len);
      d_eq->reset();
      d_eq->set_carrier_offset(carrier_offset);
      d_eq->equalize(out, frame_len, d_channel_state);
      d_eq->get_channel_state(d_channel_state);
      if (d_propagate_channel_state) {
	add_item_tag(0, nitems_written(0),
	    pmt::pmt_string_to_symbol("ofdm_sync_chan_taps"),
	    pmt::pmt_init_c32vector(d_fft_len, d_channel_state));
      }

      if (d_fixed_frame_len) {
	consume_each(frame_len);
      }

      return frame_len;
    }

  } /* namespace digital */
} /* namespace gr */

