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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "stream_to_pdu_impl.h"

namespace gr {
  namespace blocks {

    stream_to_pdu::sptr
    stream_to_pdu::make(pdu::vector_type type, bool fixed_mode, const std::string& tag_key, const std::string& offset_key, int packet_length)
    {
      return gnuradio::get_initial_sptr
        (new stream_to_pdu_impl(type, fixed_mode, tag_key, offset_key, packet_length));
    }

    /*
     * The private constructor
     */
    stream_to_pdu_impl::stream_to_pdu_impl(pdu::vector_type type, bool fixed_mode, const std::string& tag_key, const std::string& offset_key, int packet_length)
      : gr::sync_block("stream_to_pdu",
              gr::io_signature::make(1, 1, pdu::itemsize(type)),
              gr::io_signature::make(0, 0, 0)),
        d_type(type),
        d_fixed_mode(fixed_mode),
        d_tag_key(tag_key),
        d_offset_key(offset_key),
        d_packet_length(packet_length),
        d_in_copy(false),
        d_start_offset(0),
        d_stop_offset(0),
        d_items_copied(0),
        d_pdu_meta(pmt::PMT_NIL),
        d_pdu_vector(pmt::PMT_NIL)
    {
      message_port_register_out(PDU_PORT_ID);
    }

    /*
     * Our virtual destructor.
     */
    stream_to_pdu_impl::~stream_to_pdu_impl()
    {
    }

    void
    stream_to_pdu_impl::copy_pdu_vector_items(const void* in, int noutput_items)
    {
      int start_idx = (int) (d_start_offset + d_items_copied - nitems_read(0));
      int stop_idx = (int) (d_stop_offset - nitems_read(0));

      // Get pointer to PDU vector
      void* p = pdu::pdu_vector_writable_elements(d_type, d_pdu_vector, d_packet_length);
      
      if(stop_idx > noutput_items) {
        // This PDU vector copy is going to continue past this work() call
        stop_idx = noutput_items;
        d_in_copy = true;
      }
      else {
        // This PDU vector copy can be completed during this work call
        d_in_copy = false;
      }

      // Take other tags from the packet window and add them to the PDU metadata dictionary
      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0) + start_idx, nitems_read(0) + stop_idx);
      for(unsigned int i = 0; i < tags.size(); i++) {
        // Only copy tags that are unique for the trigger tag
        if(pmt::symbol_to_string(tags[i].key) != d_tag_key) {
          d_pdu_meta = dict_add(d_pdu_meta, tags[i].key, tags[i].value);
        }
      }

      // Copy items into the PDU vector memory
      memcpy(p + d_items_copied*pdu::itemsize(d_type), in + start_idx*pdu::itemsize(d_type), (stop_idx - start_idx)*pdu::itemsize(d_type));
      d_items_copied += (stop_idx - start_idx);
    }

    int
    stream_to_pdu_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const void *in = (const void *) input_items[0];

      if(d_in_copy) {
        // If we are currently writing to a PDU packet, finish that PDU write before continuing
        copy_pdu_vector_items(in, noutput_items);
        if(d_in_copy) {
          return noutput_items;
        }
        else {
          message_port_pub(PDU_PORT_ID, pmt::cons(d_pdu_meta, d_pdu_vector));
        }
      }

      // Fetch new tags from the input stream
      std::vector<tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + noutput_items, pmt::string_to_symbol(d_tag_key));

      for(unsigned int i = 0; i < tags.size(); i++) {
        // Make sure this tag (start of next packet) doesn't overlap with the previous packet
        if(tags[i].offset >= d_stop_offset) {
          // Determine packet length
          if(!d_fixed_mode) {
            if(pmt::is_integer(tags[i].value)) {
              d_packet_length = (int) pmt::to_long(tags[i].value);
            }
            else {
              continue; // Invalid tag value, ignore this tag
            }
          }

          d_start_offset = tags[i].offset;
          d_stop_offset = d_start_offset + d_packet_length;
          d_items_copied = 0;

          // Initialize PDU metadata          
          d_pdu_meta = pmt::make_dict();
          d_pdu_meta = dict_add(d_pdu_meta, pmt::string_to_symbol(d_offset_key), pmt::from_uint64(d_start_offset));
          
          // Initialize PDU vector
          d_pdu_vector = pdu::make_empty_pdu_vector(d_type, d_packet_length);
          
          // Copy items into PDU vector
          copy_pdu_vector_items(in, noutput_items);
          if(d_in_copy) {
            return noutput_items;
          }
          else {
            message_port_pub(PDU_PORT_ID, pmt::cons(d_pdu_meta, d_pdu_vector));
          }
        }
      }
      
      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace blocks */
} /* namespace gr */

