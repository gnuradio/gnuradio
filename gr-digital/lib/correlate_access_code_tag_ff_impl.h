/* -*- c++ -*- */
/*
 * Copyright 2005,2006,2011,2012,2017 Free Software Foundation, Inc.
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

#ifndef INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_IMPL_H
#define INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_IMPL_H

#include <gnuradio/digital/correlate_access_code_tag_ff.h>

namespace gr {
  namespace digital {

    class correlate_access_code_tag_ff_impl :
      public correlate_access_code_tag_ff
    {
    private:
      unsigned long long d_access_code;	// access code to locate start of packet
                                        //   access code is left justified in the word
      unsigned long long d_data_reg;	// used to look for access_code
      unsigned long long d_mask;	// masks access_code bits (top N bits are set where
                                        //   N is the number of bits in the access code)
      unsigned int d_threshold; 	// how many bits may be wrong in sync vector
      unsigned int d_len;               // the length of the access code

      pmt::pmt_t d_key, d_me; //d_key is the tag name, d_me is the block name + unique ID

      gr::thread::mutex d_mutex_access_code;

    public:
      correlate_access_code_tag_ff_impl(const std::string &access_code,
					int threshold,
					const std::string &tag_name);
      ~correlate_access_code_tag_ff_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);

      bool set_access_code(const std::string &access_code);
      void set_threshold(int threshold) { d_threshold = threshold; };
      void set_tagname(const std::string &tag_name) { d_key = pmt::string_to_symbol(tag_name); };
    };

  } /* namespace digital */
} /* namespace gr */

#endif /* INCLUDED_DIGITAL_CORRELATE_ACCESS_CODE_TAG_FF_IMPL_H */
