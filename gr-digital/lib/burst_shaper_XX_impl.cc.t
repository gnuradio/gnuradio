/* -*- c++ -*- */
/* 
 * Copyright 2015 Free Software Foundation, Inc.
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

/* @WARNING@ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "@IMPL_NAME@.h"

namespace gr {
  namespace digital {

    @BASE_NAME@::sptr
    @BASE_NAME@::make(const std::vector<@I_TYPE@> &taps,
                      int pre_padding, int post_padding,
                      bool insert_phasing,
                      const std::string &length_tag_name)
    {
      return gnuradio::get_initial_sptr
        (new @IMPL_NAME@(taps, pre_padding, post_padding,
                         insert_phasing, length_tag_name));
    }

    @IMPL_NAME@::@IMPL_NAME@(const std::vector<@I_TYPE@> &taps,
                             int pre_padding, int post_padding,
                             bool insert_phasing,
                             const std::string &length_tag_name)
      : gr::block("@BASE_NAME@",
              gr::io_signature::make(1, 1, sizeof(@I_TYPE@)),
              gr::io_signature::make(1, 1, sizeof(@O_TYPE@))),
        d_upflank(taps.begin(), taps.begin() + taps.size()/2 + taps.size()%2),
        d_downflank(taps.begin() + taps.size()/2, taps.end()),
        d_nprepad(pre_padding),
        d_npostpad(post_padding),
        d_insert_phasing(insert_phasing),
        d_length_tag_key(pmt::string_to_symbol(length_tag_name)),
        d_state(STATE_WAITING)
    {
        if(d_insert_phasing)
            for(unsigned int i = 0; i < d_upflank.size(); i++)
                d_phasing.push_back(i%2 ? @I_TYPE@(1.0f) : @I_TYPE@(-1.0f));
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }

    void
    @IMPL_NAME@::forecast(int noutput_items, gr_vector_int &ninput_items_required)
    {
        ninput_items_required[0] = noutput_items;
    }

    int
    @IMPL_NAME@::general_work(int noutput_items,
                      gr_vector_int &ninput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
        const @I_TYPE@ *in = (const @I_TYPE@ *) input_items[0];
        @O_TYPE@ *out = (@O_TYPE@ *) output_items[0];

        int nwritten = 0;
        int nread = 0;
        int curr_tag_index = 0;
        int nprocess = 0;

        std::vector<tag_t> tags;
        get_tags_in_window(tags, 0, 0, ninput_items[0]);
	std::sort(tags.begin(), tags.end(), tag_t::offset_compare);

        //TODO: figure out what to do with tag gaps - should probably just drop
        while((nwritten < noutput_items) && (nread < ninput_items[0])) {
            switch(d_state) {
                case(STATE_WAITING):
                case(STATE_PREPAD):
		    std::memset(out, 0x00, nprocess * sizeof(@O_TYPE@));
                case(STATE_RAMPUP):
                case(STATE_COPY):
                    std::memcpy(out, in, nprocess * sizeof(@O_TYPE@));
                case(STATE_RAMPDOWN):
                case(STATE_POSTPAD):
		    std::memset(out, 0x00, nprocess * sizeof(@O_TYPE@));
            }
        }

        consume_each (noutput_items);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

  } /* namespace digital */
} /* namespace gr */
