/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
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

#include <boost/format.hpp>
#include <gnuradio/io_signature.h>
#include "packet_headerparser_b_impl.h"

namespace gr {
  namespace digital {

    packet_headerparser_b::sptr
    packet_headerparser_b::make(long header_len, const std::string &len_tag_key)
    {
      const packet_header_default::sptr header_formatter(
	  new packet_header_default(header_len, len_tag_key)
      );
      return gnuradio::get_initial_sptr (new packet_headerparser_b_impl(header_formatter));
    }

    packet_headerparser_b::sptr
    packet_headerparser_b::make(const gr::digital::packet_header_default::sptr &header_formatter)
    {
      return gnuradio::get_initial_sptr (new packet_headerparser_b_impl(header_formatter));
    }

    packet_headerparser_b_impl::packet_headerparser_b_impl(const gr::digital::packet_header_default::sptr &header_formatter)
      : sync_block("packet_headerparser_b",
		      io_signature::make(1, 1, sizeof (unsigned char)),
		      io_signature::make(0, 0, 0)),
        d_header_formatter(header_formatter),
        d_port(pmt::mp("header_data"))
    {
      message_port_register_out(d_port);
      set_output_multiple(header_formatter->header_len());
    }

    packet_headerparser_b_impl::~packet_headerparser_b_impl()
    {
    }

    int
    packet_headerparser_b_impl::work(int noutput_items,
			  gr_vector_const_void_star &input_items,
			  gr_vector_void_star &output_items)
    {
      const unsigned char *in = (const unsigned char *) input_items[0];

      if (noutput_items < d_header_formatter->header_len()) {
	return 0;
      }

      std::vector<tag_t> tags;
      get_tags_in_range(
	  tags, 0,
	  nitems_read(0),
	  nitems_read(0)+d_header_formatter->header_len()
      );

      if (!d_header_formatter->header_parser(in, tags)) {
	GR_LOG_INFO(d_logger, boost::format("Detected an invalid packet at item %1%") % nitems_read(0));
	message_port_pub(d_port, pmt::PMT_F);
      } else {
	pmt::pmt_t dict(pmt::make_dict());
	for (unsigned i = 0; i < tags.size(); i++) {
	  dict = pmt::dict_add(dict, tags[i].key, tags[i].value);
	}
	message_port_pub(d_port, dict);
      }

      return d_header_formatter->header_len();
    }

  } /* namespace digital */
} /* namespace gr */

