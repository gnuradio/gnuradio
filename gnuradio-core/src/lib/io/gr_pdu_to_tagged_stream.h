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

#ifndef INCLUDED_GR_PDU_TO_TAGGED_STREAM_H
#define INCLUDED_GR_PDU_TO_TAGGED_STREAM_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>
#include <gr_pdu.h>

class gr_pdu_to_tagged_stream;
typedef boost::shared_ptr<gr_pdu_to_tagged_stream> gr_pdu_to_tagged_stream_sptr;
  
GR_CORE_API gr_pdu_to_tagged_stream_sptr gr_make_pdu_to_tagged_stream (gr_pdu_vector_type t);

/*!
 * \brief Turn received messages into a stream
 * \ingroup source_blk
 */
class GR_CORE_API gr_pdu_to_tagged_stream : public gr_sync_block
{
 private:
  gr_pdu_vector_type    d_vectortype;
  size_t 	        d_itemsize;
  std::vector<uint8_t>  d_remain;

  friend GR_CORE_API gr_pdu_to_tagged_stream_sptr
  gr_make_pdu_to_tagged_stream(gr_pdu_vector_type t);

 protected:
  gr_pdu_to_tagged_stream (gr_pdu_vector_type t);

 public:
  ~gr_pdu_to_tagged_stream ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

};

#endif /* INCLUDED_GR_PDU_TO_TAGGED_SOURCE_H */
