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

#include "pdu_to_tagged_stream_impl.h"
#include <gnuradio/blocks/pdu.h>
#include <gnuradio/io_signature.h>

namespace gr {
  namespace blocks {

    pdu_to_tagged_stream::sptr
    pdu_to_tagged_stream::make(pdu::vector_type type, const std::string& tsb_tag_key)
    {
      return gnuradio::get_initial_sptr(new pdu_to_tagged_stream_impl(type, tsb_tag_key));
    }

    pdu_to_tagged_stream_impl::pdu_to_tagged_stream_impl(pdu::vector_type type, const std::string& tsb_tag_key)
      : tagged_stream_block("pdu_to_tagged_stream",
                            io_signature::make(0, 0, 0),
                            io_signature::make(1, 1, pdu::itemsize(type)),
                            tsb_tag_key),
        d_itemsize(pdu::itemsize(type)),
        d_type(type),
        d_curr_len(0)
    {
      message_port_register_in(pdu::pdu_port_id());
    }

    int pdu_to_tagged_stream_impl::calculate_output_stream_length(const gr_vector_int &)
    {
      if (d_curr_len == 0) {
          /* FIXME: This blocking call is far from ideal but is the best we
	   *        can do at the moment
	   */
        pmt::pmt_t msg(delete_head_blocking(pdu::pdu_port_id(), 100));
        if (msg.get() == NULL) {
          return 0;
        }

        if (!pmt::is_pair(msg))
          throw std::runtime_error("received a malformed pdu message");

        d_curr_meta = pmt::car(msg);
        d_curr_vect = pmt::cdr(msg);
        // do not assume the length of  PMT is in items (e.g.: from socket_pdu)
        d_curr_len = pmt::blob_length(d_curr_vect)/d_itemsize;
      }

      return d_curr_len;
    }

    int
    pdu_to_tagged_stream_impl::work (int noutput_items,
                                     gr_vector_int &ninput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items)
    {
      uint8_t *out = (uint8_t*) output_items[0];

      if (d_curr_len == 0) {
        return 0;
      }

      // work() should only be called if the current PDU fits entirely
      // into the output buffer.
      assert(noutput_items >= d_curr_len);

      // Copy vector output
      size_t nout = d_curr_len;
      size_t io(0);
      const uint8_t* ptr = (const uint8_t*) uniform_vector_elements(d_curr_vect, io);
      memcpy(out, ptr, d_curr_len*d_itemsize);

      // Copy tags
      if (!pmt::eq(d_curr_meta, pmt::PMT_NIL) ) {
        pmt::pmt_t klist(pmt::dict_keys(d_curr_meta));
        for (size_t i = 0; i < pmt::length(klist); i++) {
          pmt::pmt_t k(pmt::nth(i, klist));
          pmt::pmt_t v(pmt::dict_ref(d_curr_meta, k, pmt::PMT_NIL));
          add_item_tag(0, nitems_written(0), k, v, alias_pmt());
        }
      }

      // Reset state
      d_curr_len = 0;

      return nout;
    } /* work() */

  } /* namespace blocks */
} /* namespace gr */
