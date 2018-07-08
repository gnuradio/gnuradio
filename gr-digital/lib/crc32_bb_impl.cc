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

#include <gnuradio/io_signature.h>
#include "crc32_bb_impl.h"

namespace gr {
  namespace digital {

    crc32_bb::sptr
    crc32_bb::make(bool check, const std::string &lengthtagname, bool packed) {
      return gnuradio::get_initial_sptr(new crc32_bb_impl(check, lengthtagname, packed));
    }

    crc32_bb_impl::crc32_bb_impl(bool check, const std::string &lengthtagname, bool packed)
        : tagged_stream_block("crc32_bb",
                              io_signature::make(1, 1, sizeof(char)),
                              io_signature::make(1, 1, sizeof(char)),
                              lengthtagname),
          d_check(check), d_packed(packed){
      d_crc_length = 4;
      if (!d_packed) {
        d_crc_length = 32;
        d_buffer = std::vector<char>(d_crc_length);
      }else{
        d_buffer = std::vector<char>(4096);
      }
      set_tag_propagation_policy(TPP_DONT);
    }

    crc32_bb_impl::~crc32_bb_impl() {
    }

    int
    crc32_bb_impl::calculate_output_stream_length(const gr_vector_int &ninput_items) {
      if (d_check) {
        return ninput_items[0] - d_crc_length;
      } else {
        return ninput_items[0] + d_crc_length;
      }
    }

    unsigned int
    crc32_bb_impl::calculate_crc32(const unsigned char* in, size_t packet_length){
      unsigned int crc = 0;
      d_crc_impl.reset();
      if (!d_packed){
        const size_t n_packed_length = 1 + ((packet_length - 1) / 8);
        if (n_packed_length > d_buffer.size()){
          d_buffer.resize(n_packed_length);
          }
        std::fill(d_buffer.begin(), d_buffer.begin() + n_packed_length, 0);
        for (size_t bit = 0; bit < packet_length; bit++){
          d_buffer[bit/8] |= (in[bit] << (bit % 8));
        }
        d_crc_impl.process_bytes(&d_buffer[0], n_packed_length);
        crc = d_crc_impl();
      } else{
        d_crc_impl.process_bytes(in, packet_length);
        crc = d_crc_impl();
      }
      return crc;
    }

    int
    crc32_bb_impl::work(int noutput_items,
                        gr_vector_int &ninput_items,
                        gr_vector_const_void_star &input_items,
                        gr_vector_void_star &output_items) {
      const unsigned char *in = (const unsigned char *) input_items[0];
      unsigned char *out = (unsigned char *) output_items[0];
      size_t packet_length = ninput_items[0];
      int packet_size_diff = d_check ? -d_crc_length : d_crc_length;
      unsigned int crc;

      if (d_check) {
        if (packet_length <= d_crc_length){
          return 0;
        }
        d_crc_impl.process_bytes(in, packet_length - d_crc_length);
        crc = calculate_crc32(in, packet_length - d_crc_length);
        if (d_packed) {
          if (crc != *(unsigned int *) (in + packet_length - d_crc_length)) { // Drop package
            return 0;
          }
        }
        else{
          for(int i=0; i < d_crc_length; i++){
            if(((crc >> i) & 0x1) != *(in + packet_length - d_crc_length + i)) { // Drop package
              return 0;
            }
          }
        }
        memcpy((void *) out, (const void *) in, packet_length - d_crc_length);
      } else {
        crc = calculate_crc32(in, packet_length);
        memcpy((void *) out, (const void *) in, packet_length);
        if (d_packed) {
          memcpy((void *) (out + packet_length), &crc, d_crc_length); // FIXME big-endian/little-endian, this might be wrong
        }
        else {
          for (int i = 0; i < d_crc_length; i++) { // unpack CRC and store in buffer
            d_buffer[i] = (crc >> i) & 0x1;
          }
          memcpy((void *) (out + packet_length), (void *) &d_buffer[0], d_crc_length);
        }
      }

      std::vector <tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0), nitems_read(0) + packet_length);
      for (size_t i = 0; i < tags.size(); i++) {
        tags[i].offset -= nitems_read(0);
        if (d_check && tags[i].offset > (unsigned int) (packet_length + packet_size_diff)) {
          tags[i].offset = packet_length - d_crc_length - 1;
        }
        add_item_tag(0, nitems_written(0) + tags[i].offset,
                     tags[i].key,
                     tags[i].value);
      }

      return packet_length + packet_size_diff;
    }

  } /* namespace digital */
} /* namespace gr */

