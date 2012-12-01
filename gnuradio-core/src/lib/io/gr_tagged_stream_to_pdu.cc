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

#include <gr_tagged_stream_to_pdu.h>
#include <gr_io_signature.h>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdexcept>
#include <string.h>

// public constructor that returns a shared_ptr

gr_tagged_stream_to_pdu_sptr
gr_make_tagged_stream_to_pdu(gr_pdu_vector_type t)
{
  return gnuradio::get_initial_sptr(new gr_tagged_stream_to_pdu(t));
}

gr_tagged_stream_to_pdu::gr_tagged_stream_to_pdu (gr_pdu_vector_type t)
  : gr_sync_block("tagged_stream_to_pdu",
		  gr_make_io_signature(1, 1, gr_pdu_itemsize(t)),
		  gr_make_io_signature(0, 0, 0)),
    d_vectortype(t), d_itemsize(gr_pdu_itemsize(t)), d_inpdu(false),
    d_pdu_meta(pmt::PMT_NIL), d_pdu_vector(pmt::PMT_NIL)
{
  message_port_register_out(pdu_port_id);
}

gr_tagged_stream_to_pdu::~gr_tagged_stream_to_pdu()
{
  printf("destructor running\n");
}

int
gr_tagged_stream_to_pdu::work(int noutput_items,
			      gr_vector_const_void_star &input_items,
			      gr_vector_void_star &output_items)
{
  const uint8_t *in = (const uint8_t*) input_items[0];
  uint64_t abs_N = nitems_read(0);

  // if we are not in a pdu already, start a new one 
  if(!d_inpdu){
    get_tags_in_range(d_tags, 0, abs_N, abs_N+1);
    bool found_length_tag(false);
    for(d_tags_itr = d_tags.begin(); (d_tags_itr != d_tags.end()) && (!found_length_tag); d_tags_itr++){
      if( pmt::pmt_equal( (*d_tags_itr).key, pdu_length_tag ) ){
          if( (*d_tags_itr).offset != abs_N ){
              throw std::runtime_error("expected next pdu length tag on a different item...");
              }
              found_length_tag = true;
              d_pdu_length = pmt::pmt_to_long( (*d_tags_itr).value );
              d_pdu_remain = d_pdu_length;
              d_pdu_meta = pmt::pmt_make_dict();
              break;
          } // if have length tag
      } // iter over tags
    if(!found_length_tag){
        throw std::runtime_error("tagged stream does not contain a pdu_length tag!");
    }
  }

  size_t ncopy = std::min((size_t)noutput_items, d_pdu_remain);

  // copy any tags in this range into our meta object
  get_tags_in_range(d_tags, 0, abs_N, abs_N+ncopy);
  for(d_tags_itr = d_tags.begin(); d_tags_itr != d_tags.end(); d_tags_itr++){
    if( ! pmt_equal( (*d_tags_itr).key, pdu_length_tag ) ){
        d_pdu_meta = pmt_dict_add(d_pdu_meta, (*d_tags_itr).key, (*d_tags_itr).value);
        }
    }

  // copy samples for this vector into either a pmt or our save buffer
  if(ncopy == d_pdu_remain){ // we will send this pdu
    if(d_save.size() == 0){
        d_pdu_vector = gr_pdu_make_vector(d_vectortype, in, ncopy);
        send_message();
      } else {
        size_t oldsize = d_save.size();
        d_save.resize((oldsize + ncopy)*d_itemsize, 0);
        memcpy( &d_save[oldsize*d_itemsize], in, ncopy*d_itemsize );
        d_pdu_vector = gr_pdu_make_vector(d_vectortype, &d_save[0], d_pdu_length);
        send_message();
        d_save.clear();
        }
    } else {
        d_inpdu = true;
        size_t oldsize = d_save.size();
        d_save.resize( (oldsize+ncopy)*d_itemsize );
        memcpy( &d_save[oldsize*d_itemsize], in, ncopy*d_itemsize );
        d_pdu_remain -= ncopy;
    }

  return ncopy;
}

void gr_tagged_stream_to_pdu::send_message(){

    if(pmt::pmt_length(d_pdu_vector) != d_pdu_length){
        throw std::runtime_error("msg length not correct");
    }

    pmt::pmt_t msg = pmt::pmt_cons( d_pdu_meta, d_pdu_vector );
    message_port_pub( pdu_port_id, msg );

    d_pdu_meta = pmt::PMT_NIL;
    d_pdu_vector = pmt::PMT_NIL;
    d_pdu_length = 0;
    d_pdu_remain = 0;
    d_inpdu = false;
}
