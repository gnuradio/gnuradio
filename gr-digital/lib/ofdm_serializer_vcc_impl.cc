/* -*- c++ -*- */
/* Copyright 2012,2014 Free Software Foundation, Inc.
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

#include <gnuradio/io_signature.h>
#include "ofdm_serializer_vcc_impl.h"

namespace gr {
  namespace digital {

    ofdm_serializer_vcc::sptr
    ofdm_serializer_vcc::make(
	int fft_len,
	const std::vector<std::vector<int> > &occupied_carriers,
	const std::string &len_tag_key,
	const std::string &packet_len_tag_key,
	int symbols_skipped,
	const std::string &carr_offset_key,
	bool input_is_shifted
    )
    {
      return gnuradio::get_initial_sptr (
	  new ofdm_serializer_vcc_impl(
		      fft_len, occupied_carriers,
		      len_tag_key, packet_len_tag_key,
		      symbols_skipped,
		      carr_offset_key,
		      input_is_shifted
	  )
      );
    }

    ofdm_serializer_vcc::sptr
    ofdm_serializer_vcc::make(
		const gr::digital::ofdm_carrier_allocator_cvc::sptr &allocator,
		const std::string &packet_len_tag_key,
		int symbols_skipped,
		const std::string &carr_offset_key,
		bool input_is_shifted
    )
    {
      return gnuradio::get_initial_sptr(
	  new ofdm_serializer_vcc_impl(
	    allocator->fft_len(),
	    allocator->occupied_carriers(),
	    allocator->len_tag_key(),
	    packet_len_tag_key,
	    symbols_skipped,
	    carr_offset_key,
	    !input_is_shifted
	  )
      );
    }

    ofdm_serializer_vcc_impl::ofdm_serializer_vcc_impl (
		    int fft_len,
		    const std::vector<std::vector<int> > &occupied_carriers,
		    const std::string &len_tag_key,
		    const std::string &packet_len_tag_key,
		    int symbols_skipped,
		    const std::string &carr_offset_key,
		    bool input_is_shifted)
      : tagged_stream_block ("ofdm_serializer_vcc",
		       io_signature::make(1, 1, sizeof (gr_complex) * fft_len),
		       io_signature::make(1, 1, sizeof (gr_complex)),
		       len_tag_key),
	    d_fft_len(fft_len),
	    d_occupied_carriers(occupied_carriers),
	    d_packet_len_tag_key(pmt::string_to_symbol(packet_len_tag_key)),
	    d_out_len_tag_key(pmt::string_to_symbol((packet_len_tag_key.empty() ? len_tag_key : packet_len_tag_key))),
	    d_symbols_skipped(symbols_skipped % occupied_carriers.size()),
	    d_carr_offset_key(pmt::string_to_symbol(carr_offset_key)),
	    d_curr_set(symbols_skipped % occupied_carriers.size()),
	    d_symbols_per_set(0)
    {
      for (unsigned i = 0; i < d_occupied_carriers.size(); i++) {
	for (unsigned k = 0; k < d_occupied_carriers[i].size(); k++) {
	  if (input_is_shifted) {
	    d_occupied_carriers[i][k] += fft_len/2;
	    if (d_occupied_carriers[i][k] > fft_len) {
	      d_occupied_carriers[i][k] -= fft_len;
	    }
	  } else {
	    if (d_occupied_carriers[i][k] < 0) {
	      d_occupied_carriers[i][k] += fft_len;
	    }
	  }
	  if (d_occupied_carriers[i][k] >= fft_len || d_occupied_carriers[i][k] < 0) {
	    throw std::invalid_argument("ofdm_serializer_vcc: trying to occupy a carrier outside the fft length.");
	  }
	}
      }

      for (unsigned i = 0; i < d_occupied_carriers.size(); i++) {
	d_symbols_per_set += d_occupied_carriers[i].size();
      }
      set_relative_rate((double) d_symbols_per_set / d_occupied_carriers.size());
      set_tag_propagation_policy(TPP_DONT);
    }

    ofdm_serializer_vcc_impl::~ofdm_serializer_vcc_impl()
    {
    }

    int
    ofdm_serializer_vcc_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int nout = (ninput_items[0] / d_occupied_carriers.size()) * d_symbols_per_set;
      for (unsigned i = 0; i < ninput_items[0] % d_occupied_carriers.size(); i++) {
	nout += d_occupied_carriers[(i + d_curr_set) % d_occupied_carriers.size()].size();
      }
      return nout;
    }

    void
    ofdm_serializer_vcc_impl::update_length_tags(int n_produced, int n_ports)
    {
      add_item_tag(0, nitems_written(0),
	d_out_len_tag_key,
	pmt::from_long(n_produced)
      );
    }

    int
    ofdm_serializer_vcc_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *) input_items[0];
      gr_complex *out = (gr_complex *) output_items[0];
      long frame_length = ninput_items[0]; // Input frame
      long packet_length = 0; // Output frame
      int carr_offset = 0;

      std::vector<tag_t> tags;
      // Packet mode
      if (!d_length_tag_key_str.empty()) {
        get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+1);
        for (unsigned i = 0; i < tags.size(); i++) {
          if (tags[i].key == d_carr_offset_key) {
            carr_offset = pmt::to_long(tags[i].value);
          }
          if (tags[i].key == d_packet_len_tag_key) {
            packet_length = pmt::to_long(tags[i].value);
          }
        }
      } else {
        // recalc frame length from noutput_items
        frame_length = 0;
        int sym_per_frame = 0;
        while ((sym_per_frame + d_occupied_carriers[(frame_length + 1) % d_occupied_carriers.size()].size()) < (size_t)noutput_items) {
          frame_length++;
          sym_per_frame += d_occupied_carriers[(frame_length + 1) % d_occupied_carriers.size()].size();
        }
      }

      // Copy symbols
      int n_out_symbols = 0;
      for (int i = 0; i < frame_length; i++) {
        // Copy all tags associated with this input OFDM symbol onto the first output symbol
        get_tags_in_range(tags, 0,
            nitems_read(0)+i,
            nitems_read(0)+i+1
        );
        for (size_t t = 0; t < tags.size(); t++) {
          // The packet length tag is not propagated
          if (tags[t].key != d_packet_len_tag_key) {
            add_item_tag(0, nitems_written(0)+n_out_symbols,
                tags[t].key,
                tags[t].value
            );
          }
        }
        for (unsigned k = 0; k < d_occupied_carriers[d_curr_set].size(); k++) {
          out[n_out_symbols++] = in[i * d_fft_len + d_occupied_carriers[d_curr_set][k] + carr_offset];
        }
        if (packet_length && n_out_symbols > packet_length) {
          n_out_symbols = packet_length;
          break;
        }
        d_curr_set = (d_curr_set + 1) % d_occupied_carriers.size();
      }

      // Housekeeping
      if (d_length_tag_key_str.empty()) {
        consume_each(frame_length);
      } else {
        d_curr_set = d_symbols_skipped;
      }

      return n_out_symbols;
    }

  } /* namespace digital */
} /* namespace gr */

