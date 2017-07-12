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

#include "correlate_access_code_ff_ts_impl.h"
#include <gnuradio/io_signature.h>
#include <gnuradio/math.h>
#include <boost/format.hpp>
#include <stdexcept>
#include <volk/volk.h>
#include <cstdio>
#include <iostream>

namespace gr {
  namespace digital {

    correlate_access_code_ff_ts::sptr
    correlate_access_code_ff_ts::make(const std::string &access_code,
                                      int threshold,
                                      const std::string &tag_name)
    {
      return gnuradio::get_initial_sptr
	(new correlate_access_code_ff_ts_impl(access_code,
                                              threshold, tag_name));
    }

    correlate_access_code_ff_ts_impl::correlate_access_code_ff_ts_impl(
      const std::string &access_code, int threshold, const std::string &tag_name)
      : block("correlate_access_code_ff_ts",
              io_signature::make(1, 1, sizeof(float)),
              io_signature::make(1, 1, sizeof(float))),
	d_data_reg(0), d_mask(0),
	d_threshold(threshold), d_len(0)
    {
      set_tag_propagation_policy(TPP_DONT);

      if(!set_access_code(access_code)) {
	GR_LOG_ERROR(d_logger, "access_code is > 64 bits");
	throw std::out_of_range ("access_code is > 64 bits");
      }

      std::stringstream str;
      str << name() << unique_id();
      d_me = pmt::string_to_symbol(str.str());
      d_key = pmt::string_to_symbol(tag_name);

      d_state = STATE_SYNC_SEARCH;
      d_pkt_len = 0;
      d_pkt_count = 0;
      d_hdr_reg = 0;
      d_hdr_count = 0;
    }

    correlate_access_code_ff_ts_impl::~correlate_access_code_ff_ts_impl()
    {
    }

    bool
    correlate_access_code_ff_ts_impl::set_access_code(const std::string &access_code)
    {
      d_len = access_code.length();	// # of bytes in string
      if(d_len > 64)
        return false;

      // set len least significant bits to 1.
      d_mask = ((~0ULL) >> (64 - d_len));

      d_access_code = 0;
      for(unsigned i=0; i < d_len; i++){
        d_access_code = (d_access_code << 1) | (access_code[i] & 1);
      }

      GR_LOG_DEBUG(d_logger, boost::format("Access code: %llx") % d_access_code);
      GR_LOG_DEBUG(d_logger, boost::format("Mask: %llx") % d_mask);

      return true;
    }

    unsigned long long
    correlate_access_code_ff_ts_impl::access_code() const
    {
      return d_access_code;
    }

    inline void
    correlate_access_code_ff_ts_impl::enter_search()
    {
      d_state = STATE_SYNC_SEARCH;
    }

    inline void
    correlate_access_code_ff_ts_impl::enter_have_sync()
    {
      d_state = STATE_HAVE_SYNC;
      d_hdr_reg = 0;
      d_hdr_count = 0;
    }

    inline void
    correlate_access_code_ff_ts_impl::enter_have_header(int payload_len)
    {
      d_state = STATE_HAVE_HEADER;
      d_pkt_len = 8*payload_len;
      d_pkt_count = 0;
    }

    bool
    correlate_access_code_ff_ts_impl::header_ok()
    {
      // confirm that two copies of header info are identical
      return ((d_hdr_reg >> 16) ^ (d_hdr_reg & 0xffff)) == 0;
    }

    int
    correlate_access_code_ff_ts_impl::header_payload()
    {
      return (d_hdr_reg >> 16) & 0x0fff;
    }

    int
    correlate_access_code_ff_ts_impl::general_work(int noutput_items,
                                                   gr_vector_int &ninput_items,
                                                   gr_vector_const_void_star &input_items,
                                                   gr_vector_void_star &output_items)
    {
      const float *in = (const float*)input_items[0];
      float *out = (float*)output_items[0];

      uint64_t abs_out_sample_cnt = nitems_written(0);

      int nprod = 0;

      int count = 0;
      while(count < noutput_items) {
        switch(d_state) {
	case STATE_SYNC_SEARCH:    // Look for the access code correlation

	  while(count < noutput_items) {
            // shift in new data
            d_data_reg = (d_data_reg << 1) | (gr::branchless_binary_slicer(in[count++]) & 0x1);

            // compute hamming distance between desired access code and current data
            uint64_t wrong_bits = 0;
            uint64_t nwrong = d_threshold+1;

            wrong_bits = (d_data_reg ^ d_access_code) & d_mask;
            volk_64u_popcnt(&nwrong, wrong_bits);

            if(nwrong <= d_threshold) {
              enter_have_sync();
              break;
            }
          }
          break;

	case STATE_HAVE_SYNC:
	  while(count < noutput_items) {    // Shift bits one at a time into header
            d_hdr_reg = (d_hdr_reg << 1) | (gr::branchless_binary_slicer(in[count++]) & 0x1);
            d_hdr_count++;

            if(d_hdr_count == 32) {
	      // we have a full header, check to see if it has been received properly
	      if(header_ok()) {
		int payload_len = header_payload();
		enter_have_header(payload_len);
              }
	      else {
		enter_search();    // bad header
              }
              break;
            }
          }
          break;

	case STATE_HAVE_HEADER:
          if(d_pkt_count == 0) {
            // MAKE A TAG OUT OF THIS AND UPDATE OFFSET
            add_item_tag(0,                             // stream ID
                         abs_out_sample_cnt + nprod,    // sample
                         d_key,                         // length key
                         pmt::from_long(d_pkt_len),     // length data
                         d_me);                         // block src id
          }

	  while(count < noutput_items) {
            if(d_pkt_count < d_pkt_len) {
              out[nprod++] = in[count++];
              d_pkt_count++;
            }
            else {
              enter_search();
              break;
            }
          }
          break;
        }
      }

      consume_each(noutput_items);
      return nprod;
    }

  } /* namespace digital */
} /* namespace gr */
