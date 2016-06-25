/* -*- c++ -*- */
/*
 * Copyright 2016 Free Software Foundation, Inc.
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

#include <volk/volk.h>
#include <gnuradio/io_signature.h>
#include "protocol_formatter_bb_impl.h"
#include <stdio.h>

namespace gr {
  namespace digital {

    protocol_formatter_bb::sptr
    protocol_formatter_bb::make(const header_format_base::sptr &format,
                                const std::string &len_tag_key)
    {
      return gnuradio::get_initial_sptr
        (new protocol_formatter_bb_impl(format, len_tag_key));
    }

    protocol_formatter_bb_impl::protocol_formatter_bb_impl(const header_format_base::sptr &format,
                                                           const std::string &len_tag_key)
      : tagged_stream_block("protocol_formatter_bb",
                            io_signature::make(1, 1, sizeof(char)),
                            io_signature::make(1, 1, sizeof(char)),
                            len_tag_key),
        d_format(format)
    {
      set_output_multiple(d_format->header_nbytes());

      // This is the worst case rate, because we don't know the true value, of course
      set_relative_rate(d_format->header_nbytes());
      set_tag_propagation_policy(TPP_DONT);
    }

    protocol_formatter_bb_impl::~protocol_formatter_bb_impl()
    {
    }

    void
    protocol_formatter_bb_impl::set_header_format(header_format_base::sptr &format)
    {
      gr::thread::scoped_lock guard(d_setlock);
      d_format = format;
    }

    int
    protocol_formatter_bb_impl::work(int noutput_items,
                                     gr_vector_int &ninput_items,
                                     gr_vector_const_void_star &input_items,
                                     gr_vector_void_star &output_items)
    {
      gr::thread::scoped_lock guard(d_setlock);
      unsigned char *out = (unsigned char *) output_items[0];
      const unsigned char *in = (const unsigned char *) input_items[0];

      // Not really sure what to do with these tags; extract them and
      // plug them into the info dictionary?
      //std::vector<tag_t> tags;
      //get_tags_in_window(tags, 0, 0, ninput_items[0]);

      pmt::pmt_t pmt_out;
      pmt::pmt_t info = pmt::PMT_NIL;
      if(!d_format->format(ninput_items[0], in, pmt_out, info)) {
	GR_LOG_FATAL(d_logger, boost::format("header format returned false "
                                             "(this shouldn't happen). Offending "
                                             "header started at %1%") % nitems_read(0));
	throw std::runtime_error("header format returned false.");
      }

      size_t len;
      const uint8_t *data = pmt::u8vector_elements(pmt_out, len);
      if(len != d_format->header_nbytes()) {
        throw std::runtime_error("Header format got wrong size header");
      }
      memcpy(out, data, len);

      if(pmt::is_dict(info)) {
        std::vector<tag_t> mtags;
        pmt::pmt_t mkeys = pmt::dict_keys(info);
        pmt::pmt_t mvals = pmt::dict_values(info);
        for(size_t i = 0; i < pmt::length(mkeys); i++) {
          tag_t tag;
          tag.offset = nitems_written(0);
          tag.key = pmt::nth(i, mkeys);
          tag.value = pmt::nth(i, mvals);
          tag.srcid = alias_pmt();
          add_item_tag(0, tag);
        }
      }

      return len;
    }

  } /* namespace digital */
} /* namespace gr */
