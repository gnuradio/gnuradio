/* -*- c++ -*- */
/* Copyright 2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "packet_headerparser_b_impl.h"
#include <gnuradio/io_signature.h>
#include <boost/format.hpp>

namespace gr {
namespace digital {

packet_headerparser_b::sptr packet_headerparser_b::make(long header_len,
                                                        const std::string& len_tag_key)
{
    const packet_header_default::sptr header_formatter(
        new packet_header_default(header_len, len_tag_key));
    return gnuradio::get_initial_sptr(new packet_headerparser_b_impl(header_formatter));
}

packet_headerparser_b::sptr packet_headerparser_b::make(
    const gr::digital::packet_header_default::sptr& header_formatter)
{
    return gnuradio::get_initial_sptr(new packet_headerparser_b_impl(header_formatter));
}

packet_headerparser_b_impl::packet_headerparser_b_impl(
    const gr::digital::packet_header_default::sptr& header_formatter)
    : sync_block("packet_headerparser_b",
                 io_signature::make(1, 1, sizeof(unsigned char)),
                 io_signature::make(0, 0, 0)),
      d_header_formatter(header_formatter),
      d_port(pmt::mp("header_data"))
{
    message_port_register_out(d_port);
    set_output_multiple(header_formatter->header_len());
}

packet_headerparser_b_impl::~packet_headerparser_b_impl() {}

int packet_headerparser_b_impl::work(int noutput_items,
                                     gr_vector_const_void_star& input_items,
                                     gr_vector_void_star& output_items)
{
    const unsigned char* in = (const unsigned char*)input_items[0];

    if (noutput_items < d_header_formatter->header_len()) {
        return 0;
    }

    std::vector<tag_t> tags;
    get_tags_in_range(
        tags, 0, nitems_read(0), nitems_read(0) + d_header_formatter->header_len());

    if (!d_header_formatter->header_parser(in, tags)) {
        GR_LOG_INFO(d_logger,
                    boost::format("Detected an invalid packet at item %1%") %
                        nitems_read(0));
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
