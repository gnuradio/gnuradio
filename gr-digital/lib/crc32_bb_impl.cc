/* -*- c++ -*- */
/* 
 * Copyright 2013 Free Software Foundation, Inc.
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
#include "crc32_bb_impl.h"

namespace gr {
  namespace digital {

    crc32_bb::sptr
    crc32_bb::make(bool check, const std::string& lengthtagname)
    {
      return gnuradio::get_initial_sptr (new crc32_bb_impl(check, lengthtagname));
    }

    crc32_bb_impl::crc32_bb_impl(bool check, const std::string& lengthtagname)
      : tagged_stream_block("crc32_bb",
		   io_signature::make(1, 1, sizeof (char)),
		   io_signature::make(1, 1, sizeof (char)),
		   lengthtagname),
	d_check(check)
    {
      set_tag_propagation_policy(TPP_DONT);
    }

    crc32_bb_impl::~crc32_bb_impl()
    {
    }

    int
    crc32_bb_impl::calculate_output_stream_length(const gr_vector_int &ninput_items)
    {
      if (d_check) {
	return ninput_items[0] - 4;
      } else {
	return ninput_items[0] + 4;
      }
    }

    int
    crc32_bb_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      long packet_length = ninput_items[0];
      int packet_size_diff = d_check ? -4 : 4;
      unsigned int crc;

      if (d_check) {
    //	crc = gr::digital::crc32(in, packet_length-4);
        d_crc_impl.reset();
        d_crc_impl.process_bytes(in, packet_length-4);
        crc = d_crc_impl();
	if (crc != *(unsigned int *)(in+packet_length-4)) { // Drop package
      d_nfail++;
	  return 0;
	}
    d_npass++;
	memcpy((void *) out, (const void *) in, packet_length-4);
      } else {
        d_crc_impl.reset();
        d_crc_impl.process_bytes(in, packet_length);
        crc = d_crc_impl();
	   // crc = gr::digital::crc32(in, packet_length);
	memcpy((void *) out, (const void *) in, packet_length);
	memcpy((void *) (out + packet_length), &crc, 4); // FIXME big-endian/little-endian, this might be wrong
      }

      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+packet_length);
      for (size_t i = 0; i < tags.size(); i++) {
	tags[i].offset -= nitems_read(0);
	if (d_check && tags[i].offset > (unsigned int)(packet_length+packet_size_diff)) {
	  tags[i].offset = packet_length-5;
	}
	add_item_tag(0, nitems_written(0) + tags[i].offset,
	    tags[i].key,
	    tags[i].value);
      }

      return packet_length + packet_size_diff;
    }

  } /* namespace digital */
} /* namespace gr */

