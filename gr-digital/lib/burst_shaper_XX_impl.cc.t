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

#include <boost/format.hpp>
#include <gnuradio/io_signature.h>
#include <volk/volk.h>
#include "@IMPL_NAME@.h"

#ifndef VOLK_MULT_gr_complex
#define VOLK_MULT_gr_complex volk_32fc_x2_multiply_32fc
#endif
#ifndef VOLK_MULT_float
#define VOLK_MULT_float volk_32f_x2_multiply_32f
#endif

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
        d_up_ramp(taps.begin(), taps.begin() + taps.size()/2 + taps.size()%2),
        d_down_ramp(taps.begin() + taps.size()/2, taps.end()),
        d_nprepad(pre_padding),
        d_npostpad(post_padding),
        d_insert_phasing(insert_phasing),
        d_length_tag_key(pmt::string_to_symbol(length_tag_name)),
        d_ncopy(0),
        d_limit(0),
        d_index(0),
        d_length_tag_offset(0),
        d_finished(false),
        d_state(STATE_WAIT)
    {
        assert(d_up_ramp.size() == d_down_ramp.size());

        d_up_phasing.resize(d_up_ramp.size());
        d_down_phasing.resize(d_down_ramp.size());

        @I_TYPE@ symbol;
        for(unsigned int i = 0; i < d_up_ramp.size(); i++) {
            symbol = (i%2 == 0) ? @I_TYPE@(1.0f) : @I_TYPE@(-1.0f);
            d_up_phasing[i] = symbol * d_up_ramp[i];
            d_down_phasing[i] = symbol * d_down_ramp[i];
        }

        //set_relative_rate(1.0);
        set_tag_propagation_policy(TPP_DONT);
    }

    @IMPL_NAME@::~@IMPL_NAME@()
    {
    }

    void
    @IMPL_NAME@::forecast(int noutput_items,
                          gr_vector_int &ninput_items_required)
    {
      switch(d_state) {
      case(STATE_RAMPDOWN):
        // If inserting phasing; we don't need any input
        if(d_insert_phasing) {
          ninput_items_required[0] = 0;
        }
        else {
          ninput_items_required[0] = noutput_items;
        }
        break;
      case(STATE_POSTPAD):
        // Padding 0's requires no input
        ninput_items_required[0] = 0;
        break;
      default:
        ninput_items_required[0] = noutput_items;
      }
    }

    int
    @IMPL_NAME@::general_work(int noutput_items,
                      gr_vector_int &ninput_items,
                      gr_vector_const_void_star &input_items,
                      gr_vector_void_star &output_items)
    {
        const @I_TYPE@ *in = reinterpret_cast<const @I_TYPE@ *>(input_items[0]);
        @O_TYPE@ *out = reinterpret_cast<@O_TYPE@ *>(output_items[0]);

        int nwritten = 0;
        int nread = 0;
        int nspace = 0;
        int nskip = 0;
        int curr_tag_index = 0;

        std::vector<tag_t> length_tags;
        get_tags_in_window(length_tags, 0, 0, ninput_items[0], d_length_tag_key);
        std::sort(length_tags.rbegin(), length_tags.rend(), tag_t::offset_compare);

        while(nwritten < noutput_items) {
            // Only check the nread condition if we are actually reading
            // from the input stream.
            if(((d_state != STATE_RAMPDOWN) && (d_state != STATE_POSTPAD)) ||
               ((d_state == STATE_RAMPDOWN) && !d_insert_phasing))
              {
                if(nread >= ninput_items[0]) {
                    break;
                }
            }

            if(d_finished) {
                d_finished = false;
                break;
            }
            nspace = noutput_items - nwritten;
            switch(d_state) {
                case(STATE_WAIT):
                    if(!length_tags.empty()) {
                        d_length_tag_offset = length_tags.back().offset;
                        curr_tag_index = (int)(d_length_tag_offset - nitems_read(0));
                        d_ncopy = pmt::to_long(length_tags.back().value);
                        length_tags.pop_back();
                        nskip = curr_tag_index - nread;
                        add_length_tag(nwritten);
                        propagate_tags(curr_tag_index, nwritten, 1, false);
                        enter_prepad();
                    }
                    else {
                        nskip = ninput_items[0] - nread;
                    }
                    if(nskip > 0) {
                        GR_LOG_WARN(d_logger,
                                    boost::format("Dropping %1% samples") %
                                    nskip);
                        nread += nskip;
                        in += nskip;
                    }
                    break;

                case(STATE_PREPAD):
                    write_padding(out, nwritten, nspace);
                    if(d_index == d_limit)
                        enter_rampup();
                    break;

                case(STATE_RAMPUP):
                    apply_ramp(out, in, nwritten, nread, nspace);
                    if(d_index == d_limit)
                        enter_copy();
                    break;

                case(STATE_COPY):
                    copy_items(out, in, nwritten, nread, nspace);
                    if(d_index == d_limit)
                        enter_rampdown();
                    break;

                case(STATE_RAMPDOWN):
                    apply_ramp(out, in, nwritten, nread, nspace);
                    if(d_index == d_limit)
                        enter_postpad();
                    break;

                case(STATE_POSTPAD):
                    write_padding(out, nwritten, nspace);
                    if(d_index == d_limit)
                        enter_wait();
                    break;

                default:
                    throw std::runtime_error("@BASE_NAME@: invalid state");
            }
        }

        consume_each(nread);

        return nwritten;
    }

    int
    @IMPL_NAME@::prefix_length() const
    {
        return (d_insert_phasing) ?
               d_nprepad + d_up_ramp.size() : d_nprepad;
    }

    int
    @IMPL_NAME@::suffix_length() const
    {
        return (d_insert_phasing) ?
               d_npostpad + d_down_ramp.size() : d_npostpad;
    }

    void
    @IMPL_NAME@::write_padding(@O_TYPE@ *&dst, int &nwritten, int nspace)
    {
        int nprocess = std::min(d_limit - d_index, nspace);
        std::memset(dst, 0x00, nprocess * sizeof(@O_TYPE@));
        dst += nprocess;
        nwritten += nprocess;
        d_index += nprocess;
    }

    void
    @IMPL_NAME@::copy_items(@O_TYPE@ *&dst, const @I_TYPE@ *&src, int &nwritten,
                            int &nread, int nspace)
    {
        int nprocess = std::min(d_limit - d_index, nspace);
        propagate_tags(nread, nwritten, nprocess);
        std::memcpy(dst, src, nprocess * sizeof(@O_TYPE@));
        dst += nprocess;
        nwritten += nprocess;
        src += nprocess;
        nread += nprocess;
        d_index += nprocess;
    }

    void
    @IMPL_NAME@::apply_ramp(@O_TYPE@ *&dst, const @I_TYPE@ *&src, int &nwritten,
                            int &nread, int nspace)
    {
        int nprocess = std::min(d_limit - d_index, nspace);
        @O_TYPE@ *phasing;
        const @O_TYPE@ *ramp;

        if(d_state == STATE_RAMPUP) {
            phasing = &d_up_phasing[d_index];
            ramp = &d_up_ramp[d_index];
        }
        else {
            phasing = &d_down_phasing[d_index];
            ramp = &d_down_ramp[d_index];
        }

        if(d_insert_phasing)
            std::memcpy(dst, phasing, nprocess * sizeof(@O_TYPE@));
        else {
            propagate_tags(nread, nwritten, nprocess);
            VOLK_MULT_@O_TYPE@(dst, src, ramp, nprocess);
            src += nprocess;
            nread += nprocess;
        }

        dst += nprocess;
        nwritten += nprocess;
        d_index += nprocess;
    }

    void
    @IMPL_NAME@::add_length_tag(int offset)
    {
        add_item_tag(0, nitems_written(0) + offset, d_length_tag_key,
                     pmt::from_long(d_ncopy + prefix_length() +
                                    suffix_length()),
                     pmt::string_to_symbol(name()));
    }

    void
    @IMPL_NAME@::propagate_tags(int in_offset, int out_offset, int count, bool skip)
    {
        uint64_t abs_start = nitems_read(0) + in_offset;
        uint64_t abs_end = abs_start + count;
        uint64_t abs_offset = nitems_written(0) + out_offset;
        tag_t temp_tag;

        std::vector<tag_t> tags;
        std::vector<tag_t>::iterator it;

        get_tags_in_range(tags, 0, abs_start, abs_end);

        for(it = tags.begin(); it != tags.end(); it++) {
            if(!pmt::equal(it->key, d_length_tag_key)) {
                if(skip && (it->offset == d_length_tag_offset))
                    continue;
                temp_tag = *it;
                temp_tag.offset = abs_offset + it->offset - abs_start;
                add_item_tag(0, temp_tag);
            }
        }
    }

    void
    @IMPL_NAME@::enter_wait()
    {
        d_finished = true;
        d_index = 0;
        d_state = STATE_WAIT;
    }

    void
    @IMPL_NAME@::enter_prepad()
    {
        d_limit = d_nprepad;
        d_index = 0;
        d_state = STATE_PREPAD;
    }

    void
    @IMPL_NAME@::enter_rampup()
    {
        if(d_insert_phasing)
            d_limit = d_up_ramp.size();
        else
            d_limit = std::min((size_t)(d_ncopy/2), d_up_ramp.size());
        d_index = 0;
        d_state = STATE_RAMPUP;
    }

    void
    @IMPL_NAME@::enter_copy()
    {
        if(d_insert_phasing)
            d_limit = d_ncopy;
        else
            d_limit = d_ncopy - std::min((size_t)((d_ncopy/2)*2),
                                         d_up_ramp.size() +
                                         d_down_ramp.size());
        d_index = 0;
        d_state = STATE_COPY;
    }

    void
    @IMPL_NAME@::enter_rampdown()
    {
        if(d_insert_phasing)
            d_limit = d_down_ramp.size();
        else
            d_limit = std::min((size_t)(d_ncopy/2), d_down_ramp.size());
        d_index = 0;
        d_state = STATE_RAMPDOWN;
    }

    void
    @IMPL_NAME@::enter_postpad()
    {
        d_limit = d_npostpad;
        d_index = 0;
        d_state = STATE_POSTPAD;
    }

  } /* namespace digital */
} /* namespace gr */
