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

#include "tagged_stream_to_pdu_impl.h"
#include <gnuradio/blocks/pdu.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    tagged_stream_to_pdu::sptr
    tagged_stream_to_pdu::make(pdu::vector_type type, const std::string& lengthtagname)
    {
      return gnuradio::get_initial_sptr(new tagged_stream_to_pdu_impl(type, lengthtagname));
    }

    tagged_stream_to_pdu_impl::tagged_stream_to_pdu_impl(pdu::vector_type type, const std::string& lengthtagname)
      : tagged_stream_block("tagged_stream_to_pdu",
		      io_signature::make(1, 1, pdu::itemsize(type)),
		      io_signature::make(0, 0, 0), lengthtagname),
	d_type(type),
	d_pdu_meta(pmt::PMT_NIL),
	d_pdu_vector(pmt::PMT_NIL)
    {
      message_port_register_out(pdu::pdu_port_id());
    }

    int
    tagged_stream_to_pdu_impl::work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t*) input_items[0];

      // Grab tags, throw them into dict
      get_tags_in_range(d_tags, 0,
	  nitems_read(0),
	  nitems_read(0) + ninput_items[0]
      );
      d_pdu_meta = pmt::make_dict();
      for (d_tags_itr = d_tags.begin(); d_tags_itr != d_tags.end(); d_tags_itr++) {
	  d_pdu_meta = dict_add(d_pdu_meta, (*d_tags_itr).key, (*d_tags_itr).value);
      }

      // Grab data, throw into vector
      d_pdu_vector = pdu::make_pdu_vector(d_type, in, ninput_items[0]);

      // Send msg
      pmt::pmt_t msg = pmt::cons(d_pdu_meta, d_pdu_vector);
      message_port_pub(pdu::pdu_port_id(), msg);

      return ninput_items[0];
    }

  } /* namespace blocks */
} /* namespace gr */
