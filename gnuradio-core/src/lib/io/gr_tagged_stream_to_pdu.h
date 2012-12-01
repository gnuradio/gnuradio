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

#ifndef INCLUDED_GR_TAGGED_STREAM_TO_PDU_H
#define INCLUDED_GR_TAGGED_STREAM_TO_PDU_H

#include <gr_core_api.h>
#include <gr_sync_block.h>
#include <gr_message.h>
#include <gr_msg_queue.h>
#include <gr_pdu.h>

class gr_tagged_stream_to_pdu;
typedef boost::shared_ptr<gr_tagged_stream_to_pdu> gr_tagged_stream_to_pdu_sptr;
  
GR_CORE_API gr_tagged_stream_to_pdu_sptr gr_make_tagged_stream_to_pdu (gr_pdu_vector_type t);

/*!
 * \brief Turn received messages into a stream
 * \ingroup source_blk
 */
class GR_CORE_API gr_tagged_stream_to_pdu : public gr_sync_block
{
 private:
  gr_pdu_vector_type    d_vectortype;
  size_t                d_itemsize;

  std::vector<uint8_t>  d_save;

  std::vector<gr_tag_t> d_tags;
  std::vector<gr_tag_t>::iterator d_tags_itr;

  bool d_inpdu;

  size_t                d_pdu_length;
  size_t                d_pdu_remain;
  pmt::pmt_t            d_pdu_meta;
  pmt::pmt_t            d_pdu_vector;

  friend GR_CORE_API gr_tagged_stream_to_pdu_sptr
  gr_make_tagged_stream_to_pdu(gr_pdu_vector_type t);

 protected:
  gr_tagged_stream_to_pdu (gr_pdu_vector_type t);

 public:
  ~gr_tagged_stream_to_pdu ();

  int work (int noutput_items,
	    gr_vector_const_void_star &input_items,
	    gr_vector_void_star &output_items);

  void send_message();

};

#endif /* INCLUDED_GR_PDU_TO_TAGGED_SOURCE_H */
