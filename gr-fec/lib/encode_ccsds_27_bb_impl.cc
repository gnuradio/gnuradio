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

#include "encode_ccsds_27_bb_impl.h"
#include <gnuradio/io_signature.h>

extern "C" {
#include <gnuradio/fec/viterbi.h>
}

namespace gr {
  namespace fec {

    encode_ccsds_27_bb::sptr encode_ccsds_27_bb::make()
    {
      return gnuradio::get_initial_sptr(new encode_ccsds_27_bb_impl());
    }

    encode_ccsds_27_bb_impl::encode_ccsds_27_bb_impl()
      : sync_interpolator("encode_ccsds_27_bb",
			     io_signature::make (1, 1, sizeof(char)),
			     io_signature::make (1, 1, sizeof(char)),
			     16)  // Rate 1/2 code, packed to unpacked conversion
    {
      d_encstate = 0;
    }

    int
    encode_ccsds_27_bb_impl::work(int noutput_items,
				  gr_vector_const_void_star &input_items,
				  gr_vector_void_star &output_items)
    {
      unsigned char *in = (unsigned char *)input_items[0];
      unsigned char *out = (unsigned char *)output_items[0];

      d_encstate = encode(out, in, noutput_items/16, d_encstate);

      return noutput_items;
    }

  } /* namespace fec */
}/* namespace gr */
