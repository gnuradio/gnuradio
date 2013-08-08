/* -*- c++ -*- */
/*
 * Copyright 2006,2012 Free Software Foundation, Inc.
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

#ifndef INCLUDED_PAGER_FLEX_PARSE_IMPL_H
#define INCLUDED_PAGER_FLEX_PARSE_IMPL_H

#include <gnuradio/pager/flex_parse.h>
#include <gnuradio/sync_block.h>
#include <gnuradio/msg_queue.h>
#include "flex_modes.h"
#include <sstream>

namespace gr {
  namespace pager {

#define FIELD_DELIM ((unsigned char)128)

    class flex_parse_impl : public flex_parse
    {
    private:
      std::ostringstream d_payload;
      msg_queue::sptr d_queue;		  // Destination for decoded pages

      int d_count;	                  // Count of received codewords
      int d_datawords[88];                // 11 blocks of 8 32-bit words

      page_type_t d_type;	  	  // Current page type
      int d_capcode;	                  // Current page destination address
      bool d_laddr;	                  // Current page has long address
      float d_freq;			  // Channel frequency

      void parse_data();	    	  // Handle a frame's worth of data
      void parse_capcode(int32_t aw1, int32_t aw2);
      void parse_alphanumeric(int mw1, int mw2, int j);
      void parse_numeric(int mw1, int mw2, int j);
      void parse_tone_only();
      void parse_unknown(int mw1, int mw2);

    public:
      flex_parse_impl(msg_queue::sptr queue, float freq);
      ~flex_parse_impl();

      int work(int noutput_items,
	       gr_vector_const_void_star &input_items,
	       gr_vector_void_star &output_items);
    };

  } /* namespace pager */
} /* namespace gr */

#endif /* INCLUDED_PAGER_FLEX_PARSE_IMPL_H */
