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
      : sync_block("tagged_stream_to_pdu",
		      io_signature::make(1, 1, pdu::itemsize(type)),
		      io_signature::make(0, 0, 0)),
	d_itemsize(pdu::itemsize(type)),
	d_inpdu(false),
	d_type(type),
	d_pdu_meta(pmt::PMT_NIL),
	d_pdu_vector(pmt::PMT_NIL),
	d_tag(pmt::mp(lengthtagname))
    {
      message_port_register_out(PDU_PORT_ID);
    }

    int
    tagged_stream_to_pdu_impl::work(int noutput_items,
				    gr_vector_const_void_star &input_items,
				    gr_vector_void_star &output_items)
    {
      const uint8_t *in = (const uint8_t*) input_items[0];
      uint64_t abs_N = nitems_read(0);

      // if we are not in a pdu already, start a new one 
      if (!d_inpdu) {
	bool found_length_tag(false);

	get_tags_in_range(d_tags, 0, abs_N, abs_N+1);

	for (d_tags_itr = d_tags.begin(); (d_tags_itr != d_tags.end()) && (!found_length_tag); d_tags_itr++) {
	  if (pmt::eq((*d_tags_itr).key, d_tag)) {

          if ((*d_tags_itr).offset != abs_N )
	    throw std::runtime_error("expected next pdu length tag on a different item...");

	  found_length_tag = true;
	  d_pdu_length = pmt::to_long((*d_tags_itr).value);
	  d_pdu_remain = d_pdu_length;
	  d_pdu_meta = pmt::make_dict();
              break;
          } // if have length tag
	} // iter over tags

	if (!found_length_tag)
	  throw std::runtime_error("tagged stream does not contain a pdu_length tag");
      }

      size_t ncopy = std::min((size_t)noutput_items, d_pdu_remain);

      // copy any tags in this range into our meta object
      get_tags_in_range(d_tags, 0, abs_N, abs_N+ncopy);
      for (d_tags_itr = d_tags.begin(); d_tags_itr != d_tags.end(); d_tags_itr++)
	if(!pmt::eq((*d_tags_itr).key, d_tag ))
	  d_pdu_meta = dict_add(d_pdu_meta, (*d_tags_itr).key, (*d_tags_itr).value);

      // copy samples for this vector into either a pmt or our save buffer
      if (ncopy == d_pdu_remain) { // we will send this pdu
	if (d_save.size() == 0) {
	  d_pdu_vector = pdu::make_pdu_vector(d_type, in, ncopy);
	  send_message();
	} 
	else {
	  size_t oldsize = d_save.size();
	  d_save.resize((oldsize + ncopy)*d_itemsize, 0);
	  memcpy(&d_save[oldsize*d_itemsize], in, ncopy*d_itemsize);
	  d_pdu_vector = pdu::make_pdu_vector(d_type, &d_save[0], d_pdu_length);
	  send_message();
	  d_save.clear();
        }
      } 
      else {
        d_inpdu = true;
        size_t oldsize = d_save.size();
        d_save.resize((oldsize+ncopy)*d_itemsize);
        memcpy(&d_save[oldsize*d_itemsize], in, ncopy*d_itemsize);
        d_pdu_remain -= ncopy;
      }

      return ncopy;
    }

    void
    tagged_stream_to_pdu_impl::send_message()
    {
      if (pmt::length(d_pdu_vector) != d_pdu_length)
        throw std::runtime_error("msg length not correct");
      
      pmt::pmt_t msg = pmt::cons(d_pdu_meta, d_pdu_vector);
      message_port_pub(PDU_PORT_ID, msg);
      
      d_pdu_meta = pmt::PMT_NIL;
      d_pdu_vector = pmt::PMT_NIL;
      d_pdu_length = 0;
      d_pdu_remain = 0;
      d_inpdu = false;
    }
    
  } /* namespace blocks */
} /* namespace gr */
