/* -*- c++ -*- */
/*
 * Copyright 2012 Free Software Foundation, Inc.
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
#include "repack_bits_bb_impl.h"

namespace gr {
  namespace blocks {

    repack_bits_bb::sptr
    repack_bits_bb::make(int k, int l, const std::string &len_tag_key, bool align_output)
    {
      return gnuradio::get_initial_sptr (new repack_bits_bb_impl(k, l, len_tag_key, align_output));
    }

    repack_bits_bb_impl::repack_bits_bb_impl(int k, int l, const std::string &len_tag_key, bool align_output)
      : tagged_stream_block("repack_bits_bb",
		      io_signature::make(1, 1, sizeof (char)),
		      io_signature::make(1, 1, sizeof (char)),
		      len_tag_key),
      d_k(k), d_l(l),
      d_packet_mode(!len_tag_key.empty()),
      d_in_index(0), d_out_index(0),
      d_align_output(align_output)
    {
      if (d_k > 8 || d_k < 1 || d_l > 8 || d_l < 1) {
	throw std::invalid_argument("k and l must be in [1, 8]");
      }

      set_relative_rate((double) d_k / d_l);
    }

    repack_bits_bb_impl::~repack_bits_bb_impl()
    {
    }

    int
    repack_bits_bb_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      int n_out_bytes_required = (ninput_items[0] * d_k) / d_l;
      if ((ninput_items[0] * d_k) % d_l && (!d_packet_mode || (d_packet_mode && !d_align_output))) {
	n_out_bytes_required++;
      }

      return n_out_bytes_required;
    }

    int
    repack_bits_bb_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      int bytes_to_write = noutput_items;

      if (d_packet_mode) { // noutput_items could be larger than necessary
	int bytes_to_read = ninput_items[0];
	bytes_to_write = bytes_to_read * d_k / d_l;
	if (!d_align_output && (((bytes_to_read * d_k) % d_l) != 0)) {
	  bytes_to_write++;
	}
      }

      int n_read = 0;
      int n_written = 0;
      while(n_written < bytes_to_write && n_read < ninput_items[0]) {
	if (d_out_index == 0) { // Starting a fresh byte
	  out[n_written] = 0;
	}
	out[n_written] |= ((in[n_read] >> d_in_index) & 0x01) << d_out_index;

	d_in_index = (d_in_index + 1) % d_k;
	d_out_index = (d_out_index + 1) % d_l;
	if (d_in_index == 0) {
	  n_read++;
	  d_in_index = 0;
	}
	if (d_out_index == 0) {
	  n_written++;
	  d_out_index = 0;
	}
      }

      if (d_packet_mode) {
	if (d_out_index) {
	  n_written++;
	  d_out_index = 0;
	}
      } else {
	consume_each(n_read);
      }

      return n_written;
    }

  } /* namespace blocks */
} /* namespace gr */

