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

#ifndef INCLUDED_TAGGED_STREAM_TO_PDU_IMPL_H
#define INCLUDED_TAGGED_STREAM_TO_PDU_IMPL_H

#include <gnuradio/blocks/tagged_stream_to_pdu.h>

namespace gr {
  namespace blocks {

    class BLOCKS_API tagged_stream_to_pdu_impl : public tagged_stream_to_pdu
    {
      pdu::vector_type     d_type;
      pmt::pmt_t           d_pdu_meta;
      pmt::pmt_t           d_pdu_vector;
      std::vector<tag_t>::iterator d_tags_itr;
      std::vector<tag_t>   d_tags;

    public:
      tagged_stream_to_pdu_impl(pdu::vector_type type, const std::string& lengthtagname);

      int work(int noutput_items,
               gr_vector_int &ninput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

    };

  } /* namespace blocks */
} /* namespace gr */

#endif
