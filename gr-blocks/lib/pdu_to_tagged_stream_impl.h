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

#ifndef INCLUDED_PDU_TO_TAGGED_STREAM_IMPL_H
#define INCLUDED_PDU_TO_TAGGED_STREAM_IMPL_H

#include <gnuradio/blocks/pdu_to_tagged_stream.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API pdu_to_tagged_stream_impl : public pdu_to_tagged_stream
    {
      size_t               d_itemsize;
      pdu::vector_type     d_type;
      std::vector<uint8_t> d_remain;
      pmt::pmt_t           d_tag;
      int                  d_sleep_duration;
      bool                 d_no_message;

    public:
      pdu_to_tagged_stream_impl(pdu::vector_type type, const std::string& lengthtagname="packet_len", int sleep_duration=0);

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace blocks */
} /* namespace gr */

#endif 
