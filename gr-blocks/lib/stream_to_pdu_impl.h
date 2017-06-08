/* -*- c++ -*- */
/* 
 * Copyright 2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_BLOCKS_STREAM_TO_PDU_IMPL_H
#define INCLUDED_BLOCKS_STREAM_TO_PDU_IMPL_H

#include <gnuradio/blocks/stream_to_pdu.h>

namespace gr {
  namespace blocks {

    class stream_to_pdu_impl : public stream_to_pdu
    {
     private:
      pdu::vector_type d_type;
      bool d_fixed_mode;
      int d_packet_length;
      std::string d_tag_key;
      std::string d_offset_key;
      bool d_in_copy;
      uint64_t d_start_offset;
      uint64_t d_stop_offset;
      int d_items_copied;
      pmt::pmt_t d_pdu_meta;
      pmt::pmt_t d_pdu_vector;

      void copy_pdu_vector_items(const void* in, int noutput_items);

     public:
      stream_to_pdu_impl(pdu::vector_type type, bool fixed_mode, const std::string& tag_key, const std::string& offset_key, int packet_length);
      ~stream_to_pdu_impl();
      
      // Where all the action really happens
      int work(int noutput_items,
         gr_vector_const_void_star &input_items,
         gr_vector_void_star &output_items);
    };

  } // namespace blocks
} // namespace gr

#endif /* INCLUDED_BLOCKS_STREAM_TO_PDU_IMPL_H */

