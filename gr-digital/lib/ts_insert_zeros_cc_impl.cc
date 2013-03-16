/* -*- c++ -*- */
/* 
 * Copyright 2012 <+YOU OR YOUR COMPANY+>.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gr_io_signature.h>
#include "ts_insert_zeros_cc_impl.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>

namespace gr {
  namespace digital {

    ts_insert_zeros_cc::sptr
    ts_insert_zeros_cc::make(const std::string &lengthtagname)
    {
      return gnuradio::get_initial_sptr (new ts_insert_zeros_cc_impl(lengthtagname));
    }

    /*
     * The private constructor
     */
    ts_insert_zeros_cc_impl::ts_insert_zeros_cc_impl(const std::string &lengthtagname)
      : gr_block("ts_insert_zeros_cc",
                 gr_make_io_signature(1, 1, sizeof(gr_complex)),
                 gr_make_io_signature(1, 1, sizeof(gr_complex))),
        d_lengthtagname(lengthtagname)
    {
    }

    /*
     * Our virtual destructor.
     */
    ts_insert_zeros_cc_impl::~ts_insert_zeros_cc_impl()
    {
    }

    void
    ts_insert_zeros_cc_impl::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = 0;
    }

    int
    ts_insert_zeros_cc_impl::general_work (
      int noutput_items,
      gr_vector_int &ninput_items,
      gr_vector_const_void_star &input_items,
      gr_vector_void_star &output_items
    )
    {
      gr_complex *out = (gr_complex *) output_items[0];
      const gr_complex*in = (const gr_complex*) input_items[0];
      
      if (ninput_items[0]) {
        // Check if we have an entire packet.
        long packet_length = 0;
        std::vector<gr_tag_t> tags;
        // Get any tags associated with the first item on the input.
        this->get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+1);
        for (unsigned int j = 0; j < tags.size(); j++) {
          if (pmt::symbol_to_string(tags[j].key) == d_lengthtagname) {
            packet_length = pmt::to_long(tags[j].value);
          }
        }
        if (!packet_length) {
          throw std::runtime_error("no tag");
        }
        if (ninput_items[0] < packet_length ) {
          // We don't have enough input to produce a packet.
          // Produces zeros instead.
        } else {
          // We have enough input.
          if (noutput_items < packet_length) {
            // But we don't have enough output space.
            // We don't want to produce zeros, so return.
            set_output_multiple(packet_length);
            return 0;
          } else {
            // And we have enough output space.
            // Produce the packet.
            std::vector<gr_tag_t> tags;
            this->get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0)+packet_length);
            for (unsigned int j = 0; j < tags.size(); j++) {
              const uint64_t offset = tags[j].offset - nitems_read(0) + nitems_written(0);
              this->add_item_tag(0, offset, tags[j].key, tags[j].value);
            }
            if (noutput_items < packet_length) {
              throw std::runtime_error("Not enough room in the output buffer.");
            }
            memcpy(out, in, packet_length*sizeof(gr_complex));
            consume(0, packet_length);
            return packet_length;
          }
        }
      }
      // We're just producing zeros.
      // Either we have no input data, or not an entire packet yet.
      for (int i=0; i<noutput_items; i++) {
        out[i] = 0;
      }
      return noutput_items;
    }


  } /* namespace digital */
} /* namespace gr */

