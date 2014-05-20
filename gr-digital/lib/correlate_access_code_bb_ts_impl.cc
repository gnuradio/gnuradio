/* -*- c++ -*- */
/*
 * Copyright 2014 Free Software Foundation, Inc.
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

#include "correlate_access_code_bb_ts_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>
#include <volk/volk.h>
#include <cstdio>
#include <iostream>

namespace gr {
  namespace digital {

#define VERBOSE 0

    correlate_access_code_bb_ts::sptr
    correlate_access_code_bb_ts::make(const std::string &access_code,
				       int threshold,
				       const std::string &tag_name)
    {
      return gnuradio::get_initial_sptr
	(new correlate_access_code_bb_ts_impl(access_code,
					       threshold, tag_name));
    }


    correlate_access_code_bb_ts_impl::correlate_access_code_bb_ts_impl(
        const std::string &access_code, int threshold, const std::string &tag_name)
      : block("correlate_access_code_bb_ts",
              io_signature::make(1, 1, sizeof(char)),
              io_signature::make(1, 1, sizeof(char))),
	d_data_reg(0), d_mask(0),
	d_threshold(threshold), d_len(0)
    {
      set_tag_propagation_policy(TPP_DONT);

      if(!set_access_code(access_code)) {
	throw std::out_of_range ("access_code is > 64 bits");
      }

      std::stringstream str;
      str << name() << unique_id();
      d_me = pmt::string_to_symbol(str.str());
      d_key = pmt::string_to_symbol(tag_name);

      // READ IN AS ARGS; MAKE SETTERS/GETTERS
      d_pkt_key = pmt::string_to_symbol("pkt_len");
      d_pkt_len = 120*8;
      d_pkt_count = 0;
    }

    correlate_access_code_bb_ts_impl::~correlate_access_code_bb_ts_impl()
    {
    }

    bool
    correlate_access_code_bb_ts_impl::set_access_code(
        const std::string &access_code)
    {
      d_len = access_code.length();	// # of bytes in string
      if(d_len > 64)
        return false;

      // set len top bits to 1.
      d_mask = ((~0ULL) >> (64 - d_len)) << (64 - d_len);

      d_access_code = 0;
      for(unsigned i=0; i < d_len; i++){
        d_access_code = (d_access_code << 1) | (access_code[i] & 1);
      }
      if(VERBOSE) {
          std::cerr << "Access code: " << std::hex << d_access_code << std::dec << std::endl;
          std::cerr << "Mask: " << std::hex << d_mask << std::dec << std::endl;
      }

      return true;
    }

    int
    correlate_access_code_bb_ts_impl::general_work(int noutput_items,
                                                   gr_vector_int &ninput_items,
                                                   gr_vector_const_void_star &input_items,
                                                   gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char*)input_items[0];
      unsigned char *out = (unsigned char*)output_items[0];

      uint64_t abs_out_sample_cnt = nitems_written(0);

      int nprod = 0;

      for(int i = 0; i < noutput_items; i++) {
        if(d_pkt_count > 0) {
          out[nprod] = in[i];
          d_pkt_count--;
          nprod++;

          if(d_pkt_count == 0) {
            add_item_tag(0,
                         abs_out_sample_cnt + i,
                         pmt::intern("STOP"),
                         pmt::from_long(abs_out_sample_cnt + nprod),
                         d_me);
          }
        }
        else {

          // compute hamming distance between desired access code and current data
          uint64_t wrong_bits = 0;
          uint64_t nwrong = d_threshold+1;

          wrong_bits  = (d_data_reg ^ d_access_code) & d_mask;
          volk_64u_popcnt(&nwrong, wrong_bits);

          // shift in new data
          d_data_reg = (d_data_reg << 1) | (in[i] & 0x1);
          if(nwrong <= d_threshold) {
            if(VERBOSE)
              std::cerr << "writing tag at sample " << abs_out_sample_cnt + i << std::endl;
            add_item_tag(0,                      // stream ID
                         abs_out_sample_cnt + nprod, // sample
                         d_key,                  // frame info
                         pmt::from_long(nwrong), // data (number wrong)
                         d_me);                  // block src id

            // MAKE A TAG OUT OF THIS AND UPDATE OFFSET
            add_item_tag(0,                         // stream ID
                         abs_out_sample_cnt + nprod,    // sample
                         d_pkt_key,                 // length key
                         pmt::from_long(d_pkt_len), // length data
                         d_me);                     // block src id
            d_pkt_count = d_pkt_len;
            d_data_reg = 0;
          }
        }
      }

      //std::cerr << "Producing data: " << nprod << std::endl;
      consume_each(noutput_items);
      return nprod;
    }

  } /* namespace digital */
} /* namespace gr */

