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
        d_up_flank(taps.begin(), taps.begin() + taps.size()/2 + taps.size()%2),
        d_down_flank(taps.begin() + taps.size()/2, taps.end()),
        d_nprepad(pre_padding),
        d_npostpad(post_padding),
        d_insert_phasing(insert_phasing),
        d_length_tag_key(pmt::string_to_symbol(length_tag_name)),
        d_state(STATE_WAITING)
    {
        assert(d_up_flank.size() == d_down_flank.size());

        d_up_phasing.resize(d_up_flank.size());
        d_down_phasing.resize(d_up_flank.size());
        if(d_insert_phasing) {
            @I_TYPE@ symbol;
            for(unsigned int i = 0; i < d_up_flank.size(); i++) {
                symbol = (i%2) ? @I_TYPE@(1.0f) : @I_TYPE@(-1.0f);
                d_up_phasing.push_back(symbol * d_up_flank[i]);
                d_down_phasing.push_back(symbol * d_down_flank[i]);
            }
        }

        set_relative_rate(1.0);
        set_tag_propagation_policy(TPP_DONT);
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
        int nstart = 0;
        int nstop = 0;
        int nremaining = 0;
        int nprocessed = 0;
        uint64_t curr_tag_index = nitems_read(0);

        std::vector<tag_t> tags;
        get_tags_in_window(tags, 0, 0, ninput_items[0]);
	std::sort(tags.begin(), tags.end(), tag_t::offset_compare);

        while((nwritten < noutput_items) && (nread < ninput_items[0])) {
            nremaining = noutput_items - nwritten;
            switch(d_state) {
                case(STATE_WAITING):
                    curr_tag_index = tags[0].offset;
                    d_nremaining = pmt::to_long(tags[0].value) +
                                   prefix_length() + suffix_length();
                    nprocessed += (int)curr_tag_index;  // drop orphaned samples
                    add_length_tag(nwritten);
                    enter_prepad();
                    break;

                case(STATE_PREPAD):
		    std::memset(out, 0x00, nprocess * sizeof(@O_TYPE@));
                    break;

                case(STATE_RAMPUP):
                    nprocess = std::min(
                    if(d_insert_phasing) {
                        
                    }
                    break;

                case(STATE_COPY):
                    std::memcpy(out, in, nprocess * sizeof(@O_TYPE@));
                    break;

                case(STATE_RAMPDOWN):
                    break;

                case(STATE_POSTPAD):
		    std::memset(out, 0x00, nprocess * sizeof(@O_TYPE@));
                    break;

                default:
                    throw std::runtime_error("burst_shaper: invalid state reached");
            }
        }

        consume_each (nconsumed);

        // Tell runtime system how many output items we produced.
        return noutput_items;
    }

    void
    @IMPL_NAME@::add_length_tag(int offset)
    {
        add_item_tag(0, nitems_written(0) + offset, d_length_tag_key,
                     pmt::from_long(d_nremaining),
                     pmt::string_to_symbol(name()));
    }

    void
    @IMPL_NAME@::propagate_tags(std::vector<tag_t> &tags, int offset)
    {
        // FIXME: need to handle offsets correctly
        std::vector<tag_t>::iterator tag;
        for(tag = tags.begin(); tag != tags.end(); tag++) {
            tag_t new_tag = *tag;
            new_tag.offset = nitems_written(0) + offset;
            add_item_tag(0, new_tag);
        }
    }
  } /* namespace digital */
} /* namespace gr */
