/* -*- c++ -*- */
/*
 * Copyright 2004,2010,2012 Free Software Foundation, Inc.
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

#include "simple_framer_impl.h"
#include <gnuradio/digital/simple_framer_sync.h>
#include <gnuradio/io_signature.h>
#include <assert.h>
#include <string>

namespace gr {
  namespace digital {

    simple_framer::sptr
    simple_framer::make(int payload_bytesize)
    {
      return gnuradio::get_initial_sptr
	(new simple_framer_impl(payload_bytesize));
    }

    simple_framer_impl::simple_framer_impl(int payload_bytesize)
      : block("simple_framer",
		 io_signature::make(1, 1, sizeof(unsigned char)),
		 io_signature::make(1, 1, sizeof(unsigned char))),
	d_seqno (0), d_payload_bytesize (payload_bytesize),
	d_input_block_size (payload_bytesize),
	d_output_block_size (payload_bytesize + GRSF_OVERHEAD)
    {
      set_output_multiple(d_output_block_size);
    }

    simple_framer_impl::~simple_framer_impl()
    {
    }

    void
    simple_framer_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      assert(noutput_items % d_output_block_size == 0);

      int nblocks = noutput_items / d_output_block_size;
      int input_required = nblocks * d_input_block_size;

      unsigned ninputs = ninput_items_required.size();
      for(unsigned int i = 0; i < ninputs; i++)
	ninput_items_required[i] = input_required;
    }

    int
    simple_framer_impl::general_work(int noutput_items,
				     gr_vector_int &ninput_items,
				     gr_vector_const_void_star &input_items,
				     gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char*)input_items[0];
      unsigned char *out = (unsigned char*)output_items[0];

      int n = 0;
      int nblocks = 0;

      memset(out, 0x55, noutput_items);

      while(n < noutput_items) {
	out[0] = (GRSF_SYNC >> 56) & 0xff;
	out[1] = (GRSF_SYNC >> 48) & 0xff;
	out[2] = (GRSF_SYNC >> 40) & 0xff;
	out[3] = (GRSF_SYNC >> 32) & 0xff;
	out[4] = (GRSF_SYNC >> 24) & 0xff;
	out[5] = (GRSF_SYNC >> 16) & 0xff;
	out[6] = (GRSF_SYNC >>  8) & 0xff;
	out[7] = (GRSF_SYNC >>  0) & 0xff;
	out[8] = d_seqno++;

	memcpy(&out[9], in, d_input_block_size);
	in += d_input_block_size;
	out += d_output_block_size;
	n += d_output_block_size;
	nblocks++;
      }

      assert(n == noutput_items);

      consume_each(nblocks * d_input_block_size);
      return n;
    }

  } /* namespace digital */
} /* namespace gr */
